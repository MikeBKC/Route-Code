#define PDB_IND_CHAIN	"utt_indentify_chain"

#if (EXCEPT_QQ == FYES) || (EXCEPT_MSN == FYES) || (EXCEPT_ALI == FYES)
#define IM_EXCEPT_CHAIN "im_except_chain"/*IM°×Ãûµ¥Á´*/
#endif

#if (USER_STATUS == FYES)
#define PDB_STATS_CHAIN	"utt_stats_chain"
#endif

#if (GROUP_BASE == FYES)
#define PDB_POLICY_CHAIN    "utt_policy_chain"
#endif

#define PDB_NF_TABLE   "mangle"
#define PDB_NF_CHAIN	"FORWARD"

#if (USER_STATUS == FYES)
#define UTT_STATS_CLEAR_TIME  "10 00 * * *" 
#endif

#if (GROUP_BASE == FYES)
extern void groupbaseInit(void);
#endif

#ifdef CONFIG_UTT_NETFILTER
extern void clearPdbChain(char *name) ;
#endif
