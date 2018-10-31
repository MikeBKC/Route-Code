#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

#if (HTTP_CACHE == FYES)
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

/********************************************************************
 * 函数：isCacheRun
 * 功能：查看traffic server是否运行
 * 参数：略
 * 输出：无
 * 返回：整型 1：启动；0:未启动；－1：查询失败
 * 作者：bchen
 * 日期：2012-08-21
 *
 ********************************************************************/

int isCacheRun() {
	FILE *fp;
	char buf[256];
	char *cmd = "cache-stats.sh";
	if ((fp = popen("ps -ef|grep traffic_server|grep -v grep|wc -l", "r")) == NULL) {
                return -1;
     }

     if (fgets(buf, 256, fp) != NULL) {
		int stats = 0;
		sscanf(buf, "%d", &stats);
		pclose(fp);
		if (!stats) {
			return 0;
		} else {
			return 1;
        }

     } else {
        pclose(fp);
        return -1;
     }
}



/********************************************************************
 * 函数：initCacheConfig
 * 功能：修改traffic server配置文件
 * 参数：略
 * 输出：无
 * 返回：无
 * 作者：bchen
 * 日期：2013-01-05
 *
 ********************************************************************/

void initCacheConfig(void)
{

	MibProfileType mibType = MIB_PROF_CACHE;
	CacheProfile *prof = NULL;
	
	prof = (CacheProfile *)ProfGetInstPointByIndex(mibType,0);
	if(prof != NULL)
	{
		doSystem("echo \"#cacheurl\" > /usr/lib/cacheurl.config");
		if(prof->youkuVideoEnable)
		{		
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/youku\\/.*\\/(.*\\.(flv|f4v|mp4))$ http://video.youku.com/youku/$1' >> /usr/lib/cacheurl.config");
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/youku\\/.*\\/(.*\\.(flv|f4v|mp4))\\?start http://video.youku.com/youku/$1' >> /usr/lib/cacheurl.config");
		}

		if(prof->tudouVideoEnable)
		{		
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}(.*\\.f4v)\\?.+id=tudou*  http://video.tudou.com$1' >> /usr/lib/cacheurl.config");
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/youku\\/(.*\\.mp4) http://video.youku.com/$1' >> /usr/lib/cacheurl.config");
		}
		if(prof->tcVideoEnable)
		{		
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/[0-9]*\\/.*qqvideo.tc.qq.com\\/(.*)\\?.* http://video.tc.qq.com/$1' >> /usr/lib/cacheurl.config");
		}
		if(prof->sinaVideoEnable)
		{		
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/[0-9]*\\/.*edge.v.iask.com\\/(.*)\\?.* http://video.sina.com/$1' >> /usr/lib/cacheurl.config");
		}
		if(prof->ifengVideoEnable)
		{
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/video06\\/([0-9]*\\/[0-9]*\\/[0-9]*\\/.*\\.mp4) http://video.ifeng.com/$1' >> /usr/lib/cacheurl.config");
		}
		if(prof->letvVideoEnable)
		{
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/([0-9]{1,2}\\/[0-9]{1,2}\\/[0-9]{1,2}\\/letv-uts\\/.*\\.letv)\\?crypt.*(&rstart=[0-9]*&rend=[0-9]*) http://video.letv.com/$1$2' >> /usr/lib/cacheurl.config");
		}
		if(prof->v56VideoEnable)
		{
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/.*\\.56\\.com\\/flvdownload\\/([0-9]*\\/[0-9]*\\/.*\\.flv)\\?.*m=s$ http://video.56.com/$1' >> /usr/lib/cacheurl.config");
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/.*\\.56\\.com\\/flvdownload\\/([0-9]*\\/[0-9]*\\/.*\\.flv)\\?.*(&start=[0-9]*&end=[0-9]*) http://video.56.com/$1$2' >> /usr/lib/cacheurl.config");
		}
		if(prof->funshionVideoEnable)
		{
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/play\\/(.*\\.mp4$) http://video.funshion.com/$1' >> /usr/lib/cacheurl.config");
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/play\\/(.*\\.mp4\\?end=[0-9]*) http://video.funshion.com/$1' >> /usr/lib/cacheurl.config");
			doSystem("echo 'http:\\/\\/[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\/play\\/(.*\\.mp4\\?start=[0-9]*) http://video.funshion.com/$1' >> /usr/lib/cacheurl.config");
		}
		if(prof->baiduMp3Enable)
		{
			doSystem("echo 'http:\\/\\/zhangmenshiting\\.baidu\\.com\\/data2\\/music\\/([0-9]*\\/[0-9]*\\.mp3)\\?xcode=.*  http://music.baidu.com/$1' >> /usr/lib/cacheurl.config");
		}

		doSystem("echo '/data/cache %dMB' > /etc/storage.config",prof->size*1024);
	}
}

/*************************************************************
 * 函数: cacheProfileChange
 * 功能: 缓存初始化函数
 * 创建: 2012-12-13
 * 参数: void
 * 返回: void
 * 作者: chen.bin
 *
 ************************************************************/
static void cacheProfileChange(Pointer tag,MibProfileType profileType,
		ProfChangeType changeType,Uint32 index,
		const CacheProfile *oldProfileDataPtr,
		const CacheProfile *newProfileDataPtr)
{

	char ip[16]={0};
	int count = 0;
	CacheProfile *prof = NULL;
	NetModeSwitchProfile *netModeSwitchProf = NULL;
	BridgeProfile *brProf= NULL;
	getIfIp(getLanIfName(), ip);
	switch(changeType)
	{
		case PROF_CHANGE_ADD:
		case PROF_CHANGE_EDIT:
			initCacheConfig();
			printf("cache.sh\n");
			doSystem("cache.sh&");
		case PROF_CHANGE_DEL:
		case PROF_CHANGE_DELALL:
		default:break;
	}
	return;
}



/*************************************************************
 * 函数:funInitCache
 * 功能: 缓存初始化函数
 * 创建: 2012-12-13
 * 参数: void
 * 返回: void
 * 作者: chen.bin
 *
 ************************************************************/
void funInitCache(void)
{

	registerForProfileUpdates(MIB_PROF_CACHE,(Pointer)NULL,
			(ProfileUpdateFnc)cacheProfileChange);
	initCacheConfig();
	doSystem("cache.sh&");

}


#endif
