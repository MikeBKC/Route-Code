/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc.    This document may not be  copied,    ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1991-1993 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:     $Id: base64.c,v 1.1.1.1 2011/08/17 11:10:51 brwang Exp $
 *
 * File:	base64.c
 *
 * Overview:	en/de base-64 routines
 */


//#include "gendef.h"
#include "ddns.h"

#undef	BASE64_DEBUG

#ifdef	BASE64_DEBUG
# define PRINTF(x) printf x
#else
# define PRINTF(x)
#endif

/*
 * Produce a Vardata binary data field, given base64 text input.
 *
 * Base64 converts 3-octet chunks of binary input into 4-character
 * chunks of output, repeating until the input is exhausted.
 *
 * The 4 characters in each 4 character chunk of base64 data map to 
 * the 24 bits of information carried in each 3-octet chunk of input.
 *
 * When the input is not an even multiple of 3 octets, base64 data
 * files are padded with "=" signs such that the resulting base64
 * data files are always an even multiple of 4 characters.  This
 * provides a modicum of error-checking on the protocol.
 *
 * One additional note about base64 data format:  The 6 bits of
 * information kept in the base64 data stream are encoded into
 * printable characters using the following mapping:
 *
 * 0x00 <-> 'A'
 * 0x19 <-> 'Z'
 * 0x1A <-> 'a'
 * 0x33 <-> 'z'
 * 0x34 <-> '0'
 * 0x3D <-> '9'
 * 0x3E <-> '+'
 * 0x3F <-> '/'
 *
 * Remember that the pad character for base64 data is '='.
 *
 * Characters not in the above list (such as newline) have no effect, 
 * and newline characters are inserted in the base64 data stream such 
 * that base64 data lines are <= 76 characters in length.
 */

static const unsigned char _bs2bin[256] =
 {
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0 - 7 ^@^A^B^C^D^E^F^G */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 8 - 15 ^H^I^J^K^L^M^N^O */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 16 - 23 ^P^Q^R^S^T^U^V^W */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 24 - 31 ^X^Y^Z^[^\^]^^^_ */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 32 - 39  !"#$%&' " */
 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3f, /* 40 - 47 ()*+,-./ */
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, /* 48 - 55 01234567 */
 0x3c, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 56 - 63 89:;<=>? */
 0x00, 0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* 64 - 71 @ABCDEFG */
 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, /* 72 - 79 HIJKLMNO */
 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, /* 80 - 87 PQRSTUVW */
 0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, /* 88 - 95 XYZ[\]^_ */
 0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, /* 96 - 103 `abcdefg */
 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, /* 104 - 111 hijklmno */
 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, /* 112 - 119 pqrstuvw */
 0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, /* 120 - 127 xyz{|}~^? */
 };

/* read a base64 character sequence, appending the bytes onto the
 * given destination field.  The end of the field is known by its
 * length described by its first two bytes.
 *
 * Note: the size of the "length" is exactly two bytes, not an intrinsic
 * data type.  This results from the selection of exactly two bytes for
 * retrieving the length from the field.
 *
 * PS.  I am sure this should be said somewhere else as well as here,
 * but we SHOULD NOT convert the length calculation from "network byte
 * order" to internal format, since we do not want to have to guarantee
 * that a char array is aligned properly for all possible CPU types.
 *
 * Of course, we could set up a variable data field as a union of two 
 * elements, one of which is a char array, the other being an unsigned
 * short.  Then we could use the native integer format at the expense of
 * introducing padding to meed alignment restrictions.
 * (sounds complicated, consistent byte ordering works everywhere,
 * let's not bother)
 */
Int		
deBase64( Uint8		*dest,
	  const Char	*src )
{
  Int c ;
  Int val = 0 ;
  
  Int cindx = 0 ;
  Int coutx = 0 ;
  Int bits = 0 ;
  
  while( (c = src[cindx]) )
    {
      if( (c == '\n') || (c == '\r') )
	break ;
      
      cindx++ ;
      
      if (_bs2bin[c])
	{
	  val = (val << 6) | (_bs2bin[c] & 0x3f);
	  bits += 6;
	  
	  if (bits >= 8)
	    {
	      dest[coutx] = (val >> (bits - 8)) & 0xff ;
	      coutx++ ;
	      bits -= 8;
	    }
	}
      else
	{
	  if( c == '=' )
	    {
	      if( bits == 0 )
		{
		  /* base64 error */
		  return 0;
		}
	      
	      bits -= 2 ;
	    }
	}
    }	
  
  if( bits != 0 )
    {
      /* base64 error */
      return 0;
    }

  return (coutx);
}

static const unsigned char _bin2bs[64] =
{
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 
'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 
'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', 
'8', '9', '+', '/'
} ;

/* Convert binary data to base64 format. */

extern void
enBase64( const Uint8  *blp,
	  const Int	len,
	  Char	       *outlp )
{
  Int c_in ;
  Int c_out ;
  
  Uint32 outb ;
  Int bits ;
  Int charno ;
  Int outidx ;

  /* zero length binary data implies zero length output string. */
  if( len == 0 )
    {
      outlp[0] = 0 ;
      return ;
    }

  outidx = 0 ;
  
  outb = bits = charno = 0 ;
  while( charno < len )
    {
      c_in = blp[charno++] ;
      
      outb = (outb << 8) | (c_in & 0xff) ;
      bits += 8 ;
      
      while( bits >= 6 )
	{
	  bits -= 6 ;
	  c_out = (outb >> bits) & 0x3F ;
	  outlp[outidx++] = _bin2bs[c_out] ;
	}
    }

  /* If there are any bits left over after the main loop, 
   * get them out here.  We know that there can't be >=6,
   * or they would have been produced by the main loop.
   */
  if( bits > 0 )
    {
      PRINTF(("\nend: bits=%d\n", bits ));
      outb <<= 8 ;
      bits += 2 ;
      c_out = (outb >> bits) & 0x3F ;
      outlp[outidx++] = _bin2bs[c_out] ;
    }

    /* Always complete on a "high note": an even multiple of 4 characters.
     * do this by producing '=' until bits==8, implying that it's done.
     */
  while( bits != 0 )
    {
      PRINTF(( "\nend-loop: bits=%d\n", bits ));
      outlp[outidx++] = '=' ;
      charno++ ;
      bits += 2 ;
      if( bits >= 8 )
	break ;
    }

  outlp[outidx++] = 0 ;

  return ;
}
