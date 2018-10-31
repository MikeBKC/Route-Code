/* pptp_callmgr.h ... Call manager for PPTP connections.
 *                    Handles TCP port 1723 protocol.
 *                    C. Scott Ananian <cananian@alumni.princeton.edu>
 *
 * $Id: pptp_callmgr.h,v 1.1.1.1 2011/08/17 11:10:41 brwang Exp $
 */

#define PPTP_SOCKET_PREFIX "/var/run/pptp/"

int callmgr_main(int argc, char**argv, char**envp);
void callmgr_name_unixsock(struct sockaddr_un *where,
			   struct in_addr inetaddr,
			   struct in_addr localbind);
