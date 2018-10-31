/** @file

  HTTP configuration support.

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

# include <records/I_RecCore.h>
# include <records/I_RecHttp.h>
# include <ts/ink_defs.h>
# include <ts/Tokenizer.h>
# include <strings.h>

void RecHttpLoadIp(char const* value_name, IpAddr& ip4, IpAddr& ip6)
{
  char value[1024];
  ip4.invalidate();
  ip6.invalidate();
  if (REC_ERR_OKAY == RecGetRecordString(value_name, value, sizeof(value))) {
    Tokenizer tokens(", ");
    int n_addrs = tokens.Initialize(value);
    for (int i = 0 ; i < n_addrs ; ++i ) {
      char const* host = tokens[i];
      IpEndpoint tmp4, tmp6;
      // For backwards compatibility we need to support the use of host names
      // for the address to bind.
      if (0 == ats_ip_getbestaddrinfo(host, &tmp4, &tmp6)) {
        if (ats_is_ip4(&tmp4)) {
          if (!ip4.isValid()) ip4 = tmp4;
          else Warning("'%s' specifies more than one IPv4 address, ignoring %s.", value_name, host);
        }
        if (ats_is_ip6(&tmp6)) {
          if (!ip6.isValid()) ip6 = tmp6;
          else Warning("'%s' specifies more than one IPv6 address, ignoring %s.", value_name, host);
        }
      } else {
        Warning("'%s' has an value '%s' that is not recognized as an IP address, ignored.", value_name, host);
      }
    }
  }
}


char const* const HttpProxyPort::DEFAULT_VALUE = "8080";

char const* const HttpProxyPort::PORT_CONFIG_NAME = "proxy.config.http.server_port";
char const* const HttpProxyPort::ATTR_CONFIG_NAME = "proxy.config.http.server_port_attr";
char const* const HttpProxyPort::OTHER_PORTS_CONFIG_NAME = "proxy.config.http.server_other_ports";
char const* const HttpProxyPort::PORTS_CONFIG_NAME = "proxy.config.http.server_ports";
char const* const HttpProxyPort::SSL_ENABLED_CONFIG_NAME = "proxy.config.ssl.enabled";
char const* const HttpProxyPort::SSL_PORT_CONFIG_NAME = "proxy.config.ssl.server_port";

char const* const HttpProxyPort::OPT_FD_PREFIX = "fd";
char const* const HttpProxyPort::OPT_OUTBOUND_IP_PREFIX = "ip-out";
char const* const HttpProxyPort::OPT_INBOUND_IP_PREFIX = "ip-in";
char const* const HttpProxyPort::OPT_IPV6 = "ipv6";
char const* const HttpProxyPort::OPT_IPV4 = "ipv4";
char const* const HttpProxyPort::OPT_TRANSPARENT_INBOUND = "tr-in";
char const* const HttpProxyPort::OPT_TRANSPARENT_OUTBOUND = "tr-out";
char const* const HttpProxyPort::OPT_TRANSPARENT_FULL = "tr-full";
char const* const HttpProxyPort::OPT_SSL = "ssl";
char const* const HttpProxyPort::OPT_BLIND_TUNNEL = "blind";
char const* const HttpProxyPort::OPT_COMPRESSED = "compressed";

// File local constants.
namespace {
size_t const OPT_FD_PREFIX_LEN = strlen(HttpProxyPort::OPT_FD_PREFIX);
size_t const OPT_OUTBOUND_IP_PREFIX_LEN = strlen(HttpProxyPort::OPT_OUTBOUND_IP_PREFIX);
size_t const OPT_INBOUND_IP_PREFIX_LEN = strlen(HttpProxyPort::OPT_INBOUND_IP_PREFIX);
}

namespace {
// Solaris work around. On that OS the compiler will not let me use an
// instantiated instance of Vec<self> inside the class, even if
// static. So we have to declare it elsewhere and then import via
// reference. Might be a problem with Vec<> creating a fixed array
// rather than allocating on first use (compared to std::vector<>).
  HttpProxyPort::Group GLOBAL_DATA;
}
HttpProxyPort::Group& HttpProxyPort::m_global = GLOBAL_DATA;

HttpProxyPort::HttpProxyPort()
  : m_fd(ts::NO_FD)
  , m_type(TRANSPORT_DEFAULT)
  , m_port(0)
  , m_family(AF_INET)
  , m_inbound_transparent_p(false)
  , m_outbound_transparent_p(false)
{
}

bool HttpProxyPort::hasSSL(Group const& ports) {
  bool zret = false;
  for ( int i = 0 , n = ports.length() ; i < n && !zret ; ++i ) {
    if (ports[i].isSSL()) zret = true;
  }
  return zret;
}

HttpProxyPort* HttpProxyPort::findHttp(Group const& ports, uint16_t family) {
  bool check_family_p = ats_is_ip(family);
  self* zret = 0;
  for ( int i = 0 , n = ports.length() ; i < n && !zret ; ++i ) {
    HttpProxyPort& p = ports[i];
    if (p.m_port && // has a valid port
	TRANSPORT_DEFAULT == p.m_type && // is normal HTTP
	( !check_family_p || p.m_family == family) // right address family
	)
      zret = &p;;
  }
  return zret;
}

bool
HttpProxyPort::loadConfig(Vec<self>& entries) {
  char* text;
  char* attr;
  bool found_p;

  // Do current style port configuration first.
  text = REC_readString(PORTS_CONFIG_NAME, &found_p);
  if (found_p) self::loadValue(entries, text);
  ats_free(text);

  // Check old style single port.
  // Yeah, it's ugly, but it should be purged on the next major release.
  int sp = REC_ConfigReadInteger(PORT_CONFIG_NAME);
  attr = REC_ConfigReadString(ATTR_CONFIG_NAME);
  if (-1 == sp) {
    // Default value, ignore.
  } else if (0 < sp && sp < 65536) {
    if (attr) {
      char* buff = static_cast<char*>(alloca(6 + 1 + strlen(attr) + 1));
      sprintf(buff, "%d:%s", sp, attr);
      self::loadValue(entries, buff);
    } else {
      HttpProxyPort pd;
      pd.m_port = sp;
      entries.push_back(pd);
    }
  } else {
    Warning("Invalid port value %d is not in the range 1..65535 for '%s'.", sp, PORT_CONFIG_NAME);
  }
  ats_free(attr);

  // Do the "other" ports.
  text = REC_ConfigReadString(OTHER_PORTS_CONFIG_NAME);
  self::loadValue(entries, text);
  ats_free(text);

  // Check for old style SSL configuration. We only put this in iff
  // SSL is explicitly enabled and has the server port set.
  RecInt ssl_enabled;
  RecInt ssl_port;
  if (REC_ERR_OKAY == RecGetRecordInt(SSL_ENABLED_CONFIG_NAME, &ssl_enabled) && ssl_enabled) {
    if (REC_ERR_OKAY == RecGetRecordInt(SSL_PORT_CONFIG_NAME, &ssl_port)) {
      if (0 < ssl_port && ssl_port < 65536) {
        HttpProxyPort port;
        port.m_port = ssl_port;
        port.m_type = TRANSPORT_SSL;
        entries.push_back(port);
      } else if (-1 != ssl_port) {
	// -1 is used as the sentinel value in the config record for
	// "not set by user".
        Warning("SSL enabled but port value %"PRId64" is not in the range 1..65535.\n",
                ssl_port);
      }
    }
  }

  return 0 < entries.length();
}

bool
HttpProxyPort::loadDefaultIfEmpty(Group& ports) {
  RecInt ssl_enabled;

  if (0 == ports.length())
    self::loadValue(ports, DEFAULT_VALUE);

  // Check to see if we need to force an SSL port.
  if (REC_ERR_OKAY == RecGetRecordInt(SSL_ENABLED_CONFIG_NAME, &ssl_enabled) && ssl_enabled) {
    // SSL is explicitly enabled, so force an SSL port if there isn't one.
    if (!self::hasSSL(ports)) {
      HttpProxyPort port;
      RecInt ssl_port = -1;
      RecGetRecordInt(SSL_PORT_CONFIG_NAME, &ssl_port);
      if (ssl_port < 1 || 65535 < ssl_port) ssl_port = 443;
      port.m_port = ssl_port;
      port.m_type = TRANSPORT_SSL;
      ports.push_back(port);
    }
  }
  return 0 < ports.length();
}

bool
HttpProxyPort::loadValue(Vec<self>& ports, char const* text) {
  int n_elts = ports.length(); // remember this.
  if (text && *text) {
    Tokenizer tokens(", ");
    int n_ports = tokens.Initialize(text);
    if (n_ports > 0) {
      for ( int p = 0 ; p < n_ports ; ++p ) {
        char const* elt = tokens[p];
        HttpProxyPort entry;
        if (entry.processOptions(elt)) ports.push_back(entry);
        else Warning("No port was found in port configuration element '%s'", elt);
      }
    }
  }
  return ports.length() > n_elts; // we added at least one port.
}

bool
HttpProxyPort::processOptions(char const* opts) {
  bool zret = false; // no port found yet.
  bool af_set_p = false; // AF explicitly specified.
  bool bracket_p = false; // inside brackets during parse.
  Vec<char*> values; // Pointers to single option values.

  // Make a copy we can modify safely.
  size_t opts_len = strlen(opts) + 1;
  char* text = static_cast<char*>(alloca(opts_len));
  memcpy(text, opts, opts_len);

  // Split the copy in to tokens.
  char* token = 0;
  for (char* spot = text ; *spot ; ++spot ) {
    if (bracket_p) {
      if (']' == *spot) bracket_p = false;
    } else if (':' == *spot) {
      *spot = 0;
      token = 0;
    } else {
      if (! token) {
        token = spot;
        values.push_back(token);
      }
      if ('[' == *spot) bracket_p = true;
    }
  }
  if (bracket_p) {
    Warning("Invalid port descriptor '%s' - left bracket without closing right bracket", opts);
    return zret;
  }

  for ( int i = 0, n_items = values.length() ; i < n_items ; ++i) {
    char const* item = values[i];
    if (isdigit(item[0])) { // leading digit -> port value
      char* ptr;
      int port = strtoul(item, &ptr, 10);
      if (ptr == item) {
        // really, this shouldn't happen, since we checked for a leading digit.
        Warning("Mangled port value '%s' in port configuration '%s'", item, opts);
      } else if (port <= 0 || 65536 <= port) {
        Warning("Port value '%s' out of range in port configuration '%s'", item, opts);
      } else {
        m_port = port;
        zret = true;
      }
    } else if (0 == strncasecmp(OPT_FD_PREFIX, item, OPT_FD_PREFIX_LEN)) {
      char* ptr; // tmp for syntax check.
      item += OPT_FD_PREFIX_LEN; // skip prefix
      if ('-' == *item || '=' == *item) ++item; // permit optional '-' or '='
      int fd = strtoul(item, &ptr, 10);
      if (ptr == item) {
        Warning("Mangled file descriptor value '%s' in port descriptor '%s'", item, opts);
      } else {
        m_fd = fd;
        zret = true;
      }
    } else if (0 == strncasecmp(OPT_INBOUND_IP_PREFIX, item, OPT_INBOUND_IP_PREFIX_LEN)) {
      IpEndpoint ip;
      item += OPT_INBOUND_IP_PREFIX_LEN; // skip prefix
      if ('-' == *item || '=' == *item) ++item; // permit optional '-' or '='
      if (0 == ats_ip_pton(item, &ip))
        m_inbound_ip = ip;
      else
        Warning("Invalid IP address value '%s' in port descriptor '%s'",
          item, opts
        );
    } else if (0 == strncasecmp(OPT_OUTBOUND_IP_PREFIX, item, OPT_OUTBOUND_IP_PREFIX_LEN)) {
      IpAddr ip;
      item += OPT_OUTBOUND_IP_PREFIX_LEN; // skip prefix
      if ('-' == *item || '=' == *item) ++item; // permit optional '-' or '='
      if (0 == ip.load(item))
        this->outboundIp(ip.family()) = ip;
      else
        Warning("Invalid IP address value '%s' in port descriptor '%s'",
          item, opts
        );
    } else if (0 == strcasecmp("X", item)) {
      // defaults
    } else if (0 == strcasecmp("C", item) || 0 == strcasecmp(OPT_COMPRESSED, item)) {
      m_type = TRANSPORT_COMPRESSED;
    } else if (0 == strcasecmp("T", item) || 0 == strcasecmp(OPT_BLIND_TUNNEL, item)) {
      m_type = TRANSPORT_BLIND_TUNNEL;
    } else if (0 == strcasecmp("X6", item) || 0 == strcasecmp(OPT_IPV6, item)) {
      m_family = AF_INET6;
      m_type = TRANSPORT_DEFAULT;
      af_set_p = true;
    } else if (0 == strcasecmp(OPT_IPV4, item)) {
      m_family = AF_INET;
      af_set_p = true;
    } else if (0 == strcasecmp("S", item) || 0 == strcasecmp(OPT_SSL, item)) {
      m_type = TRANSPORT_SSL;
      m_inbound_transparent_p = m_outbound_transparent_p = false;
    } else if (0 == strcasecmp(OPT_TRANSPARENT_INBOUND, item) ||
      0 == strcasecmp("<", item)) {
# if TS_USE_TPROXY
      m_inbound_transparent_p = true;
# else
      Warning("Transparency requested [%s] in port descriptor '%s' but TPROXY was not configured.", item, opts);
# endif
    } else if (0 == strcasecmp(OPT_TRANSPARENT_OUTBOUND, item) ||
      0 == strcasecmp(">", item)) {
# if TS_USE_TPROXY
      m_outbound_transparent_p = true;
# else
      Warning("Transparency requested [%s] in port descriptor '%s' but TPROXY was not configured.", item, opts);
# endif
    } else if (0 == strcasecmp(OPT_TRANSPARENT_FULL, item)||
      0 == strcasecmp("=", item)) {
# if TS_USE_TPROXY
      m_inbound_transparent_p = true;
      m_outbound_transparent_p = true;
# else
      Warning("Transparency requested [%s] in port descriptor '%s' but TPROXY was not configured.", item, opts);
# endif
    } else {
      Warning("Invalid option '%s' in port configuration '%s'", item, opts);
    }
  }

  bool in_ip_set_p = m_inbound_ip.isValid();

  if (af_set_p) {
    if (in_ip_set_p && m_family != m_inbound_ip.family()) {
      Warning("Invalid port descriptor '%s' - the inbound adddress family [%s] is not the same type as the explict family value [%s]",
        opts, ats_ip_family_name(m_inbound_ip.family()), ats_ip_family_name(m_family));
      zret = false;
    }
  } else if (in_ip_set_p) {
    m_family = m_inbound_ip.family(); // set according to address.
  }

  return zret;
}

int
HttpProxyPort::print(char* out, size_t n) {
  size_t zret = 0; // # of chars printed so far.
  ip_text_buffer ipb;
  bool need_colon_p = false;

  if (m_inbound_ip.isValid()) {
    zret += snprintf(out+zret, n-zret, "%s=[%s]",
      OPT_INBOUND_IP_PREFIX,
      m_inbound_ip.toString(ipb, sizeof(ipb))
    );
    need_colon_p = true;
  }
  if (zret >= n) return n;

  if (m_outbound_ip4.isValid()) {
    if (need_colon_p) out[zret++] = ':';
    zret += snprintf(out+zret, n-zret, "%s=[%s]",
      OPT_OUTBOUND_IP_PREFIX,
      m_outbound_ip4.toString(ipb, sizeof(ipb))
    );
    need_colon_p = true;
  }
  if (zret >= n) return n;

  if (m_outbound_ip6.isValid()) {
    if (need_colon_p) out[zret++] = ':';
    zret += snprintf(out+zret, n-zret, "%s=[%s]",
      OPT_OUTBOUND_IP_PREFIX,
      m_outbound_ip6.toString(ipb, sizeof(ipb))
    );
    need_colon_p = true;
  }
  if (zret >= n) return n;

  if (0 != m_port) {
    if (need_colon_p) out[zret++] = ':';
    zret += snprintf(out+zret, n-zret, "%d", m_port);
    need_colon_p = true;
  }
  if (zret >= n) return n;

  if (ts::NO_FD != m_fd) {
    if (need_colon_p) out[zret++] = ':';
    zret += snprintf(out+zret, n-zret, "fd=%d", m_fd);
    need_colon_p = true;
  }
  if (zret >= n) return n;

  if (AF_INET6 == m_family)
    zret += snprintf(out+zret, n-zret, ":%s", OPT_IPV6);
  if (zret >= n) return n;

  if (TRANSPORT_BLIND_TUNNEL == m_type)
    zret += snprintf(out+zret, n-zret, ":%s", OPT_BLIND_TUNNEL);
  else if (TRANSPORT_SSL == m_type)
    zret += snprintf(out+zret, n-zret, ":%s", OPT_SSL);
  else if (TRANSPORT_COMPRESSED == m_type)
    zret += snprintf(out+zret, n-zret, ":%s", OPT_COMPRESSED);
  if (zret >= n) return n;

  if (m_outbound_transparent_p && m_inbound_transparent_p)
    zret += snprintf(out+zret, n-zret, ":%s", OPT_TRANSPARENT_FULL);
  else if (m_inbound_transparent_p)
    zret += snprintf(out+zret, n-zret, ":%s", OPT_TRANSPARENT_INBOUND);
  else if (m_outbound_transparent_p)
    zret += snprintf(out+zret, n-zret, ":%s", OPT_TRANSPARENT_OUTBOUND);

  return min(zret,n);
}
