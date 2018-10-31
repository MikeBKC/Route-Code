#include "uttMachine.h"
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"

#if (GAME_BOOST == FYES) || (WIFIDOG_AUTH == FYES) /*wifidog微信策略更新*/
extern char* parsePDB(char* pdb,const char* key){
	const char *split = ";";
	char *p,*index,*result=NULL;
	p = strtok(pdb,split);
	while(p!=NULL){
		index = strstr(p,key);
		if(index!=NULL){
			result = index+strlen(key)+1;
			//break;
		}
		p = strtok(NULL,split);
	}
	return result;
}

extern char* buildRule(char *pdb,char * result){
	char rule[256],target[256],*par;
	memset(rule,0,256);
	strcpy(target,pdb);
	par = parsePDB(target,"protocol");
	if(par!=NULL){
		sprintf(rule+strlen(rule)," -p %s ",par);
		par==NULL;
	}
	strcpy(target,pdb);
	par = parsePDB(target,"sport");
	if(par!=NULL){
		sprintf(rule+strlen(rule)," --sport %s ",par);
	}
	strcpy(target,pdb);
	par=parsePDB(target,"dport");
	if(par!=NULL){
		sprintf(rule+strlen(rule)," --dport %s ",par);
	}
	strcpy(target,pdb);
	par = parsePDB(target,"name");
	if(par!=NULL){
		sprintf(rule+strlen(rule)," -m layer7 --l7proto %s ",par);
	}
	strcpy(result,rule);
	return result;
}
#endif

#if (GAME_BOOST == FYES)
extern void gameBoostRuleclear(void){
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    int max, min, i,j;
	char rule[256];
	memset(rule,0,256);
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
		prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
		if(prof != NULL && ProfInstIsFree(prof) == 0) {
			if(prof->type == SpeedupPdb){
				for(j=0;j<MAX_PDB_LINE_COUNT;j++)
				{
					if(strlen(prof->line[j])!=0)
					{
						buildRule(prof->line[j],rule);
						doSystem("iptables -t mangle -D PREROUTING %s -j CONNMARK --set-mark 0x0100/0x0f00",rule);
					}
				}
			}
		}
    }
}


/* 初始化函数 */
extern void gameBoostRuleInit(void) 
{
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    int max, min, i,j;
	char rule[256];
	memset(rule,0,256);
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
		prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);

		if(prof != NULL && ProfInstIsFree(prof) == 0) {
			if(prof->type == SpeedupPdb&&prof->head.active==FUN_ENABLE){
				for(j=0;j<MAX_PDB_LINE_COUNT;j++)
				{
					if(strlen(prof->line[j])!=0)
					{
						buildRule(prof->line[j],rule);
						doSystem("iptables -t mangle -A PREROUTING %s -j CONNMARK --set-mark 0x0100/0x0f00",rule);
					}
				}
			}
		}
    }

}

void gameBoostRuleAdd(int index){
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, index);
	char rule[256];
	int j;
	memset(rule,0,256);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
		if(prof->type == SpeedupPdb&&prof->head.active==FUN_ENABLE){
			for(j=0;j<MAX_PDB_LINE_COUNT;j++)
			{
				if(strlen(prof->line[j])!=0)
				{
					buildRule(prof->line[j],rule);
					doSystem("iptables -t mangle -A PREROUTING %s -j CONNMARK --set-mark 0x0100/0x0f00",rule);
				}
			}
		}
	}
}

void gameBoostRuleDel(int index){
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, index);
	char rule[256];
	int j;
	memset(rule,0,256);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
		if(prof->type == SpeedupPdb){
			for(j=0;j<MAX_PDB_LINE_COUNT;j++)
			{
				if(strlen(prof->line[j])!=0)
				{
					buildRule(prof->line[j],rule);
					doSystem("iptables -t mangle -D PREROUTING %s -j CONNMARK --set-mark 0x0100/0x0f00",rule);
				}
			}
		}
	}
}
extern void updateGameBoostRule(int index){
	gameBoostRuleDel(index);
	gameBoostRuleAdd(index); 
}

extern void gameboostInit(void){

	MibProfileType mibType = MIB_PROF_INTERFACE;
	InterfaceProfile *prof = NULL;
	prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
	if(prof!=NULL&&prof->smartQosEn==FUN_ENABLE){
		gameBoostRuleInit();
	}
}
#endif
