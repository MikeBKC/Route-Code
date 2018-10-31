#ifdef LDRA
#define DBGPRINT printf
#define syslog printf
extern unsigned int ntohl(unsigned int netlong);
extern unsigned int htonl(unsigned int hostlong);
extern int inet_aton(const char *cp, struct in_addr *inp);
#define UTT_MAX_PPPOE_USER                 50
#define GLOBAL_CHAIN            "global"
#define GROUP_CHAIN             "group"
extern char *nvram_bufget(int index, char *name);
extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern char *inet_ntoa(struct in_addr in);
extern long int strtol(const char *nptr, char **endptr, int base);
extern long long int strtoll(const char *nptr, char **endptr, int base);
extern int doSystem(char_t *fmt, ...);
extern int getWanDhcpDefaultRoute(int wannum, char* ipaddr);
extern char* getWanIfNamePPP(int num);
extern char* int2ip(unsigned int lip,char * ip);
extern unsigned int ip2int(char *ip);
extern char * NextRecordStart(char*Record);
extern unsigned int sleep(unsigned int seconds);
extern int getIfIp(char *ifname, char *if_addr);
void wanUpDelay(int wanNo);
void initUpnpServer();
int  init_DDNS(void);
void ntp_sh(void);
void init_dnsmasq(int wanNo);
void iptablesPortForwardRun(void);
void init_NatRule(int wanNo, char action);
 void iptablesDMZRun(void);
 void iptablesAllNATRun(int wanNo, char action);
int _getWanLinkState(int wanNo);
int HaveOtherMainAlive(int wanNo);
int isWanInUse(int wanNo);
char * add_wanx(const char *name, int wanNo, char * nvram_name);
int headbits(unsigned int nm);
void MpCombChange(int wanStatuses[]);
int WanLinkDown(int wanNo);
void ChangeBackLines(int action);
 void ISPRouteTableAdd(int wanIndex);
 void ISPRouteTableFlush(int wanIndex);
 void addOneWanDefaultRoute(int wanIndex);
 void delOneWanDefaultRoute(int wanIndex);
 void ModifyDefaultRoute(int wanNo, int action);
 void FlushUserRoute(int wanNo);
 void AddUserRoute(int wanNo);
void EditStaticRoute(char *RouteRec, int flag);
void initUpnpServerC(int wanIndex, int action);
void add_dns_route(int wanNo);
void delete_dns_route(int wanNo);
void wanDownDelay(int wanNo);
void iptablesSecDefenseRun(void);
void initUpnpServerS();
 int init_dhcpserver(void);
void initPppoeServer();
 void _modifyLanIpRuleAndRoute(int action, unsigned int delIp);
int initWan(int wanIndex);
int initLan(void);
void add_dns_route(int wanNo);
void delete_dns_route(int wanNo);
 char* read_file_line(char *filename, char *buf, int line);
void staticRoutingInit(void);
void RoutingInit(void);
int initInternet(void);
int get_netlink_status(int wannum);
void restart8021XDaemon(int nvram);
 void initWlanSec(void);
int flash_write(char *buf, off_t to, size_t len);
extern void getDays(unsigned char day_bit,char days[]);
extern void getDays(unsigned char day_bit,char days[]);
extern void init_GlobalBase(void);
extern void initGroup(void);
extern int EditOneIptablesGroup(char *GroupList,char action ,char *lan_if_name, char *wan_if_name ,int mode);
extern char *strtok_r(char *str, const char *delim, char **saveptr);
 void div8(char * src, int division);
#endif
