#include <tommath.h>
#ifdef BN_MP_RADIX_SMAP_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://math.libtomcrypt.com
 */

/* chars used in radix conversions */
const char *mp_s_rmap = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
#endif

/* $Source: /home/cvs/3520G/user/dropbear-2012.55/libtommath/bn_mp_radix_smap.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2012/10/26 16:27:17 $ */
