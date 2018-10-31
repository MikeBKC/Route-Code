/** @file

  A brief file description

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */
#include "libts.h"

#include "HttpTransact.h"
#include "HttpTransactHeaders.h"
#include "HTTP.h"
#include "HdrUtils.h"
#include "HttpCompat.h"
#include "HttpMessageBody.h"

#include "I_Machine.h"

bool
HttpTransactHeaders::is_method_cacheable(int method)
{
  return (method == HTTP_WKSIDX_GET || method == HTTP_WKSIDX_HEAD);
}


bool
HttpTransactHeaders::is_method_cache_lookupable(int method)
{
  // responses to GET, HEAD, and POST are cacheable
  // URL's requested in DELETE and PUT are looked up to remove cached copies
  return (method == HTTP_WKSIDX_GET ||
          method == HTTP_WKSIDX_HEAD ||
          method == HTTP_WKSIDX_POST ||
          method == HTTP_WKSIDX_DELETE ||
          method == HTTP_WKSIDX_PUT || method == HTTP_WKSIDX_PURGE || method == HTTP_WKSIDX_PUSH);
}


bool
HttpTransactHeaders::is_this_a_hop_by_hop_header(const char *field_name)
{
  if (!hdrtoken_is_wks(field_name))
    return (false);
  if ((hdrtoken_wks_to_flags(field_name) & MIME_FLAGS_HOPBYHOP) && (field_name != MIME_FIELD_KEEP_ALIVE)) {
    return (true);
  } else {
    return (false);
  }
}


bool
HttpTransactHeaders::is_this_method_supported(int the_scheme, int the_method)
{
  if (the_method == HTTP_WKSIDX_CONNECT) {
    return true;
  } else if (the_scheme == URL_WKSIDX_HTTP || the_scheme == URL_WKSIDX_HTTPS)
    return is_this_http_method_supported(the_method);
  else
    return false;
}


void
HttpTransactHeaders::insert_supported_methods_in_response(HTTPHdr *response, int scheme)
{
  int method_output_lengths[32];
  const char *methods[] = {
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_GET,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_ICP_QUERY,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_POST,
    HTTP_METHOD_PURGE,
    HTTP_METHOD_PUT,
    HTTP_METHOD_PUSH,
    HTTP_METHOD_TRACE,
  };
  char inline_buffer[64];
  char *alloced_buffer, *value_buffer;

  int nmethods = sizeof(methods) / sizeof(methods[0]);
  ink_assert(nmethods <= 32);

  char *p;
  int i, is_supported;
  size_t bytes = 0;
  int num_methods_supported = 0;
  MIMEField *field;

  // step 1: determine supported methods, count bytes & allocate
  for (i = 0; i < nmethods; i++) {
    const char *method_wks = methods[i];
    ink_assert(hdrtoken_is_wks(method_wks));

    is_supported = is_this_method_supported(scheme, hdrtoken_wks_to_index(method_wks));

    if (is_supported) {
      ++num_methods_supported;
      method_output_lengths[i] = hdrtoken_wks_to_length(method_wks);
      bytes += method_output_lengths[i];
      if (num_methods_supported > 1)
        bytes += 2;             // +2 if need leading ", "
    } else {
      method_output_lengths[i] = 0;
    }
  }

  // step 2: create Allow field if not present
  field = response->field_find(MIME_FIELD_ALLOW, MIME_LEN_ALLOW);
  if (!field) {
    field = response->field_create(MIME_FIELD_ALLOW, MIME_LEN_ALLOW);
    response->field_attach(field);
  }
  // step 3: get a big enough buffer
  if (bytes <= sizeof(inline_buffer)) {
    alloced_buffer = NULL;
    value_buffer = inline_buffer;
  } else {
    alloced_buffer = (char *)ats_malloc(bytes);
    value_buffer = alloced_buffer;
  }

  // step 4: build the value
  p = value_buffer;
  for (i = 0; i < nmethods; i++) {
    if (method_output_lengths[i]) {
      memcpy(p, methods[i], method_output_lengths[i]);
      p += method_output_lengths[i];
      if (num_methods_supported > 1) {
        *p++ = ',';
        *p++ = ' ';
      }
      --num_methods_supported;
    }
  }

  // FIXME: do we really want to append to old list, or replace old list?

  // step 5: attach new allow list to end of previous list
  field->value_append(response->m_heap, response->m_mime, value_buffer, bytes);

  // step 6: free up temp storage
  ats_free(alloced_buffer);
}


void
HttpTransactHeaders::build_base_response(HTTPHdr *outgoing_response,
                                         HTTPStatus status,
                                         const char *reason_phrase, int reason_phrase_len, ink_time_t date)
{
  if (!outgoing_response->valid()) {
    outgoing_response->create(HTTP_TYPE_RESPONSE);
  }

  ink_debug_assert(outgoing_response->type_get() == HTTP_TYPE_RESPONSE);

  outgoing_response->version_set(HTTPVersion(1, 1));
  outgoing_response->status_set(status);
  outgoing_response->reason_set(reason_phrase, reason_phrase_len);
  outgoing_response->set_date(date);
}


////////////////////////////////////////////////////////////////////////
// Copy all non hop-by-hop header fields from src_hdr to new_hdr.
// If header Date: is not present or invalid in src_hdr,
// then the given date will be used.
void
HttpTransactHeaders::copy_header_fields(HTTPHdr *src_hdr,
                                        HTTPHdr *new_hdr, bool retain_proxy_auth_hdrs, ink_time_t date)
{
  ink_assert(src_hdr->valid());
  ink_assert(!new_hdr->valid());

  MIMEField *field;
  MIMEFieldIter field_iter;
  bool date_hdr = false;

  // Start with an exact duplicate
  new_hdr->copy(src_hdr);

  // Nuke hop-by-hop headers
  //
  //    The hop-by-hop header fields are layed out by the spec
  //    with two adjustments
  //      1) we treat TE as hop-by-hop because spec implies
  //         that it is by declaring anyone who sends a TE must
  //         include TE in the connection header.  This in
  //         my opinion error prone and if the client doesn't follow the spec
  //         we'll have problems with the TE being forwarded to the server
  //         and us caching the transfer encoded documents and then
  //         serving it to a client that can not handle it
  //      2) Transfer enconding is copied.  If the transfer encoding
  //         is changed for example by dechunking, the transfer encoding
  //         should be modified when when the decision is made to dechunk it

  for (field = new_hdr->iter_get_first(&field_iter); field != NULL; field = new_hdr->iter_get_next(&field_iter)) {
    if (field->m_wks_idx == -1)
      continue;

    int field_flags = hdrtoken_index_to_flags(field->m_wks_idx);

    if (field_flags & MIME_FLAGS_HOPBYHOP) {
      // Delete header if not in special proxy_auth retention mode
      if ((!retain_proxy_auth_hdrs) || (!(field_flags & MIME_FLAGS_PROXYAUTH))) {
        new_hdr->field_delete(field);
      }
    } else if (field->m_wks_idx == MIME_WKSIDX_DATE) {
      date_hdr = true;
    }
  }

  // Set date hdr if not already set and valid value passed in
  if ((date_hdr == false) && (date > 0))
    new_hdr->set_date(date);
}


////////////////////////////////////////////////////////////////////////
// Just convert the outgoing request to the appropriate version
void
HttpTransactHeaders::convert_request(HTTPVersion outgoing_ver, HTTPHdr *outgoing_request)
{
  if (outgoing_ver == HTTPVersion(1, 0)) {
    convert_to_1_0_request_header(outgoing_request);
  } else if (outgoing_ver == HTTPVersion(1, 1)) {
    convert_to_1_1_request_header(outgoing_request);
  } else if (outgoing_ver == HTTPVersion(0, 9)) {
    // Http 0.9 is a special case - do not bother copying over fields,
    // because they will all need to be removed anyway.
    convert_to_0_9_request_header(outgoing_request);
  } else {
    Debug("http_trans", "[HttpTransactHeaders::convert_request]" "Unsupported Version - passing through");
  }
}

////////////////////////////////////////////////////////////////////////
// Just convert the outgoing response to the appropriate version
void
HttpTransactHeaders::convert_response(HTTPVersion outgoing_ver, HTTPHdr *outgoing_response)
{
  if (outgoing_ver == HTTPVersion(1, 0)) {
    convert_to_1_0_response_header(outgoing_response);
  } else if (outgoing_ver == HTTPVersion(1, 1)) {
    convert_to_1_1_response_header(outgoing_response);
  } else if (outgoing_ver == HTTPVersion(0, 9)) {
    // Http 0.9 is a special case - do not bother copying over fields,
    // because they will all need to be removed anyway.
    convert_to_0_9_response_header(outgoing_response);
  } else {
    Debug("http_trans", "[HttpTransactHeaders::convert_response]" "Unsupported Version - passing through");
  }
}


////////////////////////////////////////////////////////////////////////
// Take an existing outgoing request header and make it HTTP/0.9
void
HttpTransactHeaders::convert_to_0_9_request_header(HTTPHdr *outgoing_request)
{
  // These are required
  ink_debug_assert(outgoing_request->method_get_wksidx() == HTTP_WKSIDX_GET);
  ink_debug_assert(outgoing_request->url_get()->valid());

  outgoing_request->version_set(HTTPVersion(0, 9));

  // HTTP/0.9 has no headers: nuke them all
  outgoing_request->fields_clear();
}


////////////////////////////////////////////////////////////////////////
// Take an existing outgoing request header and make it HTTP/1.0
void
HttpTransactHeaders::convert_to_1_0_request_header(HTTPHdr *outgoing_request)
{
  // These are required
  ink_debug_assert(outgoing_request->url_get()->valid());

  // Set HTTP version to 1.0
  outgoing_request->version_set(HTTPVersion(1, 0));

  // FIXME (P2): Need to change cache directives into pragma, cleanly
  //             Now, any Cache-Control hdr becomes Pragma: no-cache

  if (outgoing_request->presence(MIME_PRESENCE_CACHE_CONTROL) && !outgoing_request->is_pragma_no_cache_set()) {
    outgoing_request->value_append(MIME_FIELD_PRAGMA, MIME_LEN_PRAGMA, "no-cache", 8, true);
  }
  // We do not currently support chunked transfer encoding,
  // so specify that response should use identity transfer coding.
  //outgoing_request->value_insert(MIME_FIELD_TE, "identity;q=1.0");
  //outgoing_request->value_insert(MIME_FIELD_TE, "chunked;q=0.0");
}


////////////////////////////////////////////////////////////////////////
// Take an existing outgoing request header and make it HTTP/1.1
void
HttpTransactHeaders::convert_to_1_1_request_header(HTTPHdr *outgoing_request)
{

  // These are required
  ink_debug_assert(outgoing_request->url_get()->valid());
  ink_debug_assert(outgoing_request->version_get() == HTTPVersion(1, 1));

  if (outgoing_request->get_cooked_pragma_no_cache() &&
      !(outgoing_request->get_cooked_cc_mask() & MIME_COOKED_MASK_CC_NO_CACHE)) {
    outgoing_request->value_append(MIME_FIELD_CACHE_CONTROL, MIME_LEN_CACHE_CONTROL, "no-cache", 8, true);
  }
  // We do not currently support chunked transfer encoding,
  // so specify that response should use identity transfer coding.
  //outgoing_request->value_insert(MIME_FIELD_TE, "identity;q=1.0");
  //outgoing_request->value_insert(MIME_FIELD_TE, "chunked;q=0.0");
}


////////////////////////////////////////////////////////////////////////
// Take an existing outgoing response header and make it HTTP/0.9
void
HttpTransactHeaders::convert_to_0_9_response_header(HTTPHdr *outgoing_response)
{
  NOWARN_UNUSED(outgoing_response);
  // Http 0.9 does not require a response header.

  // There used to be clear header here, but the state machine
  // does not write down the response header if the client is
  // 0.9. We need the header fields to make decisions about
  // the size of the response body, however.
  // There is therefore no need to clear the header.
}


////////////////////////////////////////////////////////////////////////
// Take an existing outgoing response header and make it HTTP/1.0
void
HttpTransactHeaders::convert_to_1_0_response_header(HTTPHdr *outgoing_response)
{
//     // These are required
//     ink_debug_assert(outgoing_response->status_get());
//     ink_debug_assert(outgoing_response->reason_get());

  // Set HTTP version to 1.0
  outgoing_response->version_set(HTTPVersion(1, 0));

  // Keep-Alive?

  // Cache-Control?
}


////////////////////////////////////////////////////////////////////////
// Take an existing outgoing response header and make it HTTP/1.1
void
HttpTransactHeaders::convert_to_1_1_response_header(HTTPHdr *outgoing_response)
{
  // These are required
  ink_debug_assert(outgoing_response->status_get());

  // Set HTTP version to 1.1
//    ink_debug_assert(outgoing_response->version_get() == HTTPVersion (1, 1));
  outgoing_response->version_set(HTTPVersion(1, 1));
}


///////////////////////////////////////////////////////////////////////////////
// Name       : calculate_document_age()
// Description: returns age of document
//
// Input      :
// Output     : ink_time_t age
//
// Details    :
//   Algorithm is straight out of March 1998 1.1 specs, Section 13.2.3
//
///////////////////////////////////////////////////////////////////////////////
ink_time_t
HttpTransactHeaders::calculate_document_age(ink_time_t request_time,
                                            ink_time_t response_time,
                                            HTTPHdr *base_response, ink_time_t base_response_date, ink_time_t now)
{
  ink_time_t age_value = base_response->get_age();
  ink_time_t date_value = 0;
  ink_time_t apparent_age = 0;
  ink_time_t corrected_received_age = 0;
  ink_time_t response_delay = 0;
  ink_time_t corrected_initial_age = 0;
  ink_time_t current_age = 0;
  ink_time_t resident_time = 0;
  ink_time_t now_value = 0;

  ink_time_t tmp_value = 0;

  tmp_value = base_response_date;
  date_value = (tmp_value > 0) ? tmp_value : 0;

  // Deal with clock skew. Sigh.
  //
  // TODO solve this global clock problem
  now_value = max(now, response_time);

  ink_debug_assert(response_time >= 0);
  ink_debug_assert(request_time >= 0);
  ink_debug_assert(response_time >= request_time);
  ink_debug_assert(now_value >= response_time);

  if (date_value > 0) {
    apparent_age = max((time_t) 0, (response_time - date_value));
  }
  if (age_value < 0) {
    current_age = -1; // Overflow from Age: header
  } else {
    corrected_received_age = max(apparent_age, age_value);
    response_delay = response_time - request_time;
    corrected_initial_age = corrected_received_age + response_delay;
    resident_time = now_value - response_time;
    current_age = corrected_initial_age + resident_time;
  }

  Debug("http_age", "[calculate_document_age] age_value:              %" PRId64, (int64_t)age_value);
  Debug("http_age", "[calculate_document_age] date_value:             %" PRId64, (int64_t)date_value);
  Debug("http_age", "[calculate_document_age] response_time:          %" PRId64, (int64_t)response_time);
  Debug("http_age", "[calculate_document_age] now:                    %" PRId64, (int64_t)now);
  Debug("http_age", "[calculate_document_age] now (fixed):            %" PRId64, (int64_t)now_value);
  Debug("http_age", "[calculate_document_age] apparent_age:           %" PRId64, (int64_t)apparent_age);
  Debug("http_age", "[calculate_document_age] corrected_received_age: %" PRId64, (int64_t)corrected_received_age);
  Debug("http_age", "[calculate_document_age] response_delay:         %" PRId64, (int64_t)response_delay);
  Debug("http_age", "[calculate_document_age] corrected_initial_age:  %" PRId64, (int64_t)corrected_initial_age);
  Debug("http_age", "[calculate_document_age] resident_time:          %" PRId64, (int64_t)resident_time);
  Debug("http_age", "[calculate_document_age] current_age:            %" PRId64, (int64_t)current_age);

  return current_age;
}


bool
HttpTransactHeaders::does_server_allow_response_to_be_stored(HTTPHdr *resp)
{
  uint32_t cc_mask = (MIME_COOKED_MASK_CC_NO_CACHE | MIME_COOKED_MASK_CC_NO_STORE | MIME_COOKED_MASK_CC_PRIVATE);

  if ((resp->get_cooked_cc_mask() & cc_mask) || (resp->get_cooked_pragma_no_cache()))
    return false;
  else
    return true;
}


bool
HttpTransactHeaders::downgrade_request(bool *origin_server_keep_alive, HTTPHdr *outgoing_request)
{
  //HTTPVersion ver;
  /* First try turning keep_alive off */
  if (*origin_server_keep_alive) {

    *origin_server_keep_alive = false;
    //ver.set(outgoing_request->version_get());

  }

  if (outgoing_request->version_get() == HTTPVersion(1, 1)) {

    //ver.set (HTTPVersion (1, 0));
    convert_to_1_0_request_header(outgoing_request);

    // bz48199: only GET requests can be downgraded to HTTP/0.9
  } else if (outgoing_request->version_get() == HTTPVersion(1, 0) &&
             outgoing_request->method_get_wksidx() == HTTP_WKSIDX_GET) {

    //ver.set (HTTPVersion (0, 9));
    convert_to_0_9_request_header(outgoing_request);

  } else {
    return false;
  }

  return true;
}
bool
HttpTransactHeaders::get_wuts_code(HTTPHdr *hdr, WUTSCode *w)
{
#define is_alnum(c)  (ParseRules::is_alnum (c))
#define is_digit(c)  (ParseRules::is_digit (c))

  const char *phrase;
  int length;
  int i, j;

  phrase = hdr->reason_get(&length);
  if (!phrase || (length == 0) || (phrase[0] != '!'))
    return false;

  if (length < 25)
    return (false);

  if (!is_alnum(phrase[1]))
    return false;

  w->squid_hit_miss_code = (SquidHitMissCode) phrase[1];

  for (i = 0, j = 2; i < 5; i++) {
    if (!is_alnum(phrase[1]))
      return false;
    w->squid_log_code[i] = (SquidLogCode) phrase[j];
    j += 1;

    if (!is_alnum(phrase[1]))
      return false;
    w->squid_hier_code[i] = (SquidHierarchyCode) phrase[j];
    j += 1;
  }

  for (i = 0; i < 5; i++) {
    if ((phrase[j] != '0') || (phrase[j + 1] != '0'))
      return false;
    w->proxy_id[i] = 0;
    j += 2;
  }

  if (!is_digit(phrase[j + 0]) || !is_digit(phrase[j + 1]) || !is_digit(phrase[j + 2]))
    return false;

  w->proxy_status_code =
    (WUTSProxyStatusCode) ((phrase[j + 0] - '0') * 100 + (phrase[j + 1] - '0') * 10 + (phrase[j + 2] - '0'));

  return true;

#undef is_alnum
#undef is_digit
}


void
HttpTransactHeaders::set_wuts_codes(HTTPHdr *hdr, WUTSCode *code)
{
#define is_digit(c)  (ParseRules::is_digit (c))
#define is_hex(c)    (ParseRules::is_hex (c))

  char phrase[1024];
  const char *old_phrase;
  int old_length;
  int i, j, phrase_len;

  old_phrase = hdr->reason_get(&old_length);
  if (old_length > 0) {
    if ((old_length >= 25) && (old_phrase[0] == '!') &&
        is_hex(old_phrase[1]) && is_hex(old_phrase[2]) &&
        is_hex(old_phrase[3]) && is_hex(old_phrase[4]) &&
        is_hex(old_phrase[5]) && is_hex(old_phrase[6]) &&
        is_hex(old_phrase[7]) && is_hex(old_phrase[8]) &&
        is_hex(old_phrase[9]) && is_hex(old_phrase[10]) &&
        is_hex(old_phrase[11]) && is_hex(old_phrase[12]) &&
        is_hex(old_phrase[13]) && is_hex(old_phrase[14]) &&
        is_hex(old_phrase[15]) && is_hex(old_phrase[16]) &&
        is_hex(old_phrase[17]) && is_hex(old_phrase[18]) &&
        is_hex(old_phrase[19]) && is_hex(old_phrase[20]) &&
        is_hex(old_phrase[21]) && is_digit(old_phrase[22]) && is_digit(old_phrase[23]) && is_digit(old_phrase[24])) {
      old_phrase += 25;
      old_length -= 25;

      if ((old_length >= 1) && (old_phrase[0] == ' ')) {
        old_phrase += 1;
        old_length -= 1;
      }
    }

    if (old_length > (1023 - 25))
      old_length = 1023 - 25;

    phrase[0] = '!';
    phrase[25] = ' ';
    memcpy(&phrase[26], old_phrase, old_length);
    phrase[26 + old_length] = '\0';
    phrase_len = 26 + old_length;
  } else {
    phrase[0] = '!';
    phrase[25] = '\0';
    phrase_len = 25;
  }

  phrase[1] = code->squid_hit_miss_code;

  for (i = 0, j = 2; i < 5; i++) {
    phrase[j++] = code->squid_log_code[i];
    phrase[j++] = code->squid_hier_code[i];
  }

  for (i = 0; i < 5; i++) {
    phrase[j++] = ((code->proxy_id[i] / 10) % 10) + '0';
    phrase[j++] = (code->proxy_id[i] % 10) + '0';
  }

  phrase[j++] = ((code->proxy_status_code / 100) % 10) + '0';
  phrase[j++] = ((code->proxy_status_code / 10) % 10) + '0';
  phrase[j++] = (code->proxy_status_code % 10) + '0';

  hdr->reason_set(phrase, phrase_len);

#undef is_hex
#undef is_digit
}


void
HttpTransactHeaders::set_wuts_codes(HTTPHdr *hdr,
                                    SquidHitMissCode hit_miss_code,
                                    SquidLogCode log_code,
                                    SquidHierarchyCode hier_code,
                                    WUTSProxyId proxy_id, WUTSProxyStatusCode proxy_status_code)
{
  WUTSCode code;
  int i;

  if (!get_wuts_code(hdr, &code)) {
    for (i = 0; i < 5; i++) {
      code.squid_log_code[i] = SQUID_LOG_EMPTY;
      code.squid_hier_code[i] = SQUID_HIER_EMPTY;
      code.proxy_id[i] = 0;
    }
  }

  for (i = 0; i < 4; i++) {
    if ((code.squid_log_code[i] == SQUID_LOG_EMPTY) &&
        (code.squid_hier_code[i] == SQUID_HIER_EMPTY) && (code.proxy_id[i] == 0)) {
      break;
    }
  }

  code.squid_hit_miss_code = hit_miss_code;
  code.squid_log_code[i] = log_code;
  code.squid_hier_code[i] = hier_code;
  code.proxy_id[i] = proxy_id;
  code.proxy_status_code = proxy_status_code;

  set_wuts_codes(hdr, &code);
}


void
HttpTransactHeaders::generate_and_set_wuts_codes(HTTPHdr *header,
                                                 char *via_string,
                                                 HttpTransact::SquidLogInfo *squid_codes,
                                                 int wuts_id, bool set_codes_in_hdr, bool log_spider_codes)
{
  SquidLogCode log_code;
  SquidHierarchyCode hier_code;
  SquidHitMissCode hit_miss_code;
  WUTSProxyStatusCode status_code = WUTS_PROXY_STATUS_UNKNOWN;

  /////////////////////////////
  // First the Hit-Miss Code //
  /////////////////////////////
  if ((via_string[VIA_DETAIL_CACHE_LOOKUP] == VIA_DETAIL_HIT_CONDITIONAL) ||
      (via_string[VIA_DETAIL_CACHE_LOOKUP] == VIA_DETAIL_MISS_CONDITIONAL) ||
      (via_string[VIA_DETAIL_CACHE_LOOKUP] == VIA_DETAIL_HIT_SERVED)) {
    // its a cache hit.
    // INKqa10331
    hit_miss_code = SQUID_HIT_RESERVED;
  } else {
    int reason_len;
    const char *reason = header->reason_get(&reason_len);
    // INKqa10331
    if (reason != NULL && reason_len >= 24 && reason[0] == '!' && reason[1] == SQUID_HIT_RESERVED)
      hit_miss_code = SQUID_HIT_RESERVED;
    // its a miss in the cache. find out why.
    else if (via_string[VIA_DETAIL_CACHE_LOOKUP] == VIA_DETAIL_MISS_EXPIRED) {
      hit_miss_code = SQUID_MISS_PRE_EXPIRED;
    } else if (via_string[VIA_DETAIL_CACHE_LOOKUP] == VIA_DETAIL_MISS_CONFIG) {
      hit_miss_code = SQUID_MISS_NONE;
    } else if (via_string[VIA_DETAIL_CACHE_LOOKUP] == VIA_DETAIL_MISS_CLIENT) {
      hit_miss_code = SQUID_MISS_PRAGMA_NOCACHE;
    } else if (via_string[VIA_DETAIL_CACHE_LOOKUP] == VIA_DETAIL_MISS_METHOD) {
      hit_miss_code = SQUID_MISS_HTTP_NON_CACHE;
    } else if (via_string[VIA_CLIENT_REQUEST] == VIA_CLIENT_ERROR) {
      hit_miss_code = SQUID_MISS_ERROR;
    } else if (via_string[VIA_CLIENT_REQUEST] == VIA_CLIENT_NO_CACHE) {
      hit_miss_code = SQUID_MISS_PRAGMA_NOCACHE;
    } else {
      hit_miss_code = SQUID_MISS_NONE;
    }
  }

  //////////////////////
  // Now the Log Code //
  //////////////////////
  if (via_string[VIA_CLIENT_REQUEST] == VIA_CLIENT_NO_CACHE) {
    log_code = SQUID_LOG_TCP_CLIENT_REFRESH;
  }

  else {
    if (via_string[VIA_CLIENT_REQUEST] == VIA_CLIENT_IMS) {
      if ((via_string[VIA_CACHE_RESULT] == VIA_IN_CACHE_FRESH) ||
          (via_string[VIA_CACHE_RESULT] == VIA_IN_RAM_CACHE_FRESH)) {
        log_code = SQUID_LOG_TCP_IMS_HIT;
      } else {
        if (via_string[VIA_SERVER_RESULT] == VIA_SERVER_NOT_MODIFIED) {
          log_code = SQUID_LOG_TCP_REFRESH_HIT;
        } else {
          log_code = SQUID_LOG_TCP_IMS_MISS;
        }
      }
    }

    else {
      if (via_string[VIA_CACHE_RESULT] == VIA_IN_CACHE_STALE) {
        if (via_string[VIA_SERVER_RESULT] == VIA_SERVER_NOT_MODIFIED) {
          log_code = SQUID_LOG_TCP_REFRESH_HIT;
        } else {
          if (via_string[VIA_SERVER_RESULT] == VIA_SERVER_ERROR) {
            log_code = SQUID_LOG_TCP_REF_FAIL_HIT;
          } else {
            log_code = SQUID_LOG_TCP_REFRESH_MISS;
          }
        }
      } else {
        if (via_string[VIA_CACHE_RESULT] == VIA_IN_CACHE_FRESH) {
          log_code = SQUID_LOG_TCP_HIT;
        } else if (via_string[VIA_CACHE_RESULT] == VIA_IN_RAM_CACHE_FRESH) {
          log_code = SQUID_LOG_TCP_MEM_HIT;
        } else {
          log_code = SQUID_LOG_TCP_MISS;
        }
      }
    }
  }

  ////////////////////////
  // The Hierarchy Code //
  ////////////////////////
  if ((via_string[VIA_CACHE_RESULT] == VIA_IN_CACHE_FRESH) || (via_string[VIA_CACHE_RESULT] == VIA_IN_RAM_CACHE_FRESH)) {
    hier_code = SQUID_HIER_NONE;
  } else if (via_string[VIA_DETAIL_ICP_CONNECT] == VIA_DETAIL_ICP_SUCCESS) {
    hier_code = SQUID_HIER_SIBLING_HIT;
  } else if (via_string[VIA_DETAIL_PP_CONNECT] == VIA_DETAIL_PP_SUCCESS) {
    hier_code = SQUID_HIER_PARENT_HIT;
  } else if (via_string[VIA_DETAIL_CACHE_TYPE] == VIA_DETAIL_PARENT) {
    hier_code = SQUID_HIER_DEFAULT_PARENT;
  } else if (via_string[VIA_DETAIL_TUNNEL] == VIA_DETAIL_TUNNEL_NO_FORWARD) {
    hier_code = SQUID_HIER_NONE;
  } else {
    hier_code = SQUID_HIER_DIRECT;
  }

  if (log_spider_codes && hier_code == SQUID_HIER_DIRECT)
    log_code = SQUID_LOG_TCP_SPIDER_BYPASS;

  /////////////////////
  // The Status Code //
  /////////////////////
  if (via_string[VIA_CLIENT_REQUEST] == VIA_CLIENT_IMS) {
    if ((via_string[VIA_CACHE_RESULT] == VIA_IN_CACHE_FRESH) ||
        (via_string[VIA_CACHE_RESULT] == VIA_IN_RAM_CACHE_FRESH)) {
      status_code = WUTS_PROXY_STATUS_IMS_RECEIVED_CACHE_HIT;
    }
  } else if (via_string[VIA_CLIENT_REQUEST] == VIA_CLIENT_SIMPLE) {
    if ((via_string[VIA_CACHE_RESULT] == VIA_IN_CACHE_FRESH) ||
        (via_string[VIA_CACHE_RESULT] == VIA_IN_RAM_CACHE_FRESH)) {
      status_code = WUTS_PROXY_STATUS_GET_RECEIVED_CACHE_HIT;
    }
  }
  // Errors may override the other codes, so check the via string error codes last
  switch (via_string[VIA_ERROR_TYPE]) {
  case VIA_ERROR_AUTHORIZATION:
    status_code = WUTS_PROXY_STATUS_PROXY_AUTHORIZATION_FAILURE;
    // TODO decide which one?
    // log_code = SQUID_LOG_TCP_DENIED;
    log_code = SQUID_LOG_ERR_PROXY_DENIED;
    break;
  case VIA_ERROR_CONNECTION:
    status_code = log_spider_codes ? WUTS_PROXY_STATUS_SPIDER_CONNECTION_FAILED : WUTS_PROXY_STATUS_CONNECT_FAILED;
    if (log_code == SQUID_LOG_TCP_MISS) {
      log_code = log_spider_codes ? SQUID_LOG_ERR_SPIDER_CONNECT_FAILED : SQUID_LOG_ERR_CONNECT_FAIL;
    }
    break;
  case VIA_ERROR_DNS_FAILURE:
    status_code = log_spider_codes ? WUTS_PROXY_STATUS_SPIDER_DNS_HOST_NOT_FOUND : WUTS_PROXY_STATUS_DNS_LOOKUP_FAILURE;
    log_code = log_spider_codes ? SQUID_LOG_ERR_SPIDER_DNS_HOST_NOT_FOUND : SQUID_LOG_ERR_DNS_FAIL;
    hier_code = SQUID_HIER_NONE;
    break;
  case VIA_ERROR_FORBIDDEN:
    status_code = WUTS_PROXY_STATUS_FORBIDDEN;
    log_code = SQUID_LOG_ERR_PROXY_DENIED;
    break;
  case VIA_ERROR_HEADER_SYNTAX:
    status_code = WUTS_PROXY_STATUS_INVALID_REQUEST;
    log_code = SQUID_LOG_ERR_INVALID_REQ;
    hier_code = SQUID_HIER_NONE;
    break;
  case VIA_ERROR_SERVER:
    status_code = log_spider_codes ? WUTS_PROXY_STATUS_SPIDER_CONNECTION_FAILED : WUTS_PROXY_STATUS_CONNECT_FAILED;
    if (log_code == SQUID_LOG_TCP_MISS || log_code == SQUID_LOG_TCP_MISS) {
      log_code = log_spider_codes ? SQUID_LOG_ERR_SPIDER_CONNECT_FAILED : SQUID_LOG_ERR_CONNECT_FAIL;
    }
    break;
  case VIA_ERROR_TIMEOUT:
    status_code = WUTS_PROXY_STATUS_READ_TIMEOUT;
    if (log_code == SQUID_LOG_TCP_MISS || log_code == SQUID_LOG_TCP_IMS_MISS) {
      log_code = log_spider_codes ? SQUID_LOG_ERR_SPIDER_GENERAL_TIMEOUT : SQUID_LOG_ERR_READ_TIMEOUT;
    }
    if (hier_code == SQUID_HIER_SIBLING_HIT) {
      hier_code = SQUID_HIER_TIMEOUT_SIBLING_HIT;
    } else if (hier_code == SQUID_HIER_PARENT_HIT) {
      hier_code = SQUID_HIER_TIMEOUT_PARENT_HIT;
    } else {
      hier_code = SQUID_HIER_TIMEOUT_DIRECT;
    }
    break;
  case VIA_ERROR_CACHE_READ:
    // policy decision to call this a disk io error
    // in the absence of a corresponding status code
    // for TCP_SWAPFAIL. it seems as if disk io goes
    // better with the ERR_DISK_IO log code, but
    // beep it. i am sick of second guessing wuts codes.
    status_code = WUTS_PROXY_STATUS_DISK_IO_ERROR;
    log_code = SQUID_LOG_TCP_SWAPFAIL;
    hier_code = SQUID_HIER_NONE;
    break;
  default:
    break;
  }

  if (status_code == WUTS_PROXY_STATUS_UNKNOWN) {
    status_code = (WUTSProxyStatusCode) header->status_get();
  }

  Debug("http_trans",
        "[WUTS code generation] Hit/Miss: %d, Log: %d, Hier: %d, Status: %d",
        hit_miss_code, log_code, hier_code, status_code);
  squid_codes->log_code = log_code;
  squid_codes->hier_code = hier_code;

  if (hit_miss_code != SQUID_MISS_NONE) {
    squid_codes->hit_miss_code = hit_miss_code;
  } else {
    if (squid_codes->hit_miss_code == SQUID_MISS_NONE) {
      squid_codes->hit_miss_code = hit_miss_code;
    } else {
      hit_miss_code = squid_codes->hit_miss_code;
    }
  }

  if (status_code != WUTS_PROXY_STATUS_UNKNOWN) {
    squid_codes->wuts_proxy_status_code = status_code;
  }

  if (set_codes_in_hdr) {
    set_wuts_codes(header, hit_miss_code, log_code, hier_code, wuts_id, status_code);
  }
}


void
HttpTransactHeaders::convert_wuts_code_to_normal_reason(HTTPHdr *hdr)
{
  const char *phrase;
  int length;

  if (hdr == NULL)
    return;

  phrase = hdr->reason_get(&length);
  if (!phrase || (length == 0 || (phrase[0] != '!')))
    return;

  phrase = (const char *) HttpMessageBody::StatusCodeName(hdr->status_get());
  hdr->reason_set(phrase, strlen(phrase));
}


void
HttpTransactHeaders::handle_conditional_headers(HttpTransact::CacheLookupInfo *cache_info, HTTPHdr *header)
{
  if (cache_info->action == HttpTransact::CACHE_DO_UPDATE) {
    HTTPHdr *c_response = cache_info->object_read->response_get();

    // wouldn't be updating cache for range requests (would be writing)
    uint64_t mask = (MIME_PRESENCE_RANGE | MIME_PRESENCE_IF_RANGE);
    ink_release_assert(header->presence(mask) == mask);

    /*
     * Conditional Headers
     *  We use the if-modified-since and if-none-match headers in conjunction
     * whenever an etag and last-modified time were supplied in the original
     * response.
     *
     *  If no last-modified time was sent we use the date value
     *
     *  It is safe to use both the etag revalidation and last-modified
     * revalidation together since 1.1 servers will user the etags correctly
     * and 1.0 servers will ignore them, using instead the weaker validator.
     */

    /*
     *  Here we override whatever modified since time might have been
     * sent with the client. If there comes a time when the client(s) are
     * not using a given cache primarily then we may want to create a
     * special optimized path for this case.
     */
    if (c_response->presence(MIME_PRESENCE_LAST_MODIFIED)) {
      header->set_if_modified_since(c_response->get_last_modified());
    }

    /*
     * ETags
     *  If-None-Match has the semantics of If-Modified-Since for opaque
     * etag token.
     */

    MIMEField *old_field;
    old_field = c_response->field_find(MIME_FIELD_ETAG, MIME_LEN_ETAG);
    if (old_field) {
      MIMEField *new_field;

      new_field = header->field_find(MIME_FIELD_ETAG, MIME_LEN_ETAG);
      if (new_field == NULL) {
        new_field = header->field_create(MIME_FIELD_ETAG, MIME_LEN_ETAG);
        header->field_attach(new_field);
      }

      int len;
      const char *str = old_field->value_get(&len);
      header->field_value_set(new_field, str, len);
    }
  }
}


void
HttpTransactHeaders::insert_warning_header(HttpConfigParams *http_config_param, HTTPHdr *header, HTTPWarningCode code,
                                           const char *warn_text, int warn_text_len)
{
  int bufsize, len;

  // + 23 for 20 possible digits of warning code (long long max
  //  digits) & 2 spaces & the string terminator
  bufsize = http_config_param->proxy_response_via_string_len + 23;
  if (warn_text != NULL)
    bufsize += warn_text_len;
  else
    warn_text_len = 0; // Make sure it's really zero

  char *warning_text = (char *)alloca(bufsize);

  len = snprintf(warning_text, bufsize, "%3d %s %.*s", code, http_config_param->proxy_response_via_string, warn_text_len, warn_text);
  header->value_set(MIME_FIELD_WARNING, MIME_LEN_WARNING, warning_text, len);
}


void
HttpTransactHeaders::insert_time_and_age_headers_in_response(ink_time_t request_sent_time,
                                                             ink_time_t response_received_time,
                                                             ink_time_t now, HTTPHdr *base, HTTPHdr *outgoing)
{
  ink_time_t date = base->get_date();
  ink_time_t current_age = calculate_document_age(request_sent_time, response_received_time, base, date, now);

  outgoing->set_age(current_age); // set_age() deals with overflow properly, so pass it along

  // FIX: should handle missing date when response is received, not here.
  //      See INKqa09852.
  if (date <= 0)
    outgoing->set_date(now);
}


void
HttpTransactHeaders::insert_server_header_in_response(const char *server_tag, int server_tag_size, HTTPHdr *h)
{
  if (likely(server_tag && server_tag_size > 0 && h)) {
    h->set_server(server_tag, server_tag_size);
  }
}


///////////////////////////////////////////////////////////////////////////////
// Name       : insert_via_header_in_request
// Description: takes in existing via_string and inserts it in header
//
// Input      :
// Output     :
//
// Details    :
//
// [u<client-stuff> l<cache-lookup-stuff> o<server-stuff> f<cache-fill-stuff> p<proxy-stuff>]
//
//      client stuff
//              I       IMS
//              N       no-cache
//              A       accept headers
//              C       cookie
//
//      cache lookup stuff
//              M       miss
//              A       in cache, not acceptable
//              S       in cache, stale
//              H       in cache, fresh
//
//      server stuff
//              N       not-modified
//              S       served
//
//      cache fill stuff
//              F       filled into cache
//              U       updated cache
//
//      proxy stuff
//              N       not-modified
//              S       served
//              R       origin server revalidated
//
// For example:
//
//      [u lH o f pS]      cache hit
//      [u lM oS fF pS]    cache miss
//      [uN l oS f pS]     no-cache origin server fetch
//
//
///////////////////////////////////////////////////////////////////////////////
void
HttpTransactHeaders::insert_via_header_in_request(HttpTransact::State *s, HTTPHdr *header)
{
  char new_via_string[1024]; // 512-bytes for hostname+via string, 512-bytes for the debug info
  char *via_string = new_via_string;

  if ((s->http_config_param->proxy_hostname_len + s->http_config_param->proxy_request_via_string_len) > 512) {
    header->value_append(MIME_FIELD_VIA, MIME_LEN_VIA, "TrafficServer", 13, true);
    return;
  }

  char *incoming_via = s->via_string;
  int scheme = s->orig_scheme;
  ink_assert(scheme >= 0);

  int scheme_len = hdrtoken_index_to_length(scheme);
  int32_t hversion = header->version_get().m_version;

  memcpy(via_string, hdrtoken_index_to_wks(scheme), scheme_len);
  via_string += scheme_len;

  // Common case (I hope?)
  if ((HTTP_MAJOR(hversion) == 1) && HTTP_MINOR(hversion) == 1) {
    memcpy(via_string, "/1.1 ", 5);
    via_string += 5;
  } else {
    *via_string++ = '/';
    *via_string++ = '0' + HTTP_MAJOR(hversion);
    *via_string++ = '.';
    *via_string++ = '0' + HTTP_MINOR(hversion);
    *via_string++ = ' ';
  }
  via_string += nstrcpy(via_string, s->http_config_param->proxy_hostname);

  *via_string++ = '[';
  /* I thought we should use the transaction local outgoing IP address but
     that makes cycle detection (which is the point) unrealiable. We must
     use the same value every time to be sure.
  */
  memcpy(via_string, Machine::instance()->ip_hex_string, Machine::instance()->ip_hex_string_len);
  via_string += Machine::instance()->ip_hex_string_len;
  *via_string++ = ']';
  *via_string++ = ' ';
  *via_string++ = '(';

  memcpy(via_string, s->http_config_param->proxy_request_via_string, s->http_config_param->proxy_request_via_string_len);
  via_string += s->http_config_param->proxy_request_via_string_len;

  if (s->txn_conf->insert_request_via_string > 1) {
    *via_string++ = ' ';
    *via_string++ = '[';

    // incoming_via can be max MAX_VIA_INDICES+1 long (i.e. around 25 or so)
    if (s->txn_conf->insert_request_via_string > 2) { // Highest verbosity
      via_string += nstrcpy(via_string, incoming_via);
    } else {
      memcpy(via_string, incoming_via + VIA_CLIENT, VIA_SERVER - VIA_CLIENT);
      via_string += VIA_SERVER - VIA_CLIENT;
    }
    *via_string++ = ']';
  }

  *via_string++ = ')';
  *via_string = 0;

  ink_assert((size_t)(via_string - new_via_string) < (sizeof(new_via_string) - 1));
  header->value_append(MIME_FIELD_VIA, MIME_LEN_VIA, new_via_string, via_string - new_via_string, true);
}


void
HttpTransactHeaders::insert_via_header_in_response(HttpTransact::State *s, HTTPHdr *header)
{
  char new_via_string[1024]; // 512-bytes for hostname+via string, 512-bytes for the debug info
  char *via_string = new_via_string;

  if ((s->http_config_param->proxy_hostname_len + s->http_config_param->proxy_response_via_string_len) > 512) {
    header->value_append(MIME_FIELD_VIA, MIME_LEN_VIA, "TrafficServer", 13, true);
    return;
  }

  char *incoming_via =  s->via_string;
  int scheme = s->next_hop_scheme;

  ink_assert(scheme >= 0);
  int scheme_len = hdrtoken_index_to_length(scheme);
  int32_t hversion = header->version_get().m_version;

  memcpy(via_string, hdrtoken_index_to_wks(scheme), scheme_len);
  via_string += scheme_len;

  // Common case (I hope?)
  if ((HTTP_MAJOR(hversion) == 1) && HTTP_MINOR(hversion) == 1) {
    memcpy(via_string, "/1.1 ", 5);
    via_string += 5;
  } else {
    *via_string++ = '/';
    *via_string++ = '0' + HTTP_MAJOR(hversion);
    *via_string++ = '.';
    *via_string++ = '0' + HTTP_MINOR(hversion);
    *via_string++ = ' ';
  }
  via_string += nstrcpy(via_string, s->http_config_param->proxy_hostname);
  *via_string++ = ' ';
  *via_string++ = '(';

  memcpy(via_string, s->http_config_param->proxy_response_via_string, s->http_config_param->proxy_response_via_string_len);
  via_string += s->http_config_param->proxy_response_via_string_len;

  if (s->txn_conf->insert_response_via_string > 1) {
    *via_string++ = ' ';
    *via_string++ = '[';

    // incoming_via can be max MAX_VIA_INDICES+1 long (i.e. around 25 or so)
    if (s->txn_conf->insert_response_via_string > 2) { // Highest verbosity
      via_string += nstrcpy(via_string, incoming_via);
    } else {
      memcpy(via_string, incoming_via + VIA_CACHE, VIA_PROXY - VIA_CACHE);
      via_string += VIA_PROXY - VIA_CACHE;
    }
    *via_string++ = ']';
  }

  *via_string++ = ')';
  *via_string = 0;

  ink_assert((size_t)(via_string - new_via_string) < (sizeof(new_via_string) - 1));
  header->value_append(MIME_FIELD_VIA, MIME_LEN_VIA, new_via_string, via_string - new_via_string, true);
}


///////////////////////////////////////////////////////////////////////////////
// Name: insert_basic_realm_in_proxy_authenticate
// Description: insert Basic realm into Proxy-Authenticate based on
//              configuration
//  fix for INKqa09089
///////////////////////////////////////////////////////////////////////////////
void
HttpTransactHeaders::insert_basic_realm_in_proxy_authenticate(const char *realm, HTTPHdr *header, bool bRevPrxy)
{
  char new_basic_realm[128];
  char *basic_realm;

  basic_realm = new_basic_realm;
  basic_realm += nstrcpy(basic_realm, "Basic realm=\"");
  basic_realm += nstrcpy(basic_realm, (char *) realm);
  *basic_realm++ = '"';
  *basic_realm = 0;

  MIMEField *auth;
  if (false == bRevPrxy) {
    auth = header->field_create(MIME_FIELD_PROXY_AUTHENTICATE, MIME_LEN_PROXY_AUTHENTICATE);
  } else {
    auth = header->field_create(MIME_FIELD_WWW_AUTHENTICATE, MIME_LEN_WWW_AUTHENTICATE);
  }

  header->field_value_set(auth, new_basic_realm, strlen(new_basic_realm));
  header->field_attach(auth);
}


inline void
HttpTransactHeaders::process_connection_field_in_outgoing_header(HTTPHdr *base, HTTPHdr *header)
{
  _process_xxx_connection_field_in_outgoing_header(MIME_FIELD_CONNECTION, MIME_LEN_CONNECTION, base, header);
}

inline void
HttpTransactHeaders::process_proxy_connection_field_in_outgoing_header(HTTPHdr *base, HTTPHdr *header)
{
  _process_xxx_connection_field_in_outgoing_header(MIME_FIELD_PROXY_CONNECTION, MIME_LEN_PROXY_CONNECTION, base, header);
}


void
HttpTransactHeaders::process_connection_headers(HTTPHdr *base, HTTPHdr *outgoing)
{
  process_connection_field_in_outgoing_header(base, outgoing);
  process_proxy_connection_field_in_outgoing_header(base, outgoing);
}


void
HttpTransactHeaders::_process_xxx_connection_field_in_outgoing_header(const char *wks_field_name,
                                                                      int wks_field_name_len,
                                                                      HTTPHdr *base, HTTPHdr *header)
{
  MIMEField *con_hdr;
  con_hdr = base->field_find(wks_field_name, wks_field_name_len);

  if (con_hdr) {
    int val_len;
    const char *val;

    if (!con_hdr->has_dups()) { // try fastpath first
      val = con_hdr->value_get(&val_len);
      if ((ptr_len_casecmp(val, val_len, "keep-alive", 10) == 0) || (ptr_len_casecmp(val, val_len, "close", 5) == 0)) {
        return;
      }
    }
    {
      HdrCsvIter iter;
      val = iter.get_first(con_hdr, &val_len);

      while (val) {

        const char *wks = hdrtoken_string_to_wks(val, val_len);
        if (wks) {
          if ((wks != HTTP_VALUE_KEEP_ALIVE) && (wks != HTTP_VALUE_CLOSE) &&
              (wks != MIME_FIELD_HOST) && (wks != MIME_FIELD_DATE)) {
            int wks_length = hdrtoken_wks_to_length(wks);
            header->field_delete(wks, wks_length);
          }
        } else {
          // the following are needed to delete non-standard HTTP headers.
          MIMEHdrImpl *m_hdr_impl = header->m_http->m_fields_impl;
          MIMEField *hdr_to_remove;

          // The specified header is not a standard HTTP header.
          hdr_to_remove = _mime_hdr_field_list_search_by_string(m_hdr_impl, val, val_len);
          if (hdr_to_remove)
            header->field_delete(hdr_to_remove);
        }

        val = iter.get_next(&val_len);
      }
    }
  }
}


void
HttpTransactHeaders::remove_conditional_headers(HTTPHdr *base, HTTPHdr *outgoing)
{
  NOWARN_UNUSED(base);
  if (outgoing->presence(MIME_PRESENCE_IF_MODIFIED_SINCE |
                         MIME_PRESENCE_IF_UNMODIFIED_SINCE | MIME_PRESENCE_IF_MATCH | MIME_PRESENCE_IF_NONE_MATCH)) {

    outgoing->field_delete(MIME_FIELD_IF_MODIFIED_SINCE, MIME_LEN_IF_MODIFIED_SINCE);
    outgoing->field_delete(MIME_FIELD_IF_UNMODIFIED_SINCE, MIME_LEN_IF_UNMODIFIED_SINCE);
    outgoing->field_delete(MIME_FIELD_IF_MATCH, MIME_LEN_IF_MATCH);
    outgoing->field_delete(MIME_FIELD_IF_NONE_MATCH, MIME_LEN_IF_NONE_MATCH);
  }
  // TODO: how about RANGE and IF_RANGE?
}


////////////////////////////////////////////////////////////////////////
// Deal with lame-o servers by removing the host name from the url.
void
HttpTransactHeaders::remove_host_name_from_url(HTTPHdr *outgoing_request)
{
  URL *outgoing_url = outgoing_request->url_get();
  outgoing_url->nuke_proxy_stuff();
}


void
HttpTransactHeaders::add_global_user_agent_header_to_request(HttpConfigParams *http_config_param, HTTPHdr *header)
{
  if (http_config_param->global_user_agent_header) {
    MIMEField *ua_field;

    Debug("http_trans", "Adding User-Agent: %s", http_config_param->global_user_agent_header);
    if ((ua_field = header->field_find(MIME_FIELD_USER_AGENT, MIME_LEN_USER_AGENT)) == NULL) {
      if (likely((ua_field = header->field_create(MIME_FIELD_USER_AGENT, MIME_LEN_USER_AGENT)) != NULL))
        header->field_attach(ua_field);
    }
    // This will remove any old string (free it), and set our User-Agent.
    if (likely(ua_field))
      header->field_value_set(ua_field, http_config_param->global_user_agent_header,
                              http_config_param->global_user_agent_header_size);
  }
}


void
HttpTransactHeaders::add_server_header_to_response(OverridableHttpConfigParams *http_txn_conf, HTTPHdr *header)
{
  if (http_txn_conf->proxy_response_server_enabled && http_txn_conf->proxy_response_server_string) {
    MIMEField *ua_field;
    bool do_add = true;

    if ((ua_field = header->field_find(MIME_FIELD_SERVER, MIME_LEN_SERVER)) == NULL) {
      if (likely((ua_field = header->field_create(MIME_FIELD_SERVER, MIME_LEN_SERVER)) != NULL))
        header->field_attach(ua_field);
    } else {
      // There was an existing header from Origin, so only add if setting allows to overwrite.
      do_add = (1 == http_txn_conf->proxy_response_server_enabled);
    }

    // This will remove any old string (free it), and set our Server header.
    if (do_add && likely(ua_field)) {
      Debug("http_trans", "Adding Server: %s", http_txn_conf->proxy_response_server_string);
      header->field_value_set(ua_field, http_txn_conf->proxy_response_server_string, http_txn_conf->proxy_response_server_string_len);
    }
  }
}


void
HttpTransactHeaders::remove_privacy_headers_from_request(HttpConfigParams *http_config_param,
                                                         OverridableHttpConfigParams *http_txn_conf, HTTPHdr *header)
{
  if (!header)
    return;

  // From
  if (http_txn_conf->anonymize_remove_from) {
    Debug("anon", "removing 'From' headers");
    header->field_delete(MIME_FIELD_FROM, MIME_LEN_FROM);
  }
  // Referer
  if (http_txn_conf->anonymize_remove_referer) {
    Debug("anon", "removing 'Referer' headers");
    header->field_delete(MIME_FIELD_REFERER, MIME_LEN_REFERER);
  }
  // User-Agent
  if (http_txn_conf->anonymize_remove_user_agent) {
    Debug("anon", "removing 'User-agent' headers");
    header->field_delete(MIME_FIELD_USER_AGENT, MIME_LEN_USER_AGENT);
  }
  // Cookie
  if (http_txn_conf->anonymize_remove_cookie) {
    Debug("anon", "removing 'Cookie' headers");
    header->field_delete(MIME_FIELD_COOKIE, MIME_LEN_COOKIE);
  }
  // Client-ip
  if (http_txn_conf->anonymize_remove_client_ip) {
    Debug("anon", "removing 'Client-ip' headers");
    header->field_delete(MIME_FIELD_CLIENT_IP, MIME_LEN_CLIENT_IP);
  }
  /////////////////////////////////////////////
  // remove any other user specified headers //
  /////////////////////////////////////////////

  // FIXME: we shouldn't parse this list every time, only when the
  // FIXME: config file changes.
  if (http_config_param->anonymize_other_header_list) {
    Str *field;
    StrList anon_list(false);
    const char *anon_string;

    anon_string = http_config_param->anonymize_other_header_list;
    Debug("anon", "removing other headers (%s)", anon_string);
    HttpCompat::parse_comma_list(&anon_list, anon_string);
    for (field = anon_list.head; field != NULL; field = field->next) {
      Debug("anon", "removing '%s' headers", field->str);
      header->field_delete(field->str, field->len);
    }
  }
}
