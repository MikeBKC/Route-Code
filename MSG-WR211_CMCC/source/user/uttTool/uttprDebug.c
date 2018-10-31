/* 清除用户统计信息 */
#include	<string.h>
#include	"uttMachine.h"
#include	"mib.h"
#include	"profacce.h"
#include	"argcmd.h"
#include	<sys/sysinfo.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>

int uttNfNlSock;

#define UTT_DEBUG_NAME "prdebug"
static void uttdebugUse()
{
    printf("%s help: \n", UTT_DEBUG_NAME);
    printf("	%s %s -%s\n",UTT_DEBUG_NAME,"policyroute","policy route all kernel configure list");
}

int main(int argc, char **argv) {
    uttNfNlSock = uttNfNlSkBind(getpid(), 0);/*单播到本进程*/

#if (DEBUG_POLICY_ROUTE == 1)
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    if((argc >= 2) && (strcmp(argv[1], "policyroute") == 0)) {
	UttNlNfPolicyRouteConf nlMsg;
	memset(&nlMsg, 0, sizeof(nlMsg));

	if(uttNfNlSock < 0)
	{
	    return -1;
	}

	nlMsg.msgHead.nlType = UTT_NLNF_POLICY_ROUTE;
	nlMsg.debugType = UTT_DEBUG_LIST_ALL;
	uttNfNlSend(uttNfNlSock,(char*)&nlMsg, sizeof(nlMsg), 0 ,0 );/*发送消息给内核*/
    }else {
	uttdebugUse();
    }
#endif
#endif
#endif
    return 0;
}
