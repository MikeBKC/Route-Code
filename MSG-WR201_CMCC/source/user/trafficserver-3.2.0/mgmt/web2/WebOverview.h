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

#ifndef _WEB_OVERVIEW_H_
#define _WEB_OVERVIEW_H_

#include "ink_hash_table.h"
#include "ink_mutex.h"
#include "WebGlobals.h"
#include "TextBuffer.h"
#include "List.h"

#include "WebHttpContext.h"
#include "WebHttpMessage.h"
#include "ExpandingArray.h"
#include "ClusterCom.h"
#include "Alarms.h"

#include "P_RecCore.h"

/****************************************************************************
 *
 *  WebOverview.h - code to overview page
 *
 *
 ****************************************************************************/

//
//  There is one instance of the class overviewPage in the LocalManger
//    process.  The overviewPage instance stores a record of type
//    overviewRecord for each node that has been seen in the cluster.
//    The node records contain a list of active alarms on that node.
//
//  overviewPage is responsible for the synchronization issues for both
//    it self and all of its overviewRecords.  Whenever updates are made
//    to instances of either class, overviewPage's accessLock must be held.
//
//  Pointers to overviewRecords are stored in overviewPage::nodeRecords
//    hash table which is indexed on the nodes ip address (as an
//    unsigned long).  A second mapping is also maintained in sortRecords.
//
//
//  Additional Notes
//
//  These classes have expanded over time.  overviewPage and
//    overviewRecord are now clearing houses of cluster information
//    for the UI.
//  To simplify the locking issues, pointer to overviewRecords should
//    NOT be returned by overviewPage.  overviewRecords are internal
//    to overviewPage and any data needed from an overviewRecord
//    should be returned as a copy (or a const ptr)
//    through an accessor function

class AlarmListable
{
public:
  ~AlarmListable();
  alarm_t type;
  char *ip;
  char *desc;
  LINK(AlarmListable, link);
};

enum PowerLampState
{ LAMP_ON, LAMP_OFF, LAMP_WARNING };

// information about a specific node in the cluster
class overviewRecord
{
public:
  overviewRecord(unsigned long inet_addr, bool local, ClusterPeerInfo * cpi = NULL);
   ~overviewRecord();
  void addAlarm(alarm_t type, char *ip, char *desc);
  void addAlarm(AlarmListable * newAlarm);
  void updateStatus(time_t currentTime, ClusterPeerInfo * cpi);
  void getStatus(char **hotsname, bool * up, bool * alarms, PowerLampState * proxyUp);
  bool ipMatch(char *ipStr);    // is this the ip address of this node
  bool up;
  bool localNode;
  char *hostname;               // FQ hostname of the node
  unsigned long inetAddr;       // IP address of the node
  DLL<AlarmListable> nodeAlarms;   // List of alarms for the node
  void checkAlarms();
  bool varStrFromName(const char *varName, char *bufVal, int bufLen);
  RecCounter readCounter(const char *name, bool * found);
  RecInt readInteger(const char *name, bool * found);
  RecFloat readFloat(const char *name, bool * found);
  RecString readString(const char *name, bool * found);
  bool varIntFromName(const char *varName, RecInt * value);
  bool varFloatFromName(const char *varName, RecFloat * value);
  bool varCounterFromName(const char *varName, RecCounter * value);

private:
  RecRecords node_rec_data;   // a copy from ClusterPeerInfo
  int recordArraySize;          // the size of node_data.recs
  int node_rec_first_ix; // Kludge, but store the first order ix for later use
  overviewRecord(const overviewRecord &);
};

// information about the entire cluster
class overviewPage
{
public:
  overviewPage();
  ~overviewPage();

  void addAlarm(alarm_t newAlarm, char *ip, char *desc);
  void checkForUpdates();
  char *resolvePeerHostname(const char *peerIP);
  char *resolvePeerHostname_ml(const char *peerIP);
  int getClusterHosts(ExpandingArray * hosts);
  MgmtInt readInteger(const char *nodeName, const char *name, bool * found = NULL);
  MgmtFloat readFloat(const char *nodeName, const char *name, bool * found = NULL);
  MgmtString readString(const char *nodeName, const char *name, bool * found = NULL);
  void addSelfRecord();

  int varClusterFloatFromName(char *, MgmtFloat *);

private:
  ink_mutex accessLock;

  // Private fcns
    overviewPage(const overviewPage &);
  void addRecord(ClusterPeerInfo * cpi);
  overviewRecord *findNodeByName(const char *nodeName);
  void addReading(MgmtInt reading, textBuffer * output, int nDigits, const char **gifs, const char **alts);
  void addLoadBar(textBuffer * output, MgmtInt load);
  void sortHosts();
  bool moreInfoButton(const char *submission, textBuffer * output);
  void addHostPanel(WebHttpContext * whc, overviewRecord * host);

  // Private variables
  InkHashTable *nodeRecords;    // container for overviewRecords
  unsigned long ourAddr;        // the IP address of this node
  ExpandingArray sortRecords;   // A second, sorted container for nodeRecords
  int numHosts;                 // number of peers we know about including ourself
    DLL<AlarmListable> notFoundAlarms;       // List of alarms for nodes we haven't seen

  // Functions to do cluster aggregation
  //
  void clusterAgInt(const char *clustVar, const char *nodeVar);
  void clusterAgIntScale(const char *clustVar, const char *nodeVar, double factor);
#define MB_SCALE (1/(1024*1024.0))
  int clusterSumInt(const char *nodeVar, MgmtInt * sum);
  int clusterSumCounter(char *nodeVar, MgmtInt * sum);
  void clusterAgFloat(const char *clusterVar, const char *nodeVar);
  int clusterSumFloat(const char *nodeVar, MgmtFloat * sum);
  void agHostdbHitRate();
  void agCacheHitRate();
  void agCachePercentFree();
  void agBandwidthHitRate();
  void agConnections();
};

extern overviewPage *overviewGenerator; // global handle to overiewPage?
                                        // defn found in WebOverview.cc

void overviewAlarmCallback(alarm_t newAlarm, char *ip, char *desc);
void resolveAlarm(InkHashTable * post_data_ht);
int hostSortFunc(const void *arg1, const void *arg2);

#endif
