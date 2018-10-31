#ifndef __APSCAN_SHARE_H__
#define __APSCAN_SHARE_H__


typedef struct st_ap_scan_profile {
    mibHead head;
    int maxAp;
    FunEnableEnum autoScan;
    FunEnableEnum scanAll;
}ApScanProfile;




#endif


