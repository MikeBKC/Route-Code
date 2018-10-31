/*
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


//////////////////////////////////////////////////////////////////////////////////////////////
// 
// Implement the classes for the various types of hash keys we support.
//
#ifndef __LULU_H__
#define __LULU_H__ 1

#include "ink_atomic.h"

// Define UNUSED properly.
#if ((__GNUC__ >= 3) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 7)))
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif /* #if ((__GNUC__ >= 3) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 7))) */

#include <sys/types.h>

// Memory barriers on i386 / linux / gcc
#if defined(__i386__)
#define mb()  __asm__ __volatile__ ( "lock; addl $0,0(%%esp)" : : : "memory" )
#define rmb() __asm__ __volatile__ ( "lock; addl $0,0(%%esp)" : : : "memory" )
#define wmb() __asm__ __volatile__ ( "" : : : "memory")
#elif defined(__x86_64__)
#define mb()  __asm__ __volatile__ ( "mfence" : : : "memory")
#define rmb() __asm__ __volatile__ ( "lfence" : : : "memory")
#define wmb() __asm__ __volatile__ ( "" : : : "memory")
#else
#error "Define barriers"
#endif

// Used for Debug etc.
static const char* PLUGIN_NAME = "header_filter";
static const char* PLUGIN_NAME_DBG = "header_filter_dbg";

// From google styleguide: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
  TypeName(const TypeName&);                    \
  void operator=(const TypeName&)


#endif // __LULU_H__



/*
  local variables:
  mode: C++
  indent-tabs-mode: nil
  c-basic-offset: 2
  c-comment-only-line-offset: 0
  c-file-offsets: ((statement-block-intro . +)
  (label . 0)
  (statement-cont . +)
  (innamespace . 0))
  end:

  Indent with: /usr/bin/indent -ncs -nut -npcs -l 120 logstats.cc
*/
