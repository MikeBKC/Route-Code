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

/**************************************
 *
 * BaseManager.h
 *   Base Manager Class, base class for all managers.
 *
 * $Date: 2013/06/03 15:59:25 $
 *
 *
 */

#ifndef _BASE_MANAGER_H
#define _BASE_MANAGER_H

#include "ink_thread.h"
#include "ink_mutex.h"
#include "ink_llqueue.h"

#include "MgmtDefs.h"


/*******************************************
 * used by LocalManager and in Proxy Main. *
 */
#define MAX_OPTION_SIZE        2048
#define MAX_PROXY_SERVER_PORTS 2048
#define MAX_ATTR_LEN           5
/*******************************************/

/*
 * MgmtEvent defines.
 */
// Event flows: traffic manager -> traffic server
#define MGMT_EVENT_SYNC_KEY              10000
#define MGMT_EVENT_SHUTDOWN              10001
#define MGMT_EVENT_RESTART               10002
#define MGMT_EVENT_BOUNCE                10003
#define MGMT_EVENT_CLEAR_STATS           10004
#define MGMT_EVENT_CONFIG_FILE_UPDATE    10005
#define MGMT_EVENT_PLUGIN_CONFIG_UPDATE  10006
#define MGMT_EVENT_HTTP_CLUSTER_DELTA    10007
#define MGMT_EVENT_ROLL_LOG_FILES        10008
#define MGMT_EVENT_LIBRECORDS            10009

/***********************************************************************
 *
 * MODULAIZATTION: if you are adding new signals, please ensure to add
 *                 the corresponding signals in librecords/I_RecSignals.h
 *
 *
 ***********************************************************************/

// Signal flows: traffic server -> traffic manager
#define MGMT_SIGNAL_PID                   0
#define MGMT_SIGNAL_MACHINE_UP            1     /* Data is ip addr */
#define MGMT_SIGNAL_MACHINE_DOWN          2
#define MGMT_SIGNAL_CONFIG_ERROR          3     /* Data is descriptive string */
#define MGMT_SIGNAL_SYSTEM_ERROR          4
#define MGMT_SIGNAL_LOG_SPACE_CRISIS      5
#define MGMT_SIGNAL_CONFIG_FILE_READ      6
#define MGMT_SIGNAL_CACHE_ERROR           7
#define MGMT_SIGNAL_CACHE_WARNING         8
#define MGMT_SIGNAL_LOGGING_ERROR         9
#define MGMT_SIGNAL_LOGGING_WARNING       10
// Currently unused: 11
// Currently unused: 12
#define MGMT_SIGNAL_PLUGIN_ADD_REC        13
#define MGMT_SIGNAL_PLUGIN_SET_CONFIG     14
#define MGMT_SIGNAL_LOG_FILES_ROLLED      15
#define MGMT_SIGNAL_LIBRECORDS            16
#define MGMT_SIGNAL_HTTP_CONGESTED_SERVER   20  /* Congestion control -- congested server */
#define MGMT_SIGNAL_HTTP_ALLEVIATED_SERVER  21  /* Congestion control -- alleviated server */

// Wireless (WDA) plugin signal
// Warning: 1. Don't change the # mapping 'cuase 3rd party software depends on them.
//          2. Ensure 1:1 maps with alarms in Alarm.h
#define INK_MGMT_SIGNAL_WDA_BILLING_CONNECTION_DIED  100
#define INK_MGMT_SIGNAL_WDA_BILLING_CORRUPTED_DATA   101
#define INK_MGMT_SIGNAL_WDA_XF_ENGINE_DOWN           102
#define INK_MGMT_SIGNAL_WDA_RADIUS_CORRUPTED_PACKETS 103

#define INK_MGMT_SIGNAL_ACC_ALARMS_START  200
#define INK_MGMT_SIGNAL_ACC_ALARMS_END    299

#define INK_MGMT_SIGNAL_SAC_SERVER_DOWN			400

typedef struct _mgmt_message_hdr_type
{
  int msg_id;
  int data_len;
} MgmtMessageHdr;


typedef struct _mgmt_event_callback_list
{
  MgmtCallback func;
  void *opaque_data;
  struct _mgmt_event_callback_list *next;
} MgmtCallbackList;


class BaseManager
{

public:

  BaseManager();
  ~BaseManager();

  int registerMgmtCallback(int msg_id, MgmtCallback func, void *opaque_callback_data = NULL);

  int signalMgmtEntity(int msg_id, char *data_str = NULL);
  int signalMgmtEntity(int msg_id, char *data_raw, int data_len);

  LLQ *mgmt_event_queue;
  InkHashTable *mgmt_callback_table;

protected:

  void executeMgmtCallback(int msg_id, char *data_raw, int data_len);

private:

};                              /* End class BaseManager */


#endif /* _BASE_MANAGER_H */
