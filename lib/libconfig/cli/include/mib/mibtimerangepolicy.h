/*
 * add by brwang for reos 2008
 */

#ifndef MIB_TIMERANGE_POLICY
#define MIB_TIMERANGE_POLICY
typedef struct st_timeRangePolicy{
    char        name[MAX_PROFILE_NAME_LENGTH + 1];
    Uint8       active;
    char        tmgname[MAX_PROFILE_NAME_LENGTH + 1];
    Boolean	antiP2pEnabled;
    Boolean	antiQQEnabled;
    Boolean	antiMsnEnabled;
    NPW_SPEED	maxRxRate;
    NPW_SPEED	maxTxRate;
    Uint32 	ipCreditDRR;
    Uint32 	ipCreditDRR2;
    char        noLimitGroup[MAX_PROFILE_NAME_LENGTH + 1];
    Uint32                          pdbPolicy;//added by brwang for pdb can auto show in web

}TimeRangePolicyProfile;

//#define MAX_INREF_TIMERANGE_COUNT 2

#endif
