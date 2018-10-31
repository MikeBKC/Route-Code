/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                         *
 *                                                                      *
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#ifndef __CWMP_X_UTIL_H__
#define __CWMP_X_UTIL_H__

#include <cwmp/types.h>
#include <cwmp/cwmp.h>
typedef unsigned long long uint64;
typedef struct _sw_port_stat_t{
    uint64 rxPkts;
    uint64 txPkts;

    uint64 rxOctets;
    uint64 txOctets;

    uint64 rxPktsErrors;
    uint64 txPktsErrors;
}sw_port_stat_t;
#endif

