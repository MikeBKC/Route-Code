
#include "CWWTP.h"

int getSysSn(void)
{

    return  0x123456;
}

int getHwVersion(void)
{

    return  0x020000;
}

int getSwVersion(void)
{

    return  0x020500;
}

const char* getModelName(void)
{
    static const char*name = "UTT AP";

    return name;
}
