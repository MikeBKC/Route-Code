#ifndef _LANG_SHARE_H_
#define _LANG_SHARE_H_

#define MAX_LANG_LEN  15

#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == FYES)
#define LANG_DEFAULT  "zhcn" 
#elif (CONFIG_USER_GOAHEAD_LANG_EN == FYES)
#define LANG_DEFAULT  "en" 
#elif (CONFIG_USER_GOAHEAD_LANG_ZHTW == FYES)
#define LANG_DEFAULT  "zhtw" 
#endif
typedef struct _lang_profile{
    mibHead head;
    char lang[MAX_LANG_LEN+1];
}LangProfile;


#endif
