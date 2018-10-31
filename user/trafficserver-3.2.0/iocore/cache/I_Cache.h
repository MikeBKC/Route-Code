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

#ifndef _I_CACHE_H__
#define _I_CACHE_H__

#ifndef TS_INLINE
#define TS_INLINE
#endif

#include "libts.h"
#include "I_EventSystem.h"
#include "I_AIO.h"
#include "I_CacheDefs.h"
#include "I_Store.h"

#define CACHE_MODULE_MAJOR_VERSION 1
#define CACHE_MODULE_MINOR_VERSION 0
#define CACHE_MODULE_VERSION       makeModuleVersion(CACHE_MODULE_MAJOR_VERSION,\
						   CACHE_MODULE_MINOR_VERSION,\
						   PUBLIC_MODULE_HEADER)

#define CACHE_WRITE_OPT_OVERWRITE       0x0001
#define CACHE_WRITE_OPT_CLOSE_COMPLETE  0x0002
#define CACHE_WRITE_OPT_SYNC            (CACHE_WRITE_OPT_CLOSE_COMPLETE | 0x0004)
#define CACHE_WRITE_OPT_OVERWRITE_SYNC  (CACHE_WRITE_OPT_SYNC | CACHE_WRITE_OPT_OVERWRITE)

#define SCAN_KB_PER_SECOND      8192 // 1TB/8MB = 131072 = 36 HOURS to scan a TB

#define RAM_CACHE_ALGORITHM_CLFUS        0
#define RAM_CACHE_ALGORITHM_LRU          1

#define CACHE_COMPRESSION_NONE           0
#define CACHE_COMPRESSION_FASTLZ         1
#define CACHE_COMPRESSION_LIBZ           2
#define CACHE_COMPRESSION_LIBLZMA        3

struct CacheVC;
#ifdef HTTP_CACHE
class CacheLookupHttpConfig;
class URL;
class HTTPHdr;
class HTTPInfo;

typedef HTTPHdr CacheHTTPHdr;
typedef URL CacheURL;
typedef HTTPInfo CacheHTTPInfo;
#endif

struct CacheProcessor:public Processor
{
  virtual int start(int n_cache_threads = 0 /* cache uses event threads */ );
  virtual int start_internal(int flags = 0);
  void stop();

  int dir_check(bool fix);
  int db_check(bool fix);

  inkcoreapi Action *lookup(Continuation *cont, CacheKey *key,
                            bool local_only = false,
                            CacheFragType frag_type = CACHE_FRAG_TYPE_NONE, char *hostname = 0, int host_len = 0);
  inkcoreapi Action *open_read(Continuation *cont, CacheKey *key,
                               CacheFragType frag_type = CACHE_FRAG_TYPE_NONE, char *hostname = 0, int host_len = 0);
  Action *open_read_buffer(Continuation *cont, MIOBuffer *buf, CacheKey *key,
                           CacheFragType frag_type = CACHE_FRAG_TYPE_NONE, char *hostname = 0, int host_len = 0);

  inkcoreapi Action *open_write(Continuation *cont,
                                CacheKey *key,
                                CacheFragType frag_type = CACHE_FRAG_TYPE_NONE,
                                int expected_size = CACHE_EXPECTED_SIZE,
                                int options = 0,
                                time_t pin_in_cache = (time_t) 0,
                                char *hostname = 0, int host_len = 0);
  Action *open_write_buffer(Continuation *cont, MIOBuffer *buf,
                            CacheKey *key,
                            CacheFragType frag_type = CACHE_FRAG_TYPE_NONE,
                            int options = 0,
                            time_t pin_in_cache = (time_t) 0,
                            char *hostname = 0, int host_len = 0);
  inkcoreapi Action *remove(Continuation *cont, CacheKey *key,
                            CacheFragType frag_type = CACHE_FRAG_TYPE_NONE,
                            bool rm_user_agents = true, bool rm_link = false,
                            char *hostname = 0, int host_len = 0);
  Action *scan(Continuation *cont, char *hostname = 0, int host_len = 0, int KB_per_second = SCAN_KB_PER_SECOND);
#ifdef HTTP_CACHE
  Action *lookup(Continuation *cont, URL *url, bool local_only = false,
                 CacheFragType frag_type = CACHE_FRAG_TYPE_HTTP);
  inkcoreapi Action *open_read(Continuation *cont, URL *url,
                               CacheHTTPHdr *request,
                               CacheLookupHttpConfig *params,
                               time_t pin_in_cache = (time_t) 0, CacheFragType frag_type = CACHE_FRAG_TYPE_HTTP);
  Action *open_read_buffer(Continuation *cont, MIOBuffer *buf, URL *url,
                           CacheHTTPHdr *request,
                           CacheLookupHttpConfig *params, CacheFragType frag_type = CACHE_FRAG_TYPE_HTTP);
  Action *open_write(Continuation *cont, int expected_size, URL *url,
                     CacheHTTPHdr *request, CacheHTTPInfo *old_info,
                     time_t pin_in_cache = (time_t) 0, CacheFragType frag_type = CACHE_FRAG_TYPE_HTTP);
  Action *open_write_buffer(Continuation *cont, MIOBuffer *buf, URL *url,
                            CacheHTTPHdr *request, CacheHTTPHdr *response,
                            CacheFragType frag_type = CACHE_FRAG_TYPE_HTTP);
  Action *remove(Continuation *cont, URL *url, CacheFragType frag_type = CACHE_FRAG_TYPE_HTTP);

  Action *open_read_internal(int, Continuation *, MIOBuffer *, CacheURL *,
                             CacheHTTPHdr *, CacheLookupHttpConfig *,
                             CacheKey *, time_t, CacheFragType type, char *hostname, int host_len);
#endif
  Action *link(Continuation *cont, CacheKey *from, CacheKey *to,
               CacheFragType frag_type = CACHE_FRAG_TYPE_HTTP, char *hostname = 0, int host_len = 0);

  Action *deref(Continuation *cont, CacheKey *key,
                CacheFragType frag_type = CACHE_FRAG_TYPE_HTTP, char *hostname = 0, int host_len = 0);
  static int IsCacheEnabled();

  static unsigned int IsCacheReady(CacheFragType type);

  // private members
  void diskInitialized();

  void cacheInitialized();

  static volatile uint32_t cache_ready;
  static volatile int initialized;
  static volatile int start_done;
  static int clear;
  static int fix;
  static int start_internal_flags;
  static int auto_clear_flag;
};

struct CacheVConnection:public VConnection
{
  VIO *do_io_read(Continuation *c, int64_t nbytes, MIOBuffer *buf) = 0;
  virtual VIO *do_io_pread(Continuation *c, int64_t nbytes, MIOBuffer *buf, int64_t offset) = 0;
  VIO *do_io_write(Continuation *c, int64_t nbytes, IOBufferReader *buf, bool owner = false) = 0;
  void do_io_close(int lerrno = -1) = 0;
  void reenable(VIO *avio) = 0;
  void reenable_re(VIO *avio) = 0;
  void do_io_shutdown(ShutdownHowTo_t howto)
  {
    (void) howto;
    ink_assert(!"CacheVConnection::do_io_shutdown unsupported");
  }

  virtual int get_header(void **ptr, int *len) = 0;
  virtual int set_header(void *ptr, int len) = 0;
  virtual int get_single_data(void **ptr, int *len) = 0;

#ifdef HTTP_CACHE
  virtual void set_http_info(CacheHTTPInfo *info) = 0;
  virtual void get_http_info(CacheHTTPInfo **info) = 0;
#endif

  virtual bool is_ram_cache_hit() = 0;
  virtual bool set_disk_io_priority(int priority) = 0;
  virtual int get_disk_io_priority() = 0;
  virtual bool set_pin_in_cache(time_t t) = 0;
  virtual time_t get_pin_in_cache() = 0;
  virtual int64_t get_object_size() = 0;

  CacheVConnection();
};

void ink_cache_init(ModuleVersion version);
extern inkcoreapi CacheProcessor cacheProcessor;
extern Continuation *cacheRegexDeleteCont;

#endif /* _I_CACHE_H__ */
