#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"
#include "translate.h"
#include    "wsIntrn.h"
#include    "um.h"
#include    "utils.h"
#include    "mib.h"
#include    "profacce.h"
#include    "md5.h"
#include    "conver.h"
#include    "uttfunction.h"
#define SERVER_IP "202.101.35.232"
#define SERVER_PORT 69
#define PDB_MAX_LENGTH 1024
#if (UTTPDB == FYES)
static int getPdbUpdataConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);

    char_t *policyNames= T("policyNames[%d] = \"%s\"; ");
    char_t *types= T("types[%d] = \"%d\"; ");
    char_t *descriptions= T("descriptions[%d] = \"%s\"; ");
    websWrite(wp,"var policyNames =new Array();");
    websWrite(wp,"var types = new Array();");
    websWrite(wp,"var descriptions =new Array();");

    int totalrecs = 0;
    for(i = min; i < max; i++) {
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    websWrite(wp, policyNames, totalrecs, prof->head.name);
	    websWrite(wp, types, totalrecs, prof->category);
	    websWrite(wp, descriptions, totalrecs, prof->description);
	    totalrecs++;
	}
    }
    websWrite(wp,"var totalrecs=%d;",totalrecs);
    getTheErrorMsg(eid, wp, argc, argv);
    getTheRightMsg(eid, wp, argc, argv);
    return 0;
}

char PdbUpFromWhichPage[0x20];
static int getUpPdbFromWhichPage(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp, "var MainPage=\"%s\";", PdbUpFromWhichPage);
    return 0;
}
void setUpdataPdbFromWhichPage(char *page)
{
    strncpy(PdbUpFromWhichPage, page, 20);
    return;
}

bool updateOneProfilePdb(PdbProfile *profPdb, char_t *policyName)
{
    MD5_CONTEXT		md5ctx;
    char_t file_name[50]={0};
    unsigned char	hash[16];
    FILE *fp;
    int fileS, fileE, fileL;
    Uchar *tmp;
    char md5[512] = {'\0'};
 //   char md5_2[512] = {'\0'};
    char temp[3] = {'\0'};
 //   char temp2[3] = {'\0'};
    int i;
    const char *errMsg=NULL;

    doSystem("cd /etc_ro/l7-protocols/ && tftp -g -r SE_%s.xml %s %d",policyName,SERVER_IP,SERVER_PORT); 
    sprintf(file_name,"/etc_ro/l7-protocols/SE_%s.xml",policyName);

    if((fp=fopen(file_name,"r")) == NULL)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_UPDATE_FAIL);
	setTheErrorMsg(errMsg);
	return FALSE;
    }
    fseek(fp, 0, SEEK_SET);
    fileS = ftell(fp);
    fseek(fp, 0, SEEK_END);
    fileE = ftell(fp);
    fileL = fileE - fileS;
    if(fileL < 32) {
	errMsg = getTransDataForC(C_LANG_INDEX_UPDATE_FAIL);
	setTheErrorMsg(errMsg);
	fclose(fp);
	return FALSE;
    }
    tmp = (Uchar*)malloc(fileL);
    memset(tmp, 0, fileL);
    if(tmp == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_UPDATE_FAIL);
	setTheErrorMsg(errMsg);
	fclose(fp);
	return FALSE;
    }else {
	fseek(fp, 0, SEEK_SET);
	fread(tmp, fileL, 1, fp);
	fclose(fp);
	tmp[fileL-1]='\0';
	MD5Init(&md5ctx);
	MD5Update(&md5ctx, tmp, (unsigned int)(fileL-32));
	MD5Final(hash, &md5ctx);
	for(i=0; i< 16; i++)
	{   
	    sprintf(temp, "%02X",hash[i]);
	    strcat(md5,temp);
	}   
	DBGPRINT("hash[%s]\n",md5);
	//DBGPRINT("md5_2[%s]\n",tmp+strlen(tmp)-32+1);
	if(memcmp(md5, tmp+strlen((char*)tmp)-32+1, 30) == 0) {
	    DBGPRINT("md5 check ok\n");
	    *(tmp+fileL-32) = 0;
	    DBGPRINT("%s :%d policyName tmp = \n%s\n",__FUNCTION__,__LINE__,tmp);
	    doSystem("cat /dev/null > %s",file_name); 
	    if((fp=fopen(file_name,"w+")) == NULL)
	    {
		     DBGPRINT("%s %d\n",__FUNCTION__,__LINE__);
		errMsg = getTransDataForC(C_LANG_INDEX_UPDATE_FAIL);
		setTheErrorMsg(errMsg);
		return FALSE;
	    }
	    fputs((char*)tmp, fp);
	    fclose(fp);
	    if(converConfigFromXmlFile(file_name) == TRUE){


		DBGPRINT("up upconfig OK!\n");
		free(tmp);
	//	doSystem("rm %s",file_name); 
		return TRUE;
	    }
	    else
	    {
		DBGPRINT("up upconfig error!\n");
		free(tmp);
	//	doSystem("rm %s",file_name); 
		return FALSE;
	    }
	} else {
	    errMsg = getTransDataForC(C_LANG_INDEX_MD5_UPDATE_FAIL);
	    setTheErrorMsg(errMsg);
	    return FALSE;
	}
    }
}

static void formUpdatePdbConfig(webs_t wp, char_t *path, char_t *query)
{

    MibProfileType mibTypePdb = MIB_PROF_PDB_GLOBAL;
    struProfAlloc *profhead = NULL;
    PdbProfile *profPdb = NULL;
    ProfChangeType profAction;

    const char *errMsg=NULL;
    char_t *policyName;
    policyName = websGetVar(wp,T("policyNames"),NULL);
    DBGPRINT("policyName = %s\n",policyName);
    profAction = PROF_CHANGE_EDIT;

    if(strcmp(policyName, "All") == 0)
    {
	/*通过改变BitTorrent实例的值，触发pdb的profilechang  每次全部更新必然触发profilechange          */
	profPdb = (PdbProfile*)ProfGetInstPointByName(mibTypePdb, "BitTorrent");
	strcpy(profPdb->line[0] , "");
	ProfBackupByName(mibTypePdb, profAction, "BitTorrent", &profhead);
#if (STATIC_ROUTE == FYES)
	/*
	 * 全部更新时备份路由pdb
	 */
	ProfBackupByName(mibTypePdb, profAction, "10000", &profhead);
	ProfBackupByName(mibTypePdb, profAction, "10010", &profhead);
	ProfBackupByName(mibTypePdb, profAction, "10086", &profhead);
#endif
    }
    else{
	ProfBackupByName(mibTypePdb, profAction, policyName, &profhead);
    }
    profPdb = (PdbProfile*)ProfGetInstPointByName(mibTypePdb, policyName);

    if(policyName == NULL){
	errMsg = getTransDataForC(C_LANG_INDEX_UPDATE_FAIL);
	setTheErrorMsg(errMsg);
	return;
    }
    //	websRedirect(wp, "pdbup.asp");

    if(updateOneProfilePdb(profPdb, policyName) == TRUE){
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
	errMsg = getTransDataForC(C_LANG_INDEX_UPDATE_SUCCESS);
	setTheRightMsg(errMsg);
    }

 #if (STATIC_ROUTE == FYES)
    if (strcmp(path, "formUpISPRoute") == 0) { /* 如果是WANConfig页面做的pdb更新 */
	websRedirect(wp,"WANConfig.asp");
    } 
    else 
#endif
    {
	websRedirect(wp,"PdbUpdata.asp");
    }

}
void formDefinePdbConfig(void)
{
    websAspDefine(T("aspformPdbUpdata"), getPdbUpdataConfig);
    websFormDefine(T("formPdbUpConfig"),formUpdatePdbConfig);
    websAspDefine(T("aspUpPdbFromWhichPage"), getUpPdbFromWhichPage);
#if (STATIC_ROUTE == FYES)
    /*
     * 线路配置更新路由策略
     */
    websFormDefine(T("formUpISPRoute"),formUpdatePdbConfig);
#endif
}
#endif
