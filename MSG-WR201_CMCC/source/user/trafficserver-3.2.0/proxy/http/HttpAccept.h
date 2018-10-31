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

#if !defined (_HttpAccept_h_)
#define _HttpAccept_h_

#include "libts.h"
#include "P_EventSystem.h"
#include "HttpConfig.h"
#include "HTTP.h"

namespace detail {
  /** Options for @c HttpAccept.

      @internal This is done as a separate class for two reasons.

      The first is that in current usage many instances are created
      with the same options so (for the client) this is easier and
      more efficient than passing options directly to the @c
      HttpAccept or calling setters.

      The second is that @c HttpAccept is not provided with any thread
      safety because it is intended as an immutable object. Putting
      the setters here and not there makes that clearer.

      We don't do this directly as nested class because we want to
      inherit the data members rather than duplicate the declarations
      and initializations.
   */
  class HttpAcceptOptions {
  private:
    typedef HttpAcceptOptions self; ///< Self reference type.
  public:
    HttpAcceptOptions();

    // Connection type (HttpProxyPort::TransportType)
    int transport_type;
    /// Set the transport type.
    self& setTransportType(int);
    /// Local address to bind for outbound connections.
    IpAddr outbound_ip4;
    /// Local address to bind for outbound connections.
    IpAddr outbound_ip6;
    /// Set the outbound IP address.
    self& setOutboundIp(IpAddr& ip);
    self& setOutboundIp(IpEndpoint* ip);
    /// Local port for outbound connection.
    uint16_t outbound_port;
    /// Set outbound port.
    self& setOutboundPort(uint16_t);
    /// Outbound transparent.
    bool f_outbound_transparent;
    /// Set outbound transparency.
    self& setOutboundTransparent(bool);
    /// Accepting backdoor connections.
    bool backdoor;
    /// Set backdoor accept.
    self& setBackdoor(bool);
  };

  inline HttpAcceptOptions::HttpAcceptOptions()
    : transport_type(0)
    , outbound_port(0)
    , f_outbound_transparent(false)
    , backdoor(false)
  {
  }

  inline HttpAcceptOptions&
  HttpAcceptOptions::setTransportType(int type) {
    transport_type =  type;
    return *this;
  }

  inline HttpAcceptOptions&
  HttpAcceptOptions::setOutboundIp(IpAddr& ip) {
    if (ip.isIp4()) outbound_ip4 = ip;
    else if (ip.isIp6()) outbound_ip6 = ip;
    return *this;
  }

  inline HttpAcceptOptions&
  HttpAcceptOptions::setOutboundIp(IpEndpoint* ip) {
    if (ip->isIp4()) outbound_ip4 = *ip;
    else if (ip->isIp6()) outbound_ip6 = *ip;
    return *this;
  }

  inline HttpAcceptOptions&
  HttpAcceptOptions::setOutboundPort(uint16_t port) {
    outbound_port = port;
    return *this;
  }

  inline HttpAcceptOptions&
  HttpAcceptOptions::setOutboundTransparent(bool flag) {
    f_outbound_transparent = flag;
    return *this;
  }

  inline HttpAcceptOptions&
  HttpAcceptOptions::setBackdoor(bool flag) {
    backdoor = flag;
    return *this;
  }
}

/**
   The continuation mutex is NULL to allow parellel accepts in NT. No
   state is recorded by the handler and values are required to be set
   during construction via the @c Options struct and never changed. So
   a NULL mutex is safe.

   Most of the state is simply passed on to the @c ClientSession after
   an accept. It is done here because this is the least bad pathway
   from the top level configuration to the HTTP session.
*/

class HttpAccept: public Continuation, private detail::HttpAcceptOptions
{
private:
  typedef HttpAccept self; ///< Self reference type.
public:
  /** Construction options.
      Provide an easier to remember typedef for clients.
  */
  typedef detail::HttpAcceptOptions Options;

  HttpAccept(Options const& opt = DEFAULT_OPTIONS)
    : Continuation(new_ProxyMutex())
    , detail::HttpAcceptOptions(opt) // copy these.
  {
    SET_HANDLER(&HttpAccept::mainEvent);
    return;
  }

  ~HttpAccept()
  {
    return;
  }

  int mainEvent(int event, void *netvc);

  /// Container for default options.
  static Options const DEFAULT_OPTIONS;

private:
    HttpAccept(const HttpAccept &);
    HttpAccept & operator =(const HttpAccept &);
};

#endif
