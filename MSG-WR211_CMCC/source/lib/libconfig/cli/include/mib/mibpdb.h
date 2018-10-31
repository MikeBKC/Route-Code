#if 0
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
 
#if ! defined ( _MIBPDB_H )
# define _MIBPDB_H

/*
 * CNC, TEL, AutoRun, CMCC, PrioGame
 */
#define MAX_SYS_RES_PDB		 5
#define MAX_PDB_PROFILES	(52 + MAX_SYS_RES_PDB)

#define MAX_PDB_LINE_COUNT 30	//modify by jyshen 060915
#define MAX_PDB_LINE_LENGTH 128

typedef enum PdbMode {
        Predefined=0,
        Userdefined
}PdbMode;

typedef enum PdbType {
	Filter=0,
	Route,
	AutoCmd,	//add by jyshen 070308
        DnsPdb,// add by brwang
        UrlPdb,// add by brwang
	PriorityPdb
}PdbType;


typedef enum  pdbCategory{
	PDB_IM=0,
	PDB_P2P,
	PDB_DNS,	//add by jyshen 070308
	PDB_URL,	//add by jyshen 070308
	PDB_GAME,
        PDB_OTHER
}PdbCategory;

typedef struct st_pdb_profile {
        char name[MAX_PROFILE_NAME_LENGTH+1];
	Uint32      createtime;	//add by jyshen 060803
        Boolean     active;
	PdbMode     mode;
	PdbType     type;
        PdbCategory category;
        int         index;
	char        description[128];
        char        line[MAX_PDB_LINE_COUNT][MAX_PDB_LINE_LENGTH+1];
} PdbProfile;
extern PdbProfile *pdbProf;

#endif /* _MIBPDB_H */
#endif
