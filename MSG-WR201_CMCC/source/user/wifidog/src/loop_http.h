#ifndef __LOOP_HTTP_H__
#define __LOOP_HTTP_H__

#define AUTH_OS_SERVER "api.greenwifi.com.cn"

char *getValueFromUrl(char *str, char *key, char *value, int maxValueLen);
char *getValueFromString(char *str, char *key, char *value, int maxValueLen);
int loop_http_get(char *host, char *url, char *recvMsg);
int loop_http_post(char *host, char *url, char *parameter,char *recvMsg);
int loop_http_get_agent(char *host, char *agent, char *url, char *recvMsg);


#endif
