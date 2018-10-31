#ifndef MIBCLIFORENGINEERONLY_SHARE_H
#define MIBCLIFORENGINEERONLY_SHARE_H

#include "mibunionShare.h"
#include "mibtypeShare.h"

#define COMMAND_LENGTH 128

typedef struct st_CliForEngineerOnly_Profile{
    mibHead head;
    char command[COMMAND_LENGTH];
}CliForEngineerOnlyProfile;
#endif
