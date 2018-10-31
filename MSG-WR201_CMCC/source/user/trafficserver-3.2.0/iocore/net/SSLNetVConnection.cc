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
#include "ink_config.h"
#include "P_Net.h"
#include "P_SSLNextProtocolSet.h"

#if HAVE_OPENSSL_TLS1_H
#include <openssl/tls1.h>
#endif

#define SSL_READ_ERROR_NONE	  0
#define SSL_READ_ERROR		  1
#define SSL_READ_READY		  2
#define SSL_READ_COMPLETE	  3
#define SSL_READ_WOULD_BLOCK      4
#define SSL_READ_EOS		  5
#define SSL_HANDSHAKE_WANT_READ   6
#define SSL_HANDSHAKE_WANT_WRITE  7
#define SSL_HANDSHAKE_WANT_ACCEPT 8
#define SSL_HANDSHAKE_WANT_CONNECT 9
#define SSL_WRITE_WOULD_BLOCK     10

ClassAllocator<SSLNetVConnection> sslNetVCAllocator("sslNetVCAllocator");

//
// Private
//

#if TS_USE_TLS_SNI

static int
ssl_servername_callback(SSL * ssl, int * ad, void * arg)
{
  SSL_CTX *       ctx = NULL;
  SSLCertLookup * lookup = (SSLCertLookup *) arg;
  const char *    servername = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);

  Debug("ssl", "ssl=%p ad=%d lookup=%p server=%s", ssl, *ad, lookup, servername);

  if (likely(servername)) {
    ctx = lookup->findInfoInHash((char *)servername);
  }

  if (ctx == NULL) {
    ctx = lookup->defaultContext();
  }

  if (ctx == NULL) {
    return SSL_TLSEXT_ERR_NOACK;
  }

  Debug("ssl", "found SSL context %p for requested name '%s'", ctx, servername);
  SSL_set_SSL_CTX(ssl, ctx);

  // We need to return one of the SSL_TLSEXT_ERR constants. If we return an
  // error, we can fill in *ad with an alert code to propgate to the
  // client, see SSL_AD_*.
  return SSL_TLSEXT_ERR_OK;
}

#endif /* TS_USE_TLS_SNI */

static SSL *
make_ssl_connection(SSL_CTX * ctx, SSLNetVConnection * netvc)
{
  SSL * ssl;

  if (likely(ssl = SSL_new(ctx))) {
    SSL_set_fd(ssl, netvc->get_socket());
    SSL_set_app_data(ssl, netvc);
  }

  return ssl;
}

static inline int
do_SSL_write(SSL * ssl, void *buf, int size)
{
  int r = 0;
  do {
    // need to check into SSL error handling
    // to see if this is good enough.
    r = SSL_write(ssl, (const char *) buf, size);
    if (r >= 0)
      return r;
    else
      r = -errno;
  } while (r == -EINTR || r == -ENOBUFS || r == -ENOMEM);

  return r;
}


static int
ssl_read_from_net(NetHandler * nh, UnixNetVConnection * vc, EThread * lthread, int64_t &ret)
{
  NOWARN_UNUSED(nh);
  NetState *s = &vc->read;
  SSLNetVConnection *sslvc = (SSLNetVConnection *) vc;
  MIOBufferAccessor & buf = s->vio.buffer;
  IOBufferBlock *b = buf.mbuf->_writer;
  int event = SSL_READ_ERROR_NONE;
  int64_t bytes_read;
  int64_t block_write_avail;
  int sslErr = SSL_ERROR_NONE;

  for (bytes_read = 0; (b != 0) && (sslErr == SSL_ERROR_NONE); b = b->next) {
    block_write_avail = b->write_avail();

    Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] b->write_avail()=%"PRId64, block_write_avail);

    int64_t offset = 0;
    // while can be replaced with if - need to test what works faster with openssl
    while (block_write_avail > 0) {
      int rres = SSL_read(sslvc->ssl, b->end() + offset, (int)block_write_avail);

      Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] rres=%d", rres);

      sslErr = SSL_get_error(sslvc->ssl, rres);
      switch (sslErr) {
      case SSL_ERROR_NONE:

        DebugBufferPrint("ssl_buff", b->end() + offset, rres, "SSL Read");
        ink_debug_assert(rres);

        bytes_read += rres;
        offset += rres;
        block_write_avail -= rres;
        ink_debug_assert(block_write_avail >= 0);

        continue;

      case SSL_ERROR_WANT_WRITE:
        event = SSL_WRITE_WOULD_BLOCK;
        Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_WOULD_BLOCK(write)");
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_X509_LOOKUP:
        event = SSL_READ_WOULD_BLOCK;
        Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_WOULD_BLOCK(read)");
        break;
      case SSL_ERROR_SYSCALL:
        if (rres != 0) {
          // not EOF
          event = SSL_READ_ERROR;
          ret = errno;
          Error("[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_SYSCALL, underlying IO error: %s", strerror(errno));
        } else {
          // then EOF observed, treat it as EOS
          event = SSL_READ_EOS;
          //Error("[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_SYSCALL, EOF observed violating SSL protocol");
        }
        break;
      case SSL_ERROR_ZERO_RETURN:
        event = SSL_READ_EOS;
        Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_ZERO_RETURN");
        break;
      case SSL_ERROR_SSL:
      default: {
        char err_string[4096];
        ERR_error_string(ERR_get_error(), err_string);
        event = SSL_READ_ERROR;
        ret = errno;
        Error("[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_SSL %s", err_string);
        break;
      }
      }                         // switch
      break;
    }                           // while( block_write_avail > 0 )
  }                             // for ( bytes_read = 0; (b != 0); b = b->next)

  if (bytes_read > 0) {
    Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] bytes_read=%"PRId64, bytes_read);
    buf.writer()->fill(bytes_read);
    s->vio.ndone += bytes_read;
    vc->netActivity(lthread);

    ret = bytes_read;

    if (s->vio.ntodo() <= 0) {
      event = SSL_READ_COMPLETE;
    } else {
      event = SSL_READ_READY;
    }
  } else                        // if( bytes_read > 0 )
  {
#if defined (_DEBUG)
    if (bytes_read == 0) {
      Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] bytes_read == 0");
    }
#endif
  }
  return (event);

}


//changed by YTS Team, yamsat
void
SSLNetVConnection::net_read_io(NetHandler *nh, EThread *lthread)
{
  int ret;
  int64_t r = 0;
  int64_t bytes = 0;
  NetState *s = &this->read;
  MIOBufferAccessor &buf = s->vio.buffer;

  MUTEX_TRY_LOCK_FOR(lock, s->vio.mutex, lthread, s->vio._cont);
  if (!lock) {
    readReschedule(nh);
    return;
  }
  // If it is not enabled, lower its priority.  This allows
  // a fast connection to speed match a slower connection by
  // shifting down in priority even if it could read.
  if (!s->enabled || s->vio.op != VIO::READ) {
    read_disable(nh, this);
    return;
  }

  ink_debug_assert(buf.writer());

  // This function will always return true unless
  // vc is an SSLNetVConnection.
  if (!getSSLHandShakeComplete()) {
    int err;

    if (getSSLClientConnection()) {
      ret = sslStartHandShake(SSL_EVENT_CLIENT, err);
    } else {
      ret = sslStartHandShake(SSL_EVENT_SERVER, err);
    }

    if (ret == EVENT_ERROR) {
      this->read.triggered = 0;
      readSignalError(nh, err);
    } else if (ret == SSL_HANDSHAKE_WANT_READ || ret == SSL_HANDSHAKE_WANT_ACCEPT) {
      read.triggered = 0;
      nh->read_ready_list.remove(this);
      readReschedule(nh);
    } else if (ret == SSL_HANDSHAKE_WANT_CONNECT || ret == SSL_HANDSHAKE_WANT_WRITE) {
      write.triggered = 0;
      nh->write_ready_list.remove(this);
      writeReschedule(nh);
    } else if (ret == EVENT_DONE) {
      read.triggered = 1;
      if (read.enabled)
        nh->read_ready_list.in_or_enqueue(this);
    } else
      readReschedule(nh);
    return;
  }

  // If there is nothing to do, disable connection
  int64_t ntodo = s->vio.ntodo();
  if (ntodo <= 0) {
    read_disable(nh, this);
    // Don't return early even if there's nothing. We still need
    // to propagate events for zero-length reads.
    readSignalDone(VC_EVENT_READ_COMPLETE, nh);
    return;
  }

  do {
    if (!buf.writer()->write_avail()) {
      buf.writer()->add_block();
    }
    ret = ssl_read_from_net(nh, this, lthread, r);
    if (ret == SSL_READ_READY || ret == SSL_READ_ERROR_NONE) {
      bytes += r;
    }

  } while (ret == SSL_READ_READY || ret == SSL_READ_ERROR_NONE);

  if (bytes > 0) {
    if (ret == SSL_READ_WOULD_BLOCK) {
      if (readSignalAndUpdate(VC_EVENT_READ_READY) != EVENT_CONT) {
        Debug("ssl", "ssl_read_from_net, readSignal !=EVENT_CONT");
        return;
      }
    }
  }

  switch (ret) {
  case SSL_READ_ERROR_NONE:
  case SSL_READ_READY:
    // how did we exit the while loop above? should never happen.
    ink_debug_assert(false);
    break;
  case SSL_WRITE_WOULD_BLOCK:
  case SSL_READ_WOULD_BLOCK:
    if (lock.m.m_ptr != s->vio.mutex.m_ptr) {
      Debug("ssl", "ssl_read_from_net, mutex switched");
      if (ret == SSL_READ_WOULD_BLOCK)
        readReschedule(nh);
      else
        writeReschedule(nh);
      return;
    }
    // reset the tigger and remove from the ready queue
    // we will need to be retriggered to read from this socket again
    read.triggered = 0;
    nh->read_ready_list.remove(this);
    Debug("ssl", "read_from_net, read finished - would block");
#ifdef TS_USE_PORT
    if (ret == SSL_READ_WOULD_BLOCK)
      readReschedule(nh);
    else
      writeReschedule(nh);
#endif
    break;

  case SSL_READ_EOS:
    // close the connection if we have SSL_READ_EOS, this is the return value from ssl_read_from_net() if we get an SSL_ERROR_ZERO_RETURN from SSL_get_error()
    // SSL_ERROR_ZERO_RETURN means that the origin server closed the SSL connection
    read.triggered = 0;
    readSignalDone(VC_EVENT_EOS, nh);

    if (bytes > 0) {
      Debug("ssl", "read_from_net, read finished - EOS");
    } else {
      Debug("ssl", "read_from_net, read finished - 0 useful bytes read, bytes used by SSL layer");
    }
    break;
  case SSL_READ_COMPLETE:
    readSignalDone(VC_EVENT_READ_COMPLETE, nh);
    Debug("ssl", "read_from_net, read finished - signal done");
    break;
  case SSL_READ_ERROR:
    this->read.triggered = 0;
    readSignalError(nh, (int)r);
    Debug("ssl", "read_from_net, read finished - read error");
    break;
  }

}


int64_t
SSLNetVConnection::load_buffer_and_write(int64_t towrite, int64_t &wattempted, int64_t &total_wrote, MIOBufferAccessor & buf)
{
  ProxyMutex *mutex = this_ethread()->mutex;
  int64_t r = 0;
  int64_t l = 0;
  int64_t offset = buf.entry->start_offset;
  IOBufferBlock *b = buf.entry->block;

  do {
    // check if we have done this block
    l = b->read_avail();
    l -= offset;
    if (l <= 0) {
      offset = -l;
      b = b->next;
      continue;
    }
    // check if to amount to write exceeds that in this buffer
    int64_t wavail = towrite - total_wrote;

    if (l > wavail)
      l = wavail;
    if (!l)
      break;
    wattempted = l;
    total_wrote += l;
    Debug("ssl", "SSLNetVConnection::loadBufferAndCallWrite, before do_SSL_write, l=%"PRId64", towrite=%"PRId64", b=%p",
          l, towrite, b);
    r = do_SSL_write(ssl, b->start() + offset, (int)l);
    if (r == l) {
      wattempted = total_wrote;
    }
    // on to the next block
    offset = 0;
    b = b->next;
    Debug("ssl", "SSLNetVConnection::loadBufferAndCallWrite,Number of bytes written=%"PRId64" , total=%"PRId64"", r, total_wrote);
    NET_DEBUG_COUNT_DYN_STAT(net_calls_to_write_stat, 1);
  } while (r == l && total_wrote < towrite && b);
  if (r > 0) {
    if (total_wrote != wattempted) {
      Debug("ssl", "SSLNetVConnection::loadBufferAndCallWrite, wrote some bytes, but not all requested.");
      return (r);
    } else {
      Debug("ssl", "SSLNetVConnection::loadBufferAndCallWrite, write successful.");
      return (total_wrote);
    }
  } else {
    int err = SSL_get_error(ssl, (int)r);

    switch (err) {
    case SSL_ERROR_NONE:
      Debug("ssl", "SSL_write-SSL_ERROR_NONE");
      break;
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_X509_LOOKUP:
      r = -EAGAIN;
      Debug("ssl", "SSL_write-SSL_ERROR_WANT_WRITE");
      break;
    case SSL_ERROR_SYSCALL:
      r = -errno;
      Debug("ssl", "SSL_write-SSL_ERROR_SYSCALL");
      break;
      // end of stream
    case SSL_ERROR_ZERO_RETURN:
      r = -errno;
      Debug("ssl", "SSL_write-SSL_ERROR_ZERO_RETURN");
      break;
    case SSL_ERROR_SSL:
    default:
      r = -errno;
      Debug("ssl", "SSL_write-SSL_ERROR_SSL");
      SSLNetProcessor::logSSLError("SSL_write");
      break;
    }
    return (r);
  }
}

SSLNetVConnection::SSLNetVConnection():
  sslHandShakeComplete(false),
  sslClientConnection(false),
  npnSet(NULL),
  npnEndpoint(NULL)
{
  ssl = NULL;
}

void
SSLNetVConnection::free(EThread * t) {
  NET_SUM_GLOBAL_DYN_STAT(net_connections_currently_open_stat, -1);
  got_remote_addr = 0;
  got_local_addr = 0;
  read.vio.mutex.clear();
  write.vio.mutex.clear();
  this->mutex.clear();
  flags = 0;
  SET_CONTINUATION_HANDLER(this, (SSLNetVConnHandler) & SSLNetVConnection::startEvent);
  nh = NULL;
  read.triggered = 0;
  write.triggered = 0;
  options.reset();
  closed = 0;
  ink_assert(con.fd == NO_FD);
  if (ssl != NULL) {
    /*if (sslHandShakeComplete)
       SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN|SSL_RECEIVED_SHUTDOWN); */
    SSL_free(ssl);
    ssl = NULL;
  }
  sslHandShakeComplete = false;
  sslClientConnection = false;
  npnSet = NULL;

  if (from_accept_thread) {
    sslNetVCAllocator.free(this);  
  } else {
    THREAD_FREE(this, sslNetVCAllocator, t);
  }
}

int
SSLNetVConnection::sslStartHandShake(int event, int &err)
{
  IpEndpoint ip;

  if (event == SSL_EVENT_SERVER) {
    if (this->ssl == NULL) {
      SSL_CTX * ctx;
      int namelen = sizeof(ip);
      char buff[INET6_ADDRSTRLEN];

      safe_getsockname(get_socket(), &ip.sa, &namelen);
      ats_ip_ntop(&ip.sa, buff, sizeof(buff));
      ctx = sslCertLookup.findInfoInHash(buff);
      if (ctx == NULL) {
        ctx = sslCertLookup.defaultContext();
      }

#if TS_USE_TLS_SNI
      Debug("ssl", "setting SNI callbacks with initial ctx %p", ctx);
      SSL_CTX_set_tlsext_servername_callback(ctx, ssl_servername_callback);
      SSL_CTX_set_tlsext_servername_arg(ctx, &sslCertLookup);
#endif /* TS_USE_TLS_SNI */

      this->ssl = make_ssl_connection(ctx, this);
      if (this->ssl == NULL) {
        Debug("ssl", "SSLNetVConnection::sslServerHandShakeEvent, ssl create failed");
        SSLNetProcessor::logSSLError("SSL_StartHandShake");
        return EVENT_ERROR;
      }
    }

    return sslServerHandShakeEvent(err);
  } else {
    if (ssl == NULL) {
      ssl = make_ssl_connection(ssl_NetProcessor.client_ctx, this);
    }
    ink_assert(event == SSL_EVENT_CLIENT);
    return (sslClientHandShakeEvent(err));
  }

}

int
SSLNetVConnection::sslServerHandShakeEvent(int &err)
{
  int ret;

  ret = SSL_accept(ssl);
  switch (SSL_get_error(ssl, ret)) {
  case SSL_ERROR_NONE:
    Debug("ssl", "SSLNetVConnection::sslServerHandShakeEvent, handshake completed successfully");
    client_cert = SSL_get_peer_certificate(ssl);
    if (client_cert != NULL) {
/*		str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);
		Free (str);

		str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0);
		Free (str);

		// Add any extra client cert verification stuff here.  SSL
		// is set up in SSLNetProcessor::start to automatically verify
		// the client cert's CA, if required.
*/
      X509_free(client_cert);
    }
    sslHandShakeComplete = 1;

#if TS_USE_TLS_NPN
    {
      const unsigned char * proto = NULL;
      unsigned len = 0;

      SSL_get0_next_proto_negotiated(ssl, &proto, &len);
      if (len) {
        if (this->npnSet) {
          this->npnEndpoint = this->npnSet->findEndpoint(proto, len);
          this->npnSet = NULL;
        }
        Debug("ssl", "client selected next protocol %.*s", len, proto);
      } else {
        Debug("ssl", "client did not select a next protocol");
      }
    }
#endif /* TS_USE_TLS_NPN */

    return EVENT_DONE;

  case SSL_ERROR_WANT_ACCEPT:
    break;

  case SSL_ERROR_WANT_CONNECT:
    return SSL_HANDSHAKE_WANT_CONNECT;

  case SSL_ERROR_WANT_WRITE:
    return SSL_HANDSHAKE_WANT_WRITE;

  case SSL_ERROR_WANT_READ:
    return SSL_HANDSHAKE_WANT_READ;
  case SSL_ERROR_WANT_X509_LOOKUP:
    Debug("ssl", "SSLNetVConnection::sslServerHandShakeEvent, would block on read or write");
    break;

  case SSL_ERROR_ZERO_RETURN:
    Debug("ssl", "SSLNetVConnection::sslServerHandShakeEvent, EOS");
    return EVENT_ERROR;
    break;

  case SSL_ERROR_SYSCALL:
    err = errno;
    Debug("ssl", "SSLNetVConnection::sslServerHandShakeEvent, syscall");
    return EVENT_ERROR;
    break;

  case SSL_ERROR_SSL:
  default:
    err = errno;
    Debug("ssl", "SSLNetVConnection::sslServerHandShakeEvent, error");
    SSLNetProcessor::logSSLError("SSL_ServerHandShake");
    return EVENT_ERROR;
    break;
  }
  return EVENT_CONT;
}


int
SSLNetVConnection::sslClientHandShakeEvent(int &err)
{
  int ret;

  ret = SSL_connect(ssl);
  switch (SSL_get_error(ssl, ret)) {
  case SSL_ERROR_NONE:
    Debug("ssl", "SSLNetVConnection::sslClientHandShakeEvent, handshake completed successfully");
    server_cert = SSL_get_peer_certificate(ssl);

/*	  str = X509_NAME_oneline (X509_get_subject_name (server_cert),0,0);
	  Free (str);

	  str = X509_NAME_oneline (X509_get_issuer_name  (server_cert),0,0);
	  Free (str);
*/

/*	 Add certificate verification stuff here before
     deallocating the certificate.
*/

    X509_free(server_cert);
    sslHandShakeComplete = 1;

    return EVENT_DONE;

  case SSL_ERROR_WANT_WRITE:
    return SSL_HANDSHAKE_WANT_WRITE;

  case SSL_ERROR_WANT_READ:
    return SSL_HANDSHAKE_WANT_READ;

  case SSL_ERROR_WANT_X509_LOOKUP:
    Debug("ssl", "SSLNetVConnection::sslClientHandShakeEvent, would block on read or write");
    break;

  case SSL_ERROR_WANT_ACCEPT:
    return SSL_HANDSHAKE_WANT_ACCEPT;

  case SSL_ERROR_WANT_CONNECT:
    break;

  case SSL_ERROR_ZERO_RETURN:
    Debug("ssl", "SSLNetVConnection::sslClientHandShakeEvent, EOS");
    return EVENT_ERROR;

  case SSL_ERROR_SYSCALL:
    err = errno;
    Debug("ssl", "SSLNetVConnection::sslClientHandShakeEvent, syscall");
    return EVENT_ERROR;
    break;


  case SSL_ERROR_SSL:
  default:
    err = errno;
    SSLNetProcessor::logSSLError("sslClientHandShakeEvent");
    return EVENT_ERROR;
    break;

  }
  return EVENT_CONT;

}

void
SSLNetVConnection::registerNextProtocolSet(const SSLNextProtocolSet * s)
{
  ink_release_assert(this->npnSet == NULL);
  this->npnSet = s;
}

int
SSLNetVConnection::advertise_next_protocol(
    SSL *ssl, const unsigned char **out, unsigned int *outlen, void *arg)
{
  SSLNetVConnection * netvc = (SSLNetVConnection *)SSL_get_app_data(ssl);

  ink_release_assert(netvc != NULL);

  if (netvc->npnSet && netvc->npnSet->advertiseProtocols(out, outlen)) {
    // Successful return tells OpenSSL to advertise.
    return SSL_TLSEXT_ERR_OK;
  }

  return SSL_TLSEXT_ERR_NOACK;
}
