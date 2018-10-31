#ifndef ARGCMD_H
#define ARGCMD_H

#include "conver.h"

extern const char *getEnumString(const LcdParamEnumValue *enumPara, int value);
extern int getEnumValue(const LcdParamEnumValue *enumPara, char *string);

#endif
