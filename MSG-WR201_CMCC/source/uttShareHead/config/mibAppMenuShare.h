#ifndef MIBAPP_MENU_SHARE_H
#define MIBAPP_MENU_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
#define MAX_SHORTCUT_ITEMS 5

/*ҳ���ݲ˵���*/
typedef struct st_shortCutMenu {
    mibHead head;
    char appName[5][50];
} ShortCutMenuProfile;

#endif
#endif
#endif
