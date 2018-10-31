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

/****************************************************************************

  ink_atomic.h

  This file defines atomic memory operations.

  On a Sparc V9, ink_atomic.c must be compiled with gcc and requires
  the argument "-Wa,-xarch=v8plus" to get the assembler to emit V9
  instructions.


 ****************************************************************************/

#ifndef _ink_atomic_h_
#define	_ink_atomic_h_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "ink_port.h"

#include "ink_apidefs.h"
#include "ink_mutex.h"

typedef volatile int8_t vint8;
typedef volatile int16_t vint16;
typedef volatile int32_t vint32;
typedef volatile int64_t vint64;
typedef volatile long vlong;
typedef volatile void *vvoidp;

typedef vint8 *pvint8;
typedef vint16 *pvint16;
typedef vint32 *pvint32;
typedef vint64 *pvint64;
typedef vlong *pvlong;
typedef vvoidp *pvvoidp;

// Sun/Solaris and the SunPRO compiler
#if defined(__SUNPRO_CC)
typedef volatile uint8_t vuint8;
typedef volatile uint16_t vuint16;
typedef volatile uint32_t vuint32;
#if __WORDSIZE == 64
typedef unsigned long uint64_s;
#else
typedef uint64_t uint64_s;
#endif
typedef volatile uint64_s vuint64_s;
typedef vuint8 *pvuint8;
typedef vuint16 *pvuint16;
typedef vuint32 *pvuint32;
typedef vuint64_s *pvuint64_s;

#include <atomic.h>

/* see http://docs.oracle.com/cd/E19082-01/819-2243/6n4i098m6/index.html */

// ink_atomic_swap(ptr, value)
// Writes @value into @ptr, returning the previous value.
template <typename T> inline T *
ink_atomic_swap(volatile T * mem, T value) {
  return (T *)atomic_swap_ptr((volatile void *)mem, value);
}

// ink_atomic_cas(mem, prev, next)
// Atomically store the value @next into the pointer @mem, but only if the current value at @mem is @prev.
// Returns true if @next was successfully stored.
template <typename T> inline bool
ink_atomic_cas(volatile T * mem, T prev, T next) {
  return prev == (T)atomic_cas_ptr((volatile void *)mem, (void *)prev, (void *)next);
}

template <> inline bool
ink_atomic_cas<uint8_t>(volatile uint8_t * mem, uint8_t prev, uint8_t next) {
  return prev == atomic_cas_8((volatile uint8_t *)mem, prev, next);
}


// ink_atomic_increment(ptr, count)
// Increment @ptr by @count, returning the previous value.
template <typename Type, typename Amount> inline Type
ink_atomic_increment(volatile Type * mem, Amount count);

template <> inline uint8_t
ink_atomic_increment<uint8_t, int>(volatile uint8_t * mem, int count) {
  return atomic_add_8_nv(mem, (int8_t)count) - count;
}

static inline int8_t  ink_atomic_swap(pvint8 mem, int8_t value) { return (int8_t)atomic_swap_8((pvuint8)mem, (uint8_t)value); }
static inline int16_t ink_atomic_swap(pvint16 mem, int16_t value) { return (int16_t)atomic_swap_16((pvuint16)mem, (uint16_t)value); }
static inline int32_t ink_atomic_swap(pvint32 mem, int32_t value) { return (int32_t)atomic_swap_32((pvuint32)mem, (uint32_t)value); }
static inline int64_t ink_atomic_swap(pvint64 mem, int64_t value) { return (int64_t)atomic_swap_64((pvuint64_s)mem, (uint64_s)value); }
static inline void *  ink_atomic_swap(vvoidp mem, void *value) { return atomic_swap_ptr((vvoidp)mem, value); }

static inline bool ink_atomic_cas(pvint32 mem, int old, int new_value) { return atomic_cas_32((pvuint32)mem, (uint32_t)old, (uint32_t)new_value) == old; }
static inline bool ink_atomic_cas(pvint64 mem, int64_t old, int64_t new_value) { return atomic_cas_64((pvuint64_s)mem, (uint64_s)old, (uint64_s)new_value) == old; }
static inline bool ink_atomic_cas(pvvoidp mem, void* old, void* new_value) { return atomic_cas_ptr((vvoidp)mem, old, new_value) == old; }

static inline int     ink_atomic_increment(pvint32 mem, int value) { return ((uint32_t)atomic_add_32_nv((pvuint32)mem, (uint32_t)value)) - value; }
static inline int     ink_atomic_increment(pvint32 mem, unsigned value) { return ((uint32_t)atomic_add_32_nv((pvuint32)mem, (uint32_t)value)) - value; }
static inline int     ink_atomic_increment(pvint32 mem, long value) { return ((uint32_t)atomic_add_32_nv((pvuint32)mem, (uint32_t)value)) - value; }
static inline int64_t ink_atomic_increment(pvint64 mem, int64_t value) { return ((uint64_s)atomic_add_64_nv((pvuint64_s)mem, (uint64_s)value)) - value; }
static inline int64_t ink_atomic_increment(pvint64 mem, int value) { return ((uint64_s)atomic_add_64_nv((pvuint64_s)mem, (uint64_s)value)) - value; }
static inline void *  ink_atomic_increment(pvvoidp mem, intptr_t value) { return (void*)(((char*)atomic_add_ptr_nv((vvoidp)mem, (ssize_t)value)) - value); }
static inline void *  ink_atomic_increment(pvvoidp mem, void* value) { return (void*)(((char*)atomic_add_ptr_nv((vvoidp)mem, (ssize_t)value)) - (ssize_t)value); }

/* not used for Intel Processors or Sparc which are mostly sequentally consistent */
#define INK_WRITE_MEMORY_BARRIER
#define INK_MEMORY_BARRIER

/* GCC compiler >= 4.1 */
#elif defined(__GNUC__) && (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 1)

/* see http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html */

// ink_atomic_swap(ptr, value)
// Writes @value into @ptr, returning the previous value.
template <typename T> static inline T
ink_atomic_swap(volatile T * mem, T value) {
  return __sync_lock_test_and_set(mem, value);
}

// ink_atomic_cas(mem, prev, next)
// Atomically store the value @next into the pointer @mem, but only if the current value at @mem is @prev.
// Returns true if @next was successfully stored.
template <typename T> static inline bool
ink_atomic_cas(volatile T * mem, T prev, T next) {
  return __sync_bool_compare_and_swap(mem, prev, next);
}

// ink_atomic_increment(ptr, count)
// Increment @ptr by @count, returning the previous value.
template <typename Type, typename Amount> static inline Type
ink_atomic_increment(volatile Type * mem, Amount count) {
  return __sync_fetch_and_add(mem, (Type)count);
}

// ink_atomic_decrement(ptr, count)
// Decrement @ptr by @count, returning the previous value.
template <typename Type, typename Amount> static inline Type
ink_atomic_decrement(volatile Type * mem, Amount count) {
  return __sync_fetch_and_sub(mem, (Type)count);
}

// Special hacks for ARM 32-bit
#if defined(__arm__) && (SIZEOF_VOIDP == 4)
extern ProcessMutex __global_death;

template<>
inline int64_t
ink_atomic_swap<int64_t>(pvint64 mem, int64_t value) {
  int64_t old;
  ink_mutex_acquire(&__global_death);
  old = *mem;
  *mem = value;
  ink_mutex_release(&__global_death);
  return old;
}

template<>
inline bool
ink_atomic_cas<int64_t>(pvint64 mem, int64_t old, int64_t new_value) {
  int64_t curr;
  ink_mutex_acquire(&__global_death);
  curr = *mem;
  if(old == curr) *mem = new_value;
  ink_mutex_release(&__global_death);
  if(old == curr) return 1;
  return 0;
}

template<>
inline int64_t
ink_atomic_increment<int64_t>(pvint64 mem, int64_t value) {
  int64_t curr;
  ink_mutex_acquire(&__global_death);
  curr = *mem;
  *mem = curr + value;
  ink_mutex_release(&__global_death);
  return curr + value;
}

template<>
inline int64_t
ink_atomic_increment<int64_t>(pvint64 mem, int value) {
  return ink_atomic_increment(mem, static_cast<int64_t>(value));
}

#endif /* Special hacks for ARM 32-bit */

/* not used for Intel Processors which have sequential(esque) consistency */
#define INK_WRITE_MEMORY_BARRIER
#define INK_MEMORY_BARRIER


#else /* not gcc > v4.1.2 */
#error Need a compiler / libc that supports atomic operations, e.g. gcc v4.1.2 or later
#endif

#endif                          /* _ink_atomic_h_ */
