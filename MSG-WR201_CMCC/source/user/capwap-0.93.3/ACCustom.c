#include "CWAC.h"

int CWListMsg(char* buffer)
{
    int num = 0, len=0, offset;
    int i, nLtoSend, iTosend, val;
    char *pData;

    if(buffer==NULL)
    {
	return 0;
    }
    offset = sizeof(int);
    pData = buffer;
    for(i=0; i<CW_MAX_WTP; i++) {
	if(gWTPs[i].isNotFree) {
	    num++;
	    iTosend = htonl(i);
	    memcpy(pData+offset, &iTosend, sizeof(int));		  
	    offset += sizeof(int);

	    if(gWTPs[i].WTPProtocolManager.name!=NULL) {
		len = strlen(gWTPs[i].WTPProtocolManager.name);
		nLtoSend = htonl(len);
		memcpy(pData+offset, &nLtoSend, sizeof(int));		  
		offset += sizeof(int);

		memcpy(pData+offset, gWTPs[i].WTPProtocolManager.name, len);		  
		offset += len;
	    }
	    else {
		len = 0;
		nLtoSend = htonl(len);
		memcpy(pData+offset, &nLtoSend, sizeof(int));		  
		offset += sizeof(int);
            }

	    len = strlen("UTT AP");
	    if(len>MAX_MODEL_NAME){
	        len = MAX_MODEL_NAME;
	    }
	    nLtoSend = htonl(len);
	    memcpy(pData+offset, &nLtoSend, sizeof(int));		  
	    offset += sizeof(int);

            memcpy(pData+offset, "UTT AP", len);		  
	    offset += len;

	    val = 0x124532;/*sn*/
	    val = htonl(val);
            memcpy(pData+offset, &val, sizeof(int));		  
	    offset += sizeof(int);

	    val = 0x02050000;/*swVersion*/
	    val = htonl(val);
            memcpy(pData+offset, &val, sizeof(int));		  
	    offset += sizeof(int);

	    val = 0x01000000;/*HwVersion*/
	    val = htonl(val);
            memcpy(pData+offset, &val, sizeof(int));		  
	    offset += sizeof(int);
	}
    }
    num = htonl(num);
    memcpy(pData, &num, sizeof(int));		  

    return offset;
}

