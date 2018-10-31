#include "CWWTP.h"
#include <ctype.h>
#include <sys/un.h>

#define CAPWAP_SOCKET  "/tmp/capwap_socket"
#define URCP_SOCKET    "/tmp/urcp_socket"
#define PACKET_SIZE	65536
//#define URCP_DEBUG
static int gUrcpSock=-1;
static struct sockaddr_un urcpAddr; 
static int capwapSock=-1;
static char buffer[PACKET_SIZE + 1];
#ifdef URCP_DEBUG
int frame_dump(char *buf, int len)
{
    int i;

    if (len>64) len=64;
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
/*
 * Steven's readn().
 */
int readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = recv(fd, ptr, nleft, 0)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
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
CW_THREAD_RETURN_TYPE CWWTPUrcp(void *arg)
{
    int seqNum, rlen, len, i;
    int fragmentsNum = 0;
    const int msgElemCount = 1;
    struct sockaddr_un capwapAddr; 
//    struct sockaddr_un urcpAddr; 
    CWProtocolMessage *completeMsgPtr = NULL;
    CWProtocolMessage *msgElems = NULL;

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

#if 1
    listen (capwapSock, 8);
    len = sizeof(urcpAddr);
    gUrcpSock = accept(capwapSock, (struct sockaddr *)&urcpAddr, (socklen_t *)&len);
    if(gUrcpSock == -1)
    {
	cprintf("accept error\n");
	exit(1);
    }
#endif
    /*      Receive data */
    CW_REPEAT_FOREVER 
    {
	CW_ZERO_MEMORY(buffer, sizeof(buffer));
#ifdef CAPWAP_UDP
	rlen = recvfrom(capwapSock, buffer,PACKET_SIZE, 0, (struct sockaddr *)&urcpAddr,(socklen_t *)&len);
#else
	//rlen = read(gUrcpSock, buffer, PACKET_SIZE);
	rlen = getPacket(gUrcpSock, buffer, PACKET_SIZE);
#endif
	if(rlen<=0)
	{
	    CWDebugLog("THR URCP: Error receiving from unix socket. errno=%d", errno);
#if 0
	    if(errno == 2) {/*可能urcp断开了*/
		gUrcpSock = accept(capwapSock, (struct sockaddr *)&urcpAddr, (socklen_t *)&len);
	    }
#endif
	    continue;
	}
#ifdef URCP_DEBUG
	cprintf("recv from urcp len:%d\n", rlen);
	frame_dump(buffer, rlen);
#endif
	completeMsgPtr = NULL;
	CW_CREATE_PROTOCOL_MSG_ARRAY_ERR(msgElems, msgElemCount, continue;);
	CW_CREATE_PROTOCOL_MESSAGE(*msgElems, 4+rlen, {free(msgElems);continue;});
	CWProtocolStore16(msgElems, CW_MSG_ELEMENT_URCP_PDU_CW_TYPE);
	CWProtocolStore16(msgElems, rlen);
	CWProtocolStoreRawBytes(msgElems, buffer, rlen);

	seqNum = CWGetSeqNum();

	if(CWAssembleMessage(&completeMsgPtr,
		    &fragmentsNum,
		    gWTPPathMTU,
		    seqNum,
		    CW_MSG_TYPE_VALUE_URCP_REQUEST,
		    msgElems,
		    msgElemCount,
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
                CWThreadMutexLock(&urcpMutex);
#ifdef CW_NO_DTLS
		if (!CWNetworkSendUnsafeConnected(gWTPSocket, completeMsgPtr[i].msg, completeMsgPtr[i].offset))
#else
		if (!CWSecuritySend(gWTPSessiong, completeMsgPtr[i].msg, completeMsgPtr[i].offset))
#endif
		{
                    CWDebugLog("Failure sending URCP PDU");
		    CWThreadMutexUnlock(&urcpMutex);
                    break;
		}
		CWThreadMutexUnlock(&urcpMutex);
	    }
	}

	for (i = 0; i < fragmentsNum; i++)
	{
	    CW_FREE_PROTOCOL_MESSAGE(completeMsgPtr[i]);
	}
	CW_FREE_OBJECT(completeMsgPtr);
    }

    close(capwapSock);
    return(NULL);
}

void CWsendtoUrcp(unsigned char *msg, int len)
{
#ifdef CAPWAP_UDP
    if(capwapSock != -1)
#else
    if(gUrcpSock != -1)
#endif
    {
#ifdef URCP_DEBUG
	cprintf("send to urcp\n");
#endif
	send_len(gUrcpSock, len);
	if(send(gUrcpSock, msg, len, 0)<0) {
	    CWDebugLog("Failure sending PDU to URCP: errno=%d", errno);
	}
    }
}

CWBool CWWTPParseUrcpPDURequest(char *buff, int len)
{
    short type, elemLen;

    type = *((short*)buff);
    type = ntohs(type);

    elemLen = *((short*)(buff+2));
    elemLen = ntohs(elemLen);
    if((type==CW_MSG_ELEMENT_URCP_PDU_CW_TYPE) &&  (elemLen == (len-4))) {
	CWsendtoUrcp(buff+4, elemLen);
    }
    return CW_TRUE;
}

CWBool CWWTPAssembleUrcpPDUResponse(CWProtocolMessage **messagesPtr,
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

