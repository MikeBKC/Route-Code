/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1991-2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:	$Id: rc4.h,v 1.1.1.1 2011/08/17 11:10:51 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/




/* rc4.h */
#ifndef _RC4_H
#define _RC4_H
struct rc4_key
{      
     unsigned char state[256];       
     unsigned char x;        
     unsigned char y;
};

void prepare_key(unsigned char *key_data_ptr, int key_data_len, struct rc4_key *key);
void rc4(unsigned char *buffer_ptr, int buffer_len, struct rc4_key * key);

#endif
   
