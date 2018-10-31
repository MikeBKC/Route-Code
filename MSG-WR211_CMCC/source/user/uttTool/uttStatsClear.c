/* 清除用户统计信息 */
#include	<string.h>
#include	"uttMachine.h"
#include	"mib.h"
#include	"profacce.h"
#include	"argcmd.h"
#include	<sys/sysinfo.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>

int uttNfNlSock;

int main(int argc, char **argv) {
    UttNlNfStatsMsg nlMsg;
    memset(&nlMsg, 0, sizeof(nlMsg));

    uttNfNlSock = uttNfNlSkBind(getpid(), 0);/*单播到本进程*/
    if(uttNfNlSock < 0)
    {
	return -1;
    }

    nlMsg.msgHead.nlType = UTT_NLNF_STATS;
    nlMsg.msgType = UTT_STATS_CLEAR;
    uttNfNlSend(uttNfNlSock,(char*)&nlMsg, sizeof(nlMsg), 0 ,0 );/*发送消息给内核*/

    return 0;
}
