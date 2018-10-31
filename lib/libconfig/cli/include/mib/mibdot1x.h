/*
 * Standard Header
 */

#ifndef MIBDOT1X_H_
#define MIBDOT1X_H_

//#include "lanprof.h"

/* --------------------------------------------------------------- */
/* constants definitions */
#define MAX_DOT1X_USER_PROFILES	600
#define DOT1X_MAX_USER_NAME_LEN	31

#ifdef MAX_PORT2_PROFILES
#define LAN_PORT_NUM		MAX_PORT2_PROFILES	//5
#define ETH_SLOT_NUM		NUM_OF_ETHER_SLOTS
#else
#define	LAN_PORT_NUM		5
#define ETH_SLOT_NUM		5
#endif
#define PHY_PORT_NUM		(LAN_PORT_NUM + ETH_SLOT_NUM)

/* --------------------------------------------------------------- */
/* types definitions */
/*
 * 	--  -----------------------------------------------------------------  --
 * 	--  IEEE 802.1X MIB
 *	--  -----------------------------------------------------------------  --
 */

typedef struct paeMIBModule {
	const char * last_updated;
	const char * orgnization;
	const char * contact_info;
	const char * revision;
	const char * description;	
}PaeMibModule,*pPaeMibModule;

/*
PaeMibModule IEEE8021PaeMIB = 
{
	"200406220000z", //June 22nd,2004
	"IEEE 802.1 Working Group",
	"http://grouper.ieee.org/groups/802/1/index.html",
	"The port access entity module for managing ieee 802.1X.",
	"200406220000z", //June 22nd 2004
};
*/
/*
 * 	--  -----------------------------------------------------------------  --
 * 	--  Textual Conventions
 *	--  -----------------------------------------------------------------  --
 */

/* The control mode values of the authenticator PAE */ 
typedef enum PaeControlledDirections {
	CTL_DIRECTION_BOTH = 0,
	CTL_DIRECTION_IN
}PaeControlledDirections;

/* The status values of the authenticator PAE controlled port */
typedef enum PaeControlledPortStatus {
	STATUS_UNAUTHORIZED = 0,
	STATUS_AUTHORIZED = 1,
	STATUS_UNDEFINED = 2,
}PaeControlledPortStatus;

typedef enum portControlMethod
{
	METHOD_PORT_BASED,
	METHOD_MAC_BASED,
}PortControlMethod;

/* The control values of the authenticator PAE controlled port 
 */
typedef enum PaeControlledPortControl {
	CTL_FORCE_UNAUTHORIZED,
	CTL_AUTO,
	CTL_FORCE_AUTHORIZED,
}PaeControlledPortControl;

typedef enum Dot1xPaePortCapability {
	PORT_AUTH_CAPABLE = 0,
	PORT_SUPP_CAPABLE = 1
};
typedef enum CtrlDirState{ 
	CTRL_DIR_FORCE_BOTH,
	CTRL_DIR_IN_OR_BOTH 
}CtrlDirState ;

typedef enum keyRxState{
	KEY_RX_NO_KEY_RECEIVE,
	KEY_RX_KEY_RECEIVE 
}KeyRxState; 

/*
 *	-------------------------------------------------------------------------------------
 */

/*
 *  --  ------------------------------------------------------------------------------  --
 *  --  groups in  The PAE MIB
 *	--  ------------------------------------------------------------------------------  --
 */

//	dot1xPaeSystem
//	dot1xPaeAuthenticator
//	dot1xPaeSupplicant

/*
 *	--  -----------------------------------------------------------------------------  --
 *	--  The PAE System Group
 *  --	-----------------------------------------------------------------------------  --
 */

/* The administrative enable\disable state for port access control
 * in a system
 */
typedef enum dot1xPaeSystemAuthControl {
	AUTH_CONTROL_DISABLED = 0,
	AUTH_CONTROL_ENABLED = 1,
}Dot1xPaeSystemAuthControl;


typedef enum dot1xReauthTimerSate{ 
	REAUTH_TIMER_INITIALIZE = 1,
	REAUTH_TIMER_REAUTHENTICATE
}Dot1xReauthTimerSate;


typedef struct dot1xPaePortEntry {
	int dot1xPaePortNumber;
	int dot1xPaePortProtocolVersion;
	/*	dot1xPaePortCapabilities
	 *	Bits{
	 * 			dot1xPaePortAuthCapable(0)
	 * 			dot1xPaePortSuppCapable(1)
	 */
	Boolean dot1xPaePortAuthCapable;
	Boolean dot1xPaePortSuppCapable;
	//int dot1xPaePortCapabilities;
	Boolean dot1xPaePortInitialize;
	Boolean dot1xPaePortReauthenticate;	
}Dot1xPaePortEntry,*pDot1xPaePortEntry;

/*
 *  --  -----------------------------------------------------------------------------  --
 *  --  The PAE Authenticator Group
 *  --  -----------------------------------------------------------------------------  --
 *  --  The Authenticator Configuration Table
 *  --  -----------------------------------------------------------------------------  --
 */

typedef enum dot1xAuthPaeState {     //read only
	AUTH_PAE_INITIALIZE,
	AUTH_PAE_DISCONNECTED,
	AUTH_PAE_CONNECTING,
	AUTH_PAE_AUTHENTICATING,
	AUTH_PAE_AUTHENTICATED,
	AUTH_PAE_ABORTING ,
	AUTH_PAE_HELD,
	AUTH_PAE_FORCE_AUTH,
	AUTH_PAE_FORCE_UNAUTH,
	AUTH_PAE_RESTART,
}Dot1xAuthPaeState;

typedef enum dot1xAuthBackendAuthState {	//read only
	BE_AUTH_REQUEST,
	BE_AUTH_RESPONSE,
	BE_AUTH_SUCCESS,
	BE_AUTH_FAIL, 
	BE_AUTH_TIMEOUT, 
	BE_AUTH_IDLE, 
	BE_AUTH_INITIALIZE,
	BE_AUTH_IGNORE
}Dot1xAuthBackendAuthState;

typedef struct dot1xAuthConfigEntry {	//not accessible
	Dot1xAuthPaeState authPaeState;	//RO
	Dot1xAuthBackendAuthState BackendAuthState;	//RO
	PaeControlledDirections authAdminControlledDirections;	//RW
	PaeControlledDirections authOperControlledDirections;	//RO
	PaeControlledPortStatus AuthPortStatus;	//RO
	PaeControlledPortControl PortControl;	//RW
	PortControlMethod ControlMethod;
	unsigned int MaxHostNum;
	Boolean AuthMultiHost;
	Uint32	QuietPeriod;	//RW
	Uint32 TxPeriod;			//deprecated
	Uint32 SuppTimeout;		//deprecated
	Uint32	ServerTimeout;	//RW
	Uint32	MaxReq;		//deprecated
	Uint32	ReAuthPeriod;	//RW
	Boolean	ReAuthEnabled;	//RW
	Boolean	AuthKeyTxEnabled;	//RW
}Dot1xAuthConfigEntry,*pDot1xAuthConfigEntry;

/*	
 *	--  -----------------------------------------------------------------  --
 * 	--  The Authenticator Statistics Table
 * 	--  -----------------------------------------------------------------  --
 */

typedef struct dot1xAuthStatsEntry {
	Uint32 authEapolFramesRx;
	Uint32 authEapolFramesTx;
	Uint32 authEapolStartFramesRx;
	Uint32 authEapolLogoffFramesRx;
	Uint32 authEapolRespIdFramesRx;
	Uint32 authEapolRespFramesRx;
	Uint32 authEapolReqIdFramesTx;
	Uint32 authEapolReqFramesTx;
	Uint32 authInvalidEapolFramesRx;
	Uint32 authEapLengthErrorFramesRx;
	Uint32 authLastEapolFrameVersion;
	BYTE authLastEapolFrameSource[6];	
}Dot1xAuthStatsEntry,*pDot1xAuthStatsEntry;

/*	
 *	--  -----------------------------------------------------------------  --
 * 	--  The Authenticator Diagnostics Table
 * 	--  -----------------------------------------------------------------  --
 */

typedef struct dot1xAuthDiagEntry {		//deprecated
	Uint32 authEntersConnecting;	//deprecated
	Uint32 authEapLogoffsWhileConnecting;	//deprecatedd	
	Uint32 authEntersAuthenticating;	//deprecated
	Uint32 authAuthSuccessWhileAuthenticating;	//deprecated
	Uint32 authAuthTimeoutsWhileAuthenticating;	//deprecated
	Uint32 authAuthFailWhileAuthenticating;	//deprecated
	Uint32 authAuthReauthsWhileAuthenticating;	//deprecated
	Uint32 authAuthEapStartsWhileAuthenticating;	//deprecated
	Uint32 authAuthEapLogoffWhileAuthenticating;	//deprecated
	Uint32 authAuthReauthsWhileAuthenticated;	//deprecated
	Uint32 authAuthEapStartsWhileAuthenticated;	//deprecated
	Uint32 authAuthEapLogoffWhileAuthenticated;	//deprecated
	Uint32 authBackendResponses;		//deprecated
	Uint32 authBackendAccessChallenges;		//deprecated
	Uint32 authBackendOtherRequestsToSupplicant;	//deprecated
	Uint32 authBackendNonNakResponsesFromSupplicant;	//deprecated
	Uint32 authBackendAuthSuccesses;	//deprecated
	Uint32 authBackendAuthFails;	//deprecated
}Dot1xAuthDiagEntry,*pDot1xAuthDiagEntry;

/*	
 *	--  -----------------------------------------------------------------  --
 * 	--  The Authenticator Session Statistics Table
 * 	--  -----------------------------------------------------------------  --
 */

typedef enum dot1xAuthSessionAuthenticMethod {
	LOCAL_AUTH_SERVER = 0,
	REMOTE_AUTH_SERVER = 1,
}Dot1xAuthSessionAuthenticMethod;

typedef enum dot1xAuthSessionTerminateCause {
	CAUSE_SUPPLICANT_LOGOFF,
	CAUSE_PORT_FAILURE,
	CAUSE_SUPPLICANT_RESTART,
	CAUSE_REAUTH_FAILED,
	CAUSE_AUTH_CONTROL_FORCE_UNAUTH,
	CAUSE_PORT_REINIT,
	CAUSE_PORT_ADMIN_DISABLED,
	CAUSE_NOT_TERMINATE_YET = 999
}Dot1xAuthSessionTerminateCause;

typedef struct dot1xAuthSessionStatsEntry {
	Uint32 session_id;
	Uint32 port_id;
	Uint16 index;
	BYTE* mac_addr;
	char user_name[DOT1X_MAX_USER_NAME_LEN + 1];
	Uint32 session_time;	
	Uint32 session_start;
	PaeControlledPortStatus auth_status;
	Dot1xAuthSessionTerminateCause terminate_cause;
	Dot1xAuthSessionAuthenticMethod authentic_method;
	struct dot1xAuthSessionStatsEntry * next;
}Dot1xAuthSessionStats,*pDot1xAuthSessionStats;

/*
 *  --  -----------------------------------------------------------------------------  --
 *  --  The PAE Supplicant Group
 *  --  -----------------------------------------------------------------------------  --
 *  --  The Supplicant Configuration Table
 *  --  -----------------------------------------------------------------------------  --
 */

typedef enum dot1xSuppPaeState {
	SUPP_PAE_DISCONNECTED,
	SUPP_PAE_LOGOFF,
	SUPP_PAE_CONNECTING,
	SUPP_PAE_AUTHENTICATING,
	SUPP_PAE_AUTHENTICATED,
	SUPP_PAE_ACQUIRED,
	SUPP_PAE_HELD,
	SUPP_PAE_RESTART,
	SUPP_PAE_sFORCE_AUTH,
	SUPP_PAE_sFORCE_UNAUTH
}Dot1xSuppPaeState;

typedef enum dot1xSuppAccessCtrlWithAuth {
	SUPP_CTRL_AUTH_INACTIVE,
	SUPP_CTRL_AUTH_ACTIVE
}Dot1xSuppAccessCtrlWithAuth;

typedef enum dot1xSuppBackendState {
	BE_SUPP_INITIALIZE,
	BE_SUPP_IDLE,
	BE_SUPP_REQUEST,
	BE_SUPP_RESPONSE,
	BE_SUPP_RECEIVE,
	BE_SUPP_FAIL,
	BE_SUPP_SUCCESS,
	BE_SUPP_TIMEOUT
}Dot1xSuppBackendState;

typedef struct dot1xSuppConfigEntry {
	Dot1xSuppPaeState SuppPaeState;
	Uint32 SuppHeldPeriod;
	Uint32 SuppAuthPeriod;
	Uint32 SuppStartPeriod;
	Uint32 SuppMaxStart;
	PaeControlledPortStatus SuppControlledPortsStatus;
	PaeControlledPortControl SuppPortControl;
	Dot1xSuppAccessCtrlWithAuth SuppAccessCtrlWithAuth;
	Dot1xSuppBackendState SuppBackendState;
	Uint32 session_time;	
	Uint32 session_start;
	BYTE	user_name[32];
	int user_name_len;
	BYTE  password[32];
	int password_len;
}Dot1xSuppConfigEntry,*pDot1xSuppConfigEntry;

/*
 *  --  -----------------------------------------------------------------------------  --
 *  --  The Supplicant Statistics Table
 *  --  -----------------------------------------------------------------------------  --
 */
typedef struct dot1xSuppStatsEntry {
	Uint32 suppEapolFramesRx;
	Uint32 suppEapolFramesTx;
	Uint32 suppEapolStartFramesTx;
	Uint32 suppEapolLogoffFramesTx;
	Uint32 suppEapolRespIdFramesTx;	//deprecated
	Uint32 suppEapolRespFramesTx;		//deprecated
	Uint32 suppEapolReqIdFramesRx;		//deprecated
	Uint32 suppEapolReqFramesRx;		//deprecated
	Uint32 suppInvalidEapolFramesRx;
	Uint32 suppEapLengthErrorFramesRx;
	Uint32 suppLastEapolFrameVersion;
	BYTE suppLastEapolFrameSource[6];	//MacAddress
}Dot1xSuppStatsEntry,*pDot1xSuppStatsEntry;

/*
 *  --  -----------------------------------------------------------------------------  --
 *  --  IEEE 802.1X MIB - Conformance Information
 *  --  -----------------------------------------------------------------------------  --
 */


/*
 * Constants Definition
 */

typedef enum eapServerType
{
	SERVER_TYPE_LOCAL = 0,
	SERVER_TYPE_RADIUS = 1,
}EapServerType;

typedef enum dot1xAuthScheme
{
	SCHEME_LOCAL = 0,
	SCHEME_RADIUS = 1,
	SCHEME_RADIUS_LOCAL = 2,
}Dot1xAuthScheme;

typedef enum switchMode 
{
	SWITCH_MODE_MANAGED = 0,
	SWITCH_MODE_UNMANAGED = 1,
}SwitchMode;

/* --------------------------------------------------------------- */
/* struct definitions */
typedef struct dot1xUserProfile
{
    char                                name[ MAX_HOST_NAME_LENGTH + 1 ];
    Boolean                             active;
    char                                description[MAX_PROFILE_NAME_LENGTH + 1];
    char                                password[ MAX_PASSWORD_LENGTH + 1 ];
    Uint8								macAddr[ETHER_ADDR_LEN];
	Boolean				dot1x_acct_enable;
	Uint32				dot1x_max_user;
	Uint32				dot1x_port;	
}Dot1xUserProf,*pDot1xUserProf;

typedef struct radiusServer
{
	int retry_interval;	//in seconds
	int timeout;
	int deadtime;
	int retransmit;
	Boolean available;
	Uint32 source_ip;
	Uint32 host_ip;
	Uint16	auth_port;
	BYTE share_key[32];
	unsigned int share_key_len;
}RadiusServer,*pRadiusServer;

typedef struct dot1xHostConfig
{
	char name[MAX_PROFILE_NAME_LENGTH+1];
	Boolean active;
	Boolean dhcp_launch;
	Boolean pppoe_launch;
	Boolean arp_launch;
	Dot1xPaeSystemAuthControl auth_control;
	Dot1xAuthScheme auth_scheme;	
	int eapol_version;
	RadiusServer primary_radius;
	RadiusServer secondary_radius;	
}Dot1xHostConfig,*pDot1xHostConfig,Dot1xProf,*pDot1xProf;

typedef struct dot1xPortConfig
{
	Dot1xPaePortEntry	 pae_port;
	Dot1xAuthConfigEntry auth_config;
	Dot1xSuppConfigEntry supp_config;	
}Dot1xPortProf,*pDot1xPortProf;

typedef struct dot1xPortMib
{
	unsigned char mac_addr[6];
	pDot1xPortProf config;
	Dot1xAuthStatsEntry  auth_stats;
	Dot1xSuppStatsEntry  supp_stats;
	unsigned int station_num;
	Boolean use_group_addr_as_dst;
}Dot1xPortMib,*pDot1xPortMib;

#endif /*MIBDOT1X_H_*/
