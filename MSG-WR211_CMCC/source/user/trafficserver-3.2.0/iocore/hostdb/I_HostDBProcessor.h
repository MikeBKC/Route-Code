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

#ifndef _I_HostDBProcessor_h_
#define _I_HostDBProcessor_h_

#include "I_EventSystem.h"
#include "SRV.h"

// Event returned on a lookup
#define EVENT_HOST_DB_LOOKUP                 (HOSTDB_EVENT_EVENTS_START+0)
#define EVENT_HOST_DB_IP_REMOVED             (HOSTDB_EVENT_EVENTS_START+1)
#define EVENT_HOST_DB_GET_RESPONSE           (HOSTDB_EVENT_EVENTS_START+2)

#define EVENT_SRV_LOOKUP                 (SRV_EVENT_EVENTS_START+0)
#define EVENT_SRV_IP_REMOVED             (SRV_EVENT_EVENTS_START+1)
#define EVENT_SRV_GET_RESPONSE           (SRV_EVENT_EVENTS_START+2)

#define HOST_DB_MAX_ROUND_ROBIN_INFO         16

//
// Data
//
struct HostDBContinuation;

//
// The host database stores host information, most notably the
// IP address.
//
// Since host information is relatively small, we can afford to have
// a reasonable size memory cache, and use a (relatively) sparce
// disk representation to decrease # of seeks.
//
extern int hostdb_enable;
extern unsigned int hostdb_current_interval;
extern unsigned int hostdb_ip_stale_interval;
extern unsigned int hostdb_ip_timeout_interval;
extern unsigned int hostdb_ip_fail_timeout_interval;
extern unsigned int hostdb_serve_stale_but_revalidate;


//
// Types
//

//
// This structure contains the host information required by
// the application.  Except for the initial fields it
// is treated as opacque by the database.
//

union HostDBApplicationInfo
{
  struct application_data_allotment
  {
    unsigned int application1;
    unsigned int application2;
  } allotment;

  //////////////////////////////////////////////////////////
  // http server attributes in the host database          //
  //                                                      //
  // http_version       - one of HttpVersion_t            //
  // pipeline_max       - max pipeline.     (up to 127).  //
  //                      0 - no keep alive               //
  //                      1 - no pipeline, only keepalive //
  // keep_alive_timeout - in seconds. (up to 63 seconds). //
  // last_failure       - UNIX time for the last time     //
  //                      we tried the server & failed    //
  // fail_count         - Number of times we tried and    //
  //                       and failed to contact the host //
  //////////////////////////////////////////////////////////
  struct http_server_attr
  {
    unsigned int http_version:3;
    unsigned int pipeline_max:7;
    unsigned int keepalive_timeout:6;
    unsigned int fail_count:8;
    unsigned int unused1:8;
    unsigned int last_failure:32;
  } http_data;

  enum HttpVersion_t
  {
    HTTP_VERSION_UNDEFINED = 0,
    HTTP_VERSION_09 = 1,
    HTTP_VERSION_10 = 2,
    HTTP_VERSION_11 = 3
  };

  struct application_data_rr
  {
    int offset;
  } rr;
};

struct HostDBRoundRobin;

struct HostDBInfo
{
  /** Internal IP address data.
      This is at least large enough to hold an IPv6 address.
  */
  sockaddr* ip()  { return &data.ip.sa; }
  sockaddr const* ip() const { return &data.ip.sa; }

  char *hostname();
  char *srvname();
  HostDBRoundRobin *rr();

  /** Indicate that the HostDBInfo is BAD and should be deleted. */
  void bad() { full = 0; }

  /** Check the HostDBInfo or selected RR entry of a HostDBInfo is ok. */
  int ok(bool byname, HostDBInfo * rr = NULL) {
    if (rr) {
      if (!byname ||
          rr->md5_high != md5_high ||
          rr->md5_low != md5_low || rr->md5_low_low != md5_low_low || rr->reverse_dns || !rr->ip())
        goto Lbad;
    } else if (byname) {
      if (reverse_dns)
        goto Lbad;
      if (!ats_is_ip(ip()))
        goto Lbad;
    } else {
      if (!reverse_dns)
        goto Lbad;
      if (!hostname())
        goto Lbad;
    }
    return 1;
  Lbad:
    bad();
    return 0;
  }

  /**
    Application specific data. NOTE: We need an integral number of these
    per block. This structure is 32 bytes. (at 200k hosts = 8 Meg). Which
    gives us 7 bytes of application information.

  */
  HostDBApplicationInfo app;

  unsigned int ip_interval()
  {
    return (hostdb_current_interval - ip_timestamp) & 0x7FFFFFFF;
  }

  int ip_time_remaining()
  {
    return (int) ip_timeout_interval - (int) ((hostdb_current_interval - ip_timestamp) & 0x7FFFFFFF);
  }

  bool is_ip_stale() {
    if (ip_timeout_interval >= 2 * hostdb_ip_stale_interval)
      return ip_interval() >= hostdb_ip_stale_interval;
    else
      return false;
  }

  bool is_ip_timeout() {
    return ip_interval() >= ip_timeout_interval;
  }

  bool is_ip_fail_timeout() {
    return ip_interval() >= hostdb_ip_fail_timeout_interval;
  }

  void refresh_ip()
  {
    ip_timestamp = hostdb_current_interval;
  }

  bool serve_stale_but_revalidate() {
    // the option is disabled
    if (hostdb_serve_stale_but_revalidate <= 0)
      return false;

    // ip_timeout_interval == DNS TTL
    // hostdb_serve_stale_but_revalidate == number of seconds
    // ip_interval() is the number of seconds between now() and when the entry was inserted
    if ((ip_timeout_interval + hostdb_serve_stale_but_revalidate) > ip_interval()) {
      Debug("hostdb", "serving stale entry %d | %d | %d as requested by config",
            ip_timeout_interval, hostdb_serve_stale_but_revalidate, ip_interval());
      return true;
    }
    // otherwise, the entry is too old
    return false;
  }


  /**
    These are the only fields which will be inserted into the
    database. Any new user fields must be added to this function.

  */
  void set_from(HostDBInfo const& that)
  {
    memcpy(&data, &that.data, sizeof data);
    ip_timestamp = that.ip_timestamp;
    ip_timeout_interval = that.ip_timeout_interval;
    round_robin = that.round_robin;
    reverse_dns = that.reverse_dns;
    app.allotment.application1 = that.app.allotment.application1;
    app.allotment.application2 = that.app.allotment.application2;
  }


  //
  // Private
  //

  union {
    IpEndpoint ip; ///< IP address / port data.
    int hostname_offset; ///< Some hostname thing.
  } data;

  unsigned int srv_weight:16;
  unsigned int srv_priority:16;
  unsigned int srv_port:16;
  unsigned int srv_count:15;
  unsigned int is_srv:1;

  unsigned int ip_timestamp;
  // limited to 0x1FFFFF (24 days)
  unsigned int ip_timeout_interval;

  unsigned int full:1;
  unsigned int backed:1;        // duplicated in lower level
  unsigned int deleted:1;
  unsigned int hits:3;

  unsigned int round_robin:1;
  unsigned int reverse_dns:1;

  unsigned int md5_low_low:24;
  unsigned int md5_low;

  uint64_t md5_high;

  bool failed() { return !ats_is_ip(ip()); }
  void set_failed() { ats_ip_invalidate(ip());  }

  void set_deleted() { deleted = 1; }
  bool is_deleted() const { return deleted; }

  bool is_empty() const { return !full; }

  void set_empty()
  {
    full = 0;
    md5_high = 0;
    md5_low = 0;
    md5_low_low = 0;
    is_srv = 0;
    srv_weight = 0;
    srv_priority = 0;
    srv_port = 0;
    srv_count = 0;
  }

  void set_full(uint64_t folded_md5, int buckets)
  {
    uint64_t ttag = folded_md5 / buckets;

    if (!ttag)
      ttag = 1;
    md5_low_low = (unsigned int) ttag;
    md5_low = (unsigned int) (ttag >> 24);
    full = 1;
  }

  void reset()
  {
    ats_ip_invalidate(ip());
    app.allotment.application1 = 0;
    app.allotment.application2 = 0;
    backed = 0;
    deleted = 0;
    hits = 0;
    round_robin = 0;
    reverse_dns = 0;
  }

  uint64_t tag() {
    uint64_t f = md5_low;
    return (f << 24) + md5_low_low;
  }

  bool match(INK_MD5 &, int, int);
  int heap_size();
  int *heap_offset_ptr();

HostDBInfo()
  : srv_weight(0)
  , srv_priority(0)
  , srv_port(0)
  , srv_count(0)
  , is_srv(0)
  , ip_timestamp(0)
  , ip_timeout_interval(0)
  , full(0)
  , backed(0)
  , deleted(0)
  , hits(0)
  , round_robin(0)
  , reverse_dns(0)
  , md5_low_low(0)
  , md5_low(0), md5_high(0) {
    app.allotment.application1 = 0;
    app.allotment.application2 = 0;
    ats_ip_invalidate(ip());

    return;
  }
};


struct HostDBRoundRobin
{
  /** Total number (to compute space used). */
  short n;

  /** Number which have not failed a connect. */
  short good;

  unsigned short current;
  ink_time_t timed_rr_ctime;

  HostDBInfo info[HOST_DB_MAX_ROUND_ROBIN_INFO];
  char rr_srv_hosts[HOST_DB_MAX_ROUND_ROBIN_INFO][MAXDNAME];

  static int size(int nn, bool using_srv)
  {
    if (using_srv) {
      /*     sizeof this struct
         minus
         unused round-robin entries [info]
         minus
         unused srv host data [rr_srv_hosts]
       */
      return (int) ((sizeof(HostDBRoundRobin)) -
                    (sizeof(HostDBInfo) * (HOST_DB_MAX_ROUND_ROBIN_INFO - nn)) -
                    (sizeof(char) * MAXDNAME * (HOST_DB_MAX_ROUND_ROBIN_INFO - nn)));
    } else {
      return (int) (sizeof(HostDBRoundRobin) -
                    sizeof(HostDBInfo) * (HOST_DB_MAX_ROUND_ROBIN_INFO - nn) -
                    sizeof(char) * MAXDNAME * HOST_DB_MAX_ROUND_ROBIN_INFO);
    }
  }

  HostDBInfo *find_ip(sockaddr const* addr);
  HostDBInfo *select_best(sockaddr const* client_ip, HostDBInfo * r = NULL);
  HostDBInfo *select_best_http(sockaddr const* client_ip, ink_time_t now, int32_t fail_window);

  HostDBInfo *increment_round_robin()
  {
    bool bad = (n <= 0 || n > HOST_DB_MAX_ROUND_ROBIN_INFO || good <= 0 || good > HOST_DB_MAX_ROUND_ROBIN_INFO);

    if (bad) {
      ink_assert(!"bad round robin size");
      return NULL;
    }
    current++;
    return NULL;
  }

  HostDBRoundRobin()
    : n(0), good(0), current(0), timed_rr_ctime(0)
  { }

};

struct HostDBCache;

// Prototype for inline completion functionf or
//  getbyname_imm()
typedef void (Continuation::*process_hostdb_info_pfn) (HostDBInfo * r);
typedef void (Continuation::*process_srv_info_pfn) (HostDBInfo * r);


/** The Host Databse access interface. */
struct HostDBProcessor: public Processor
{
  // Public Interface

  // Lookup Hostinfo by name
  //    cont->handleEvent( EVENT_HOST_DB_LOOKUP, HostDBInfo * ); on success
  //    cont->handleEVent( EVENT_HOST_DB_LOOKUP, 0); on failure
  // Failure occurs when the host cannot be DNS-ed
  // NOTE: Will call the continuation back before returning if data is in the
  //       cache.  The HostDBInfo * becomes invalid when the callback returns.
  //       The HostDBInfo may be changed during the callback.

  enum
  { HOSTDB_DO_NOT_FORCE_DNS = 0,
    HOSTDB_ROUND_ROBIN = 0,
    HOSTDB_FORCE_DNS_RELOAD = 1,
    HOSTDB_FORCE_DNS_ALWAYS = 2,
    HOSTDB_DO_NOT_ROUND_ROBIN = 4
  };

  HostDBProcessor()
  { }

  inkcoreapi Action *getbyname_re(Continuation * cont, const char *hostname, int len = 0, int port = 0,
                                  int flags = HOSTDB_DO_NOT_FORCE_DNS);

  Action *getSRVbyname_imm(Continuation * cont, process_srv_info_pfn process_srv_info, const char *hostname, int len = 0,
                           int port = 0, int flags = HOSTDB_DO_NOT_FORCE_DNS, int timeout = 0);

  Action *getbyname_imm(
    Continuation * cont,
    process_hostdb_info_pfn process_hostdb_info,
    const char *hostname,
    int len = 0,
    int port = 0,
    int flags = HOSTDB_DO_NOT_FORCE_DNS,
    int timeout = 0
  );


  /** Lookup Hostinfo by addr */
  Action *getbyaddr_re(Continuation * cont, sockaddr const* aip)
  {
    return getby(cont, NULL, 0, 0, aip, false);
  }


  /**
    If you were unable to connect to an IP address associated with a
    particular hostname, call this function and that IP address will
    be marked "bad" and if the host is using round-robin DNS, next time
    you will get a different IP address.

  */
  Action *failed_connect_on_ip_for_name(
    Continuation * cont,
    sockaddr const* aip,
    const char *hostname, int len = 0
  );

  /** Set the application information (fire-and-forget). */
  void setbyname_appinfo(char *hostname, int len, int port, HostDBApplicationInfo * app)
  {
    sockaddr_in addr;
    ats_ip4_set(&addr, INADDR_ANY, port);
    setby(hostname, len, ats_ip_sa_cast(&addr), app);
  }

  void setbyaddr_appinfo(sockaddr const* addr, HostDBApplicationInfo * app) {
    this->setby(0, 0, addr, app);
  }

  void setbyaddr_appinfo(in_addr_t ip, HostDBApplicationInfo * app)
  {
    sockaddr_in addr;
    ats_ip4_set(&addr, ip);
    this->setby(0, 0, ats_ip_sa_cast(&addr), app);
  }

  /** Configuration. */
  static int hostdb_strict_round_robin;
  static int hostdb_timed_round_robin;

  // Processor Interface
  /* hostdb does not use any dedicated event threads
   * currently. Dont pass any value to start
   */
  int start(int no_of_additional_event_threads = 0);

  // Private
  HostDBCache *cache();
  Action *getby(
    Continuation * cont,
    const char *hostname, int len,
    sockaddr const* ip,
    bool aforce_dns, int timeout = 0
  );
  /** Set something.
      @a aip can carry address and / or port information. If setting just
      by a port value, the address should be set to INADDR_ANY which is of
      type IPv4.
   */
  void setby(
    const char *hostname, ///< Hostname.
    int len, ///< Length of hostname.
    sockaddr const* aip, ///< Address and/or port.
    HostDBApplicationInfo * app ///< I don't know.
  );

};

void run_HostDBTest();

extern inkcoreapi HostDBProcessor hostDBProcessor;

void ink_hostdb_init(ModuleVersion version);

#endif
