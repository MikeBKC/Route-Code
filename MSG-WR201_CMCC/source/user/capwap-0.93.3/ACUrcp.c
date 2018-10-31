#include "CWAC.h"
#include <ctype.h>

#define CAPWAP_SOCKET  "/tmp/capwap_socket"
#define URCP_SOCKET    "/tmp/urcp_socket"
#define PACKET_SIZE	65536
//#define  URCP_DEBUG 
static int gUrcpSock=-1;
static struct sockaddr_un urcpAddr; 
static int capwapSock=-1;
static char buffer[PACKET_SIZE + 1];
static void sendUrcpMessage(int wtpIdx, CWProtocolMessage **completeMsgPtr, CWProtocolMessage *msgElems, const int msgElemNum);
#ifdef URCP_DEBUG 
int frame_dump(char *buf, int len)
{
    int i;

    if (len>72) len=72;
    for(i=0;i<len;i++)
    {
	if((i!=0) && ((i%16)==0))
	{
	    cprintf("\n");
	}
	cprintf("%02x ", (unsigned char)buf[i]);
    }
    cprintf("\n");

    return 0;
}
#endif
int mac_cmp(unsigned char *mac1, unsigned char* mac2)
{ 
    /*类型转换*/         
    unsigned short *a = (unsigned short *)mac1;
    unsigned short *b = (unsigned short *)mac2;

    /*比较*/
    return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) )!=0;
}

static int send_len(int fd, int len)
{                        
    int size, rc=-1;

    size = len;

    rc = send(fd, &size, sizeof(int), 0);

    return rc;
}
int getPacket(int sock, char *buffer, int size)
{
    int n, len;

    n = readn(sock, (char*)&len, 4);
    if(n==4)
    {
	n = readn(sock, buffer, len);
	if(n!=len){
	    n = -1;
	}
    }
    return n;
}
FILE *fd;
CW_THREAD_RETURN_TYPE CWACUrcp(void *arg)
{
    int rlen, len, i, broadcast, error, size;
    const int msgElemCount = 1;
    struct sockaddr_un capwapAddr; 
//    struct sockaddr_un urcpAddr; 
    CWProtocolMessage *completeMsgPtr = NULL;
    CWProtocolMessage *msgElems;

    CWThreadSetSignals(SIG_BLOCK, 1, SIGALRM);

    /*      Create a UNIX datagram socket for this thread        */
#ifdef CAPWAP_UDP
    if ((capwapSock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) 
#else
    if ((capwapSock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) 
#endif
    {
	CWDebugLog("THR URCP: Error creating socket");
	exit(1);
    }

    /*      Set up address structure for server socket      */
    bzero(&capwapAddr, sizeof(capwapAddr));
    capwapAddr.sun_family = AF_UNIX;
    strcpy(capwapAddr.sun_path, CAPWAP_SOCKET);

    unlink(CAPWAP_SOCKET);

#ifdef CAPWAP_UDP
    bzero(&urcpAddr, sizeof(urcpAddr));
    urcpAddr.sun_family = AF_UNIX;
    strcpy(urcpAddr.sun_path, URCP_SOCKET);
#endif
    if (bind(capwapSock, (struct sockaddr *)&capwapAddr, sizeof(capwapAddr)) < 0) 
    {
	CWDebugLog("THR URCP: Error binding socket");
	close(capwapSock);
	exit(1);
    }

    fd = fopen("/tmp/kk", "w");

    listen (capwapSock, 8);
    len = sizeof(urcpAddr);
    gUrcpSock = accept(capwapSock, (struct sockaddr *)&urcpAddr, (socklen_t *)&len);
    if(gUrcpSock == -1)
    {
	cprintf("accept error\n");
    }
#if 1
    //len = sizeof(size);
    //error = getsockopt(gUrcpSock, SOL_SOCKET, SO_SNDBUF, &size, &len);
    //cprintf("csize:%d\n", size);
    size = 20*1024;
    error = setsockopt(gUrcpSock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(int));
#endif
    /*      Receive data */
    CW_REPEAT_FOREVER 
    {
	CW_ZERO_MEMORY(buffer, sizeof(buffer));
#ifdef CAPWAP_UDP
	rlen = recvfrom(capwapSock, buffer,PACKET_SIZE, 0, (struct sockaddr *)&urcpAddr,(socklen_t *)&len);
#else
//	rlen = read(gUrcpSock, buffer, PACKET_SIZE);
	rlen = getPacket(gUrcpSock, buffer, PACKET_SIZE);
#endif
	if(rlen<=0)
	{
	    CWDebugLog("THR URCP: Error receiving from unix socket. errno=%d", errno);
#if 0
	    if(errno == 2) {/*可能urcp断开了*/
		CWDebugLog("THR URCP: sleep");
		gUrcpSock = accept(capwapSock, (struct sockaddr *)&urcpAddr, (socklen_t *)&len);
	    }
#endif
	    continue;
	}
#ifdef URCP_DEBUG 
	cprintf("\nrecv from urcp len:%d\n", rlen);
	frame_dump(buffer, rlen);
#endif
	if((buffer[0] & 0x01u) == 0) {
	    broadcast = 0;
	}
	else {
	    broadcast = 1;
	}

	completeMsgPtr = NULL;

        CW_CREATE_PROTOCOL_MSG_ARRAY_ERR(msgElems, msgElemCount, continue;);
        CW_CREATE_PROTOCOL_MESSAGE(*msgElems, 4+rlen, {free(msgElems);continue;});
	CWProtocolStore16(msgElems, CW_MSG_ELEMENT_URCP_PDU_CW_TYPE);
	CWProtocolStore16(msgElems, rlen);
	CWProtocolStoreRawBytes(msgElems, buffer, rlen);

	for(i = 0; i < CW_MAX_WTP; i++) {
            if( gWTPs[i].isNotFree && gWTPs[i].WTPProtocolManager.name 
		    && ( (broadcast==1) || mac_cmp(buffer, gWTPs[i].priv.WTPMac)==0)) {
                sendUrcpMessage(i, &completeMsgPtr, msgElems, msgElemCount);
	        if(broadcast==0) {
	            break;
		}
            }
        }
#ifdef URCP_DEBUG 
	if((i>=CW_MAX_WTP) && (broadcast==0))
	    cprintf("found no ap\n");
#endif
	for(i = 0; i < msgElemCount; i++) { CW_FREE_PROTOCOL_MESSAGE(msgElems[i]);}
	CW_FREE_OBJECT(msgElems);
    }

    close(capwapSock);
    return(NULL);
}

void CWsendtoUrcp(unsigned char *msg, int len)
{
    //int num=0;
    int rc;
#ifdef CAPWAP_UDP
    if(capwapSock != -1)
#else
    if(gUrcpSock != -1)
#endif
    {
#ifdef URCP_DEBUG 
	cprintf("send to urcp: len: %d\n", len);
	frame_dump(msg, len);
#endif
#if 0
	if((msg[9]==0xcd) && (msg[10]==0xe8) && (msg[11]==0x3d) && (len==66))
	{
	    num = *((int*)(msg+62));
	    num = ntohl(num);
	    fprintf(fd ,"%d ", num);
	}
#endif
	rc = send_len(gUrcpSock, len);
	{
	    if(send(gUrcpSock, msg, len, 0)<0) {
	        CWDebugLog("Failure sending PDU to URCP: errno=%d", errno);
	    }
	}
    }
}

CWBool CWACParseUrcpPDURequest(int wtpIdx, unsigned char *buff, int len)
{
    unsigned char *mac=NULL;
    short type, elemLen;

    type = *((short*)buff);
    type = ntohs(type);

    elemLen = *((short*)(buff+2));
    elemLen = ntohs(elemLen);
    if((type==CW_MSG_ELEMENT_URCP_PDU_CW_TYPE) &&  (elemLen == (len-4))) {
	CWsendtoUrcp(buff+4, elemLen);
	mac = buff + 4 + MAC_ADDR_LEN;
	memcpy(gWTPs[wtpIdx].priv.WTPMac, mac, MAC_ADDR_LEN);
    }
    return CW_TRUE;
}

CWBool CWACAssembleUrcpPDUResponse(CWProtocolMessage **messagesPtr,
	int *fragmentsNumPtr,
	int PMTU,
	int seqNum,
	CWProtocolResultCode resultCode)
{
    CWProtocolMessage *msgElems = NULL;
    const int msgElemCount = 1;

    if (!(CWAssembleMsgElemResultCode(msgElems,resultCode))) {
	CW_FREE_OBJECT(msgElems);
	return CW_FALSE;
    }

    if(!(CWAssembleMessage(messagesPtr,
		    fragmentsNumPtr,
		    PMTU,
		    seqNum,
		    CW_MSG_TYPE_VALUE_URCP_RESPONSE,
		    msgElems,
		    msgElemCount,
		    NULL,
		    0,
#ifdef CW_NO_DTLS
		    CW_PACKET_PLAIN
#else
		    CW_PACKET_CRYPT
#endif
		    ))){
	return CW_FALSE;
    }

    return CW_TRUE;
}
static void sendUrcpMessage(int wtpIdx, CWProtocolMessage **completeMsgPtr, CWProtocolMessage *msgElems, const int msgElemNum)
{
    int seqNum, i;
    int fragmentsNum = 0;

    seqNum = CWGetSeqNum();

    if(CWAssembleUrcpMessage(completeMsgPtr,
		&fragmentsNum,
		gWTPs[wtpIdx].pathMTU,
		seqNum,
		CW_MSG_TYPE_VALUE_URCP_REQUEST,
		msgElems,
		msgElemNum,
		NULL,
		0,
#ifdef CW_NO_DTLS
		CW_PACKET_PLAIN
#else
		CW_PACKET_CRYPT
#endif
		))
    {
	for (i = 0; i < fragmentsNum; i++) 
	{
	    CWThreadMutexLock(&gWTPs[wtpIdx].urcpMutex);
#ifdef CW_NO_DTLS
	    if (!CWNetworkSendUnsafeUnconnected(gWTPs[wtpIdx].socket, &gWTPs[wtpIdx].address, (*completeMsgPtr)[i].msg, (*completeMsgPtr)[i].offset))
#else
            if (!CWSecuritySend(gWTPs[wtpIdx].session, (*completeMsgPtr)[i].msg, (*completeMsgPtr)[i].offset))
#endif
            {
		    CWDebugLog("Failure sending URCP PDU");
		    CWThreadMutexUnlock(&gWTPs[wtpIdx].urcpMutex);
		    break;
            }
	    else {
	        CWThreadMutexUnlock(&gWTPs[wtpIdx].urcpMutex);
	    }
	}
    }

    for (i = 0; i < fragmentsNum; i++)
    {
	CW_FREE_PROTOCOL_MESSAGE((*completeMsgPtr)[i]);
    }
    CW_FREE_OBJECT(*completeMsgPtr);
}
