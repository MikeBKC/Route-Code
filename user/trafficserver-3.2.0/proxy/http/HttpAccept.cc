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

#include "HttpAccept.h"
#include "IPAllow.h"
#include "HttpClientSession.h"
#include "I_Machine.h"

HttpAccept::Options const HttpAccept::DEFAULT_OPTIONS;

int
HttpAccept::mainEvent(int event, void *data)
{
  ink_release_assert(event == NET_EVENT_ACCEPT || event == EVENT_ERROR);
  ink_release_assert((event == NET_EVENT_ACCEPT) ? (data != 0) : (1));

  if (event == NET_EVENT_ACCEPT) {
    ////////////////////////////////////////////////////
    // if client address forbidden, close immediately //
    ////////////////////////////////////////////////////
    NetVConnection *netvc = static_cast<NetVConnection *>(data);
    sockaddr const* client_ip = netvc->get_remote_addr();
    uint32_t acl_method_mask = 0;
    ip_port_text_buffer ipb;

    // The backdoor port is now only bound to "localhost", so reason to
    // check for if it's incoming from "localhost" or not.
    if (backdoor) {
      acl_method_mask = IpAllow::AllMethodMask();
    } else if (IpAllow::instance() && ((acl_method_mask = IpAllow::instance()->match(client_ip)) == 0)) {
      Warning("connect by disallowed client %s, closing", ats_ip_ntop(client_ip, ipb, sizeof(ipb)));
      netvc->do_io_close();

      return VC_EVENT_CONT;
    }

    netvc->attributes = transport_type;

    if (is_debug_tag_set("http_seq"))
      Debug("http_seq", "[HttpAccept:mainEvent %p] accepted connection from %s transport type = %d", netvc, ats_ip_nptop(client_ip, ipb, sizeof(ipb)), netvc->attributes);

    HttpClientSession *new_session = THREAD_ALLOC_INIT(httpClientSessionAllocator, netvc->thread);

   // copy over session related data.
    new_session->f_outbound_transparent = f_outbound_transparent;
    new_session->outbound_ip4 = outbound_ip4;
    new_session->outbound_ip6 = outbound_ip6;
    new_session->outbound_port = outbound_port;
    new_session->acl_method_mask = acl_method_mask;

    new_session->new_connection(netvc, backdoor);

    return EVENT_CONT;
  }

  /////////////////
  // EVENT_ERROR //
  /////////////////
  if (((long) data) == -ECONNABORTED) {
    /////////////////////////////////////////////////
    // Under Solaris, when accept() fails and sets //
    // errno to EPROTO, it means the client has    //
    // sent a TCP reset before the connection has  //
    // been accepted by the server...  Note that   //
    // in 2.5.1 with the Internet Server Supplement//
    // and also in 2.6 the errno for this case has //
    // changed from EPROTO to ECONNABORTED.        //
    /////////////////////////////////////////////////

    // FIX: add time to user_agent_hangup
    HTTP_SUM_DYN_STAT(http_ua_msecs_counts_errors_pre_accept_hangups_stat, 0);
  }

  MachineFatal("HTTP accept received fatal error: errno = %d", -((int)(intptr_t)data));
  return EVENT_CONT;
}
