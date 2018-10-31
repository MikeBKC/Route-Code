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

/**************************************************************************

  Connection.h
  Description:
  struct Connection
  struct Server
  struct ConnectionManager

  struct ConnectionManager
  ========================

  struct ConnectionManager provides the interface for network or disk
  connections.  There is a global ConnectionManager in the system
  (connectionManager).

  Connection * connect()
  Connection * accept()

  The accept call is a blocking call while connect is non-blocking. They
  returns a new Connection instance which is an handle to the newly created
  connection. The connection `q instance can be used later for read/writes
  using an intance of IOProcessor class.


  **************************************************************************/

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "libts.h"

struct NetVCOptions;

//
// Defines
//

#define NON_BLOCKING_CONNECT     true
#define BLOCKING_CONNECT         false
#define CONNECT_WITH_TCP         true
#define CONNECT_WITH_UDP         false
#define NON_BLOCKING             true
#define BLOCKING                 false
#define BIND_RANDOM_PORT         true
#define BIND_ANY_PORT            false
#define ENABLE_MC_LOOPBACK       true
#define DISABLE_MC_LOOPBACK      false
#define BC_NO_CONNECT      	 true
#define BC_CONNECT      	 false
#define BC_NO_BIND      	 true
#define BC_BIND      	 	 false

///////////////////////////////////////////////////////////////////////
//
// Connection
//
///////////////////////////////////////////////////////////////////////
struct Connection
{
  SOCKET fd; ///< Socket for connection.
  IpEndpoint addr; ///< Associated address.
  bool is_bound; ///< Flag for already bound to a local address.
  bool is_connected; ///< Flag for already connected.
  int sock_type;

  /** Create and initialize the socket for this connection.

      A socket is created and the options specified by @a opt are
      set. The socket is @b not connected.

      @note It is important to pass the same @a opt to this method and
      @c connect.

      @return 0 on success, -ERRNO on failure.
      @see connect
  */
  int open(
	   NetVCOptions const& opt = DEFAULT_OPTIONS ///< Socket options.
	   );

  /** Connect the socket.

      The socket is connected to the remote @a addr and @a port. The
      @a opt structure is used to control blocking on the socket. All
      other options are set via @c open. It is important to pass the
      same @a opt to this method as was passed to @c open.

      @return 0 on success, -ERRNO on failure.
      @see open
  */
  int connect(
           sockaddr const* to, ///< Remote address and port.
	   NetVCOptions const& opt = DEFAULT_OPTIONS ///< Socket options
	   );


  /// Set the internal socket address struct.
  /// @internal Used only by ICP.
  void setRemote(
    sockaddr const* remote_addr ///< Address and port.
  ) {
    ats_ip_copy(&addr, remote_addr);
  }

  /**
    @deprecated preserve backward compatibility with non-IPv6 iocore
  */
  void setRemote(
    in_addr_t ip,
    int port
  ) {
	ats_ip4_set(&addr.sin, ip, htons(port));
  }

    
  int setup_mc_send(sockaddr const* mc_addr,
                    sockaddr const* my_addr,
                    bool non_blocking = NON_BLOCKING,
                    unsigned char mc_ttl = 1, bool mc_loopback = DISABLE_MC_LOOPBACK, Continuation * c = NULL);

  /**
    @deprecated preserve backward compatibility with non-IPv6 iocore
  */

  int setup_mc_send(unsigned int mc_ip, int mc_port,
                    unsigned int my_ip, int my_port,
                    bool non_blocking = NON_BLOCKING,
                    unsigned char mc_ttl = 1, bool mc_loopback = DISABLE_MC_LOOPBACK, Continuation * c = NULL)
  {
    struct sockaddr_in mc_addr;
    struct sockaddr_in my_addr;

    ats_ip4_set(&mc_addr, mc_ip, htons(mc_port));
    ats_ip4_set(&my_addr, my_ip, htons(my_port));

    return setup_mc_send(
        ats_ip_sa_cast(&mc_addr), 
        ats_ip_sa_cast(&my_addr), 
        non_blocking, mc_ttl, mc_loopback, c);
  }                 


  int setup_mc_receive(sockaddr const* from,
                       bool non_blocking = NON_BLOCKING, Connection * sendchan = NULL, Continuation * c = NULL);

  /**
   @deprecated preserve backward compatibility with non-IPv6 iocore
  */

  int setup_mc_receive(unsigned int mc_ip, int port,
                       bool non_blocking = NON_BLOCKING, Connection * sendchan = NULL, Continuation * c = NULL)
  {
    struct sockaddr_in mc_addr;
    ats_ip4_set(&mc_addr, mc_ip, port);

    return setup_mc_receive(ats_ip_sa_cast(&mc_addr), non_blocking, sendchan, c);
  }


  int close();                  // 0 on success, -errno on failure

  void apply_options(NetVCOptions const& opt);

  virtual ~ Connection();
  Connection();

  /// Default options.
  static NetVCOptions const DEFAULT_OPTIONS;

protected:
  void _cleanup();
};

///////////////////////////////////////////////////////////////////////
//
// Server
//
///////////////////////////////////////////////////////////////////////
struct Server: public Connection
{
  /// Client side (inbound) local IP address.
  IpEndpoint accept_addr;

  /// If set, the related incoming connect was transparent.
  bool f_inbound_transparent;

  /// If set, a kernel HTTP accept filter
  bool http_accept_filter;

  //
  // Use this call for the main proxy accept
  //
  int proxy_listen(bool non_blocking = false);

  int accept(Connection * c);

  //
  // Listen on a socket. We assume the port is in host by order, but
  // that the IP address (specified by accept_addr) has already been
  // converted into network byte order
  //

  int listen(bool non_blocking = false, int recv_bufsize = 0, int send_bufsize = 0, bool transparent = false);
  int setup_fd_for_listen(
    bool non_blocking = false,
    int recv_bufsize = 0,
    int send_bufsize = 0,
    bool transparent = false ///< Inbound transparent.
  );

  Server()
    : Connection()
    , f_inbound_transparent(false)
  {
    ink_zero(accept_addr);
  }
};

#endif /*_Connection_h*/
