#include    <linux/autoconf.h>
#include	<string.h>
#include	"mib.h"
#include	"profacce.h"

#if (SETTINGS_CONF == FYES)
extern int get_file_len(char* fn);
int formUploadSettings(char *filename)
{
    char *     fn = filename;
    char *     bn = NULL;
    int        ret = 0;
    bool  reset = 0;
    char *postData;
    int fnlen; 
	int faultcode = 0;
#if (UTT_LICENSE == FYES)
    LicenseProfile uttlicense;
    LicenseProfile *licensePtr;
#endif
	reset = 0;

    if (fn != NULL && *fn != '\0') {
	if ((int)(bn = wcsrchr(fn, '/') + 1) == 1) {
	    if ((int)(bn = wcsrchr(fn, '\\') + 1) == 1) {
		bn = fn;
	    }
	}
    }

    fnlen = get_file_len(fn);
    if((postData = malloc(fnlen)) == NULL)
    {
        faultcode = 9019;
        goto OUT;
    }

    FILE * fp = NULL;
    if((fp = fopen(fn,"r")) == NULL)
    {
        faultcode = 9019;
        goto OUT;
    }
    if(fread(postData,1,fnlen,fp) == 0)
    {
        faultcode = 9019;
        goto OUT;
    }

    if( fnlen > 0 ){
        if(postData) {
#if (UTT_LICENSE == FYES)
            memset(&uttlicense, 0, sizeof(LicenseProfile));
            licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
            if((licensePtr != NULL)){
                memcpy(&uttlicense, licensePtr,sizeof(LicenseProfile));
            }
#endif
            postData[fnlen] = '\0';/*去除其他无用的字符*/

            //"文件格式错误",
            if(parseProfileFromStr(postData, reset) != 0 )/*合并配置到内存*/
            {
                faultcode = 9019;
                goto OUT;
            }
            else
            {
                ProfWiteToFlash();/*写flash*/
            }
#if (UTT_LICENSE == FYES)
            licensePtr =(LicenseProfile *)ProfGetInstPointByIndex(MIB_PROF_LICENSE, 0);
            if((licensePtr != NULL)){
                memcpy(licensePtr,&uttlicense,sizeof(LicenseProfile));
            }
#endif
        }
    }
    ret = 1;
OUT:
    if(postData) {
        free(postData);
    }
    if(fp) {
        fclose(fp);
    }
    return faultcode;
}
#if (WHITELIST == 1)
int check_url(char* t,int len)
{
    char *tmp = t;

    while(tmp <= (t + len))
    {   
        if(*tmp == '\n' || *tmp == '\r')
            *tmp = 0;
        tmp++;
    }
    return 0;
}
int formUploadWifiDogSettings(char *filename)
{
    char *     fn = filename;
    char StrLine[1024] = {0};
	int faultcode = 0;
    FILE * fp = NULL;
    int  index = 0;
    int  i = 0;
    int max,min ;

    MibProfileType mibType = MIB_PROF_WHITE_LIST;
    WhiteListProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD_ALL;
    struProfAlloc *profhead = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    index = min;

    if((fp = fopen(fn,"r")) == NULL)
    {
        faultcode = 9019;
        goto OUT;
    }

    ProfBackupByIndex(mibType, profAction, index, &profhead);
    while (!feof(fp) && (index < max))
    {
        memset(StrLine,0,sizeof(StrLine));
        fgets(StrLine,1024,fp);//读取一行
        check_url(StrLine,strlen(StrLine));
        printf("%s ---- %d ; strline : %s\n",__func__,__LINE__,StrLine);
#if 1
        if(1)
        {
            /* Backup */
            prof = (WhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
            /* Change value */
            strcpy(prof->domain, StrLine);
            index++;
        }
#endif
    }
    /* Effect immediately */
    ProfUpdate(profhead);
    /* Release */
    ProfFreeAllocList(profhead);
    /* Write config */
    nvramWriteCommit();

OUT:
    if(fp) {
        fclose(fp);
    }
    return faultcode;
}
#endif

#if 0
int main(int argc,char** argv)
{
    ProfInit();
    if(argc != 2)
    {
        cwmp_log_debug("usage:\n\t%s filename\n","update");
        return 0;
    }
    cwmp_log_debug("filename : %s\n",argv[1]);
    formUploadSettings(argv[1]);
    ProfDetach();
    return 0;
}
#endif
#endif
