
/*
 * Codes at here are heavily taken from upload.cgi.c which is for large file uploading , but 
 * in fact "upload_settings" only need few memory(~16k) so it is not necessary to follow 
 * upload.cgi.c at all.
 * 
 * YYHuang@Ralink TODO: code size.
 *  
 */

#include <unistd.h>	//for unlink
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/reboot.h>

#define SETTING_LENGTH 56*1024
#define RFC_ERROR "RFC1867 ...."


void *memmem(const void *buf, size_t buf_len, const void *byte_line, size_t byte_line_len)
{
    unsigned char *bl = (unsigned char *)byte_line;
    unsigned char *bf = (unsigned char *)buf;
    unsigned char *p  = bf;

    while (byte_line_len <= (buf_len - (p - bf))){
        unsigned int b = *bl & 0xff;
        if ((p = (unsigned char *) memchr(p, b, buf_len - (p - bf))) != NULL){
            if ( (memcmp(p, byte_line, byte_line_len)) == 0)
                return p;
            else
                p++;
        }else{
            break;
        }
    }
    return NULL;
}

#define MEM_SIZE	1024
#define MEM_HALT	512
int findStrInFile(char *filename, int offset, unsigned char *str, int str_len)
{
	int pos = 0, rc;
	FILE *fp;
	unsigned char mem[MEM_SIZE];

	if(str_len > MEM_HALT)
		return -1;
	if(offset <0)
		return -1;

	fp = fopen(filename, "rb");
	if(!fp)
		return -1;

	rewind(fp);
	fseek(fp, offset + pos, SEEK_SET);
	rc = fread(mem, 1, MEM_SIZE, fp);
	while(rc){
		unsigned char *mem_offset;
		mem_offset = (unsigned char*)memmem(mem, rc, str, str_len);
		if(mem_offset){
			fclose(fp);	//found it
			return (mem_offset - mem) + pos + offset;
		}

		if(rc == MEM_SIZE){
			pos += MEM_HALT;	// 8
		}else
			break;
		
		rewind(fp);
		fseek(fp, offset+pos, SEEK_SET);
		rc = fread(mem, 1, MEM_SIZE, fp);
	}

	fclose(fp);
	return -1;
}

/*
 *  ps. callee must free memory...
 */
void *getMemInFile(char *filename, int offset, int len)
{
    void *result;
    FILE *fp;
    if( (fp = fopen(filename, "r")) == NULL ){
        return NULL;
    }
	fseek(fp, offset, SEEK_SET);
    result = malloc(sizeof(unsigned char) * len );
	if(!result)
		return NULL;
    if( fread(result, 1, len, fp) != len){
        free(result);
        return NULL;
    }
    return result;
}

int import(char *filename, int offset, int len)
{
    FILE *fp;
#define BUFSZ 1024
    unsigned char buf[BUFSZ], *p;
    int found = 0;


    char cmd[SETTING_LENGTH];
    char data;
    FILE *fp1, *src;
    char *pname = tempnam("/var", "set");

    
    snprintf(cmd, SETTING_LENGTH, "cp %s /var/tmpcgi", filename);
    system(cmd);
    
    if(!( fp = fopen(pname, "w+"))){
      return;
    }

    if(!( src = fopen(filename, "r"))){
      fclose(fp);
      return;
    }

    if( fseek(src, offset, SEEK_SET) == -1){
        printf("fseek error\n");
    }

    // ralink_init utility need this to identity(?) .dat file.
    //fprintf(fp, "Default\n");

    while( len > 0){
        if(! fread(&data, 1, 1, src))
            break;
        fwrite(&data, 1, 1, fp);
        len--;
    }
    fclose(fp);    
    fclose(src);

    if (NULL == (fp1 = fopen(pname, "r"))) {
	return -1;
    }
    while (NULL != fgets(buf, BUFSZ, fp1)) {
	if (buf[0] == '\n' || buf[0] == '#')
		continue;
	if (!strncmp(buf, "Default\n", 8)) {
	    found = 1;
	    break;
	}
    }
    fclose(fp1);
    if(found == 0)
    {
	unlink(pname);
	system("rm -f /var/tmpcgi");
	return -1;
    }
    system("ralink_init clear 2860");
    snprintf(cmd, 4096, "ralink_init renew 2860 %s", pname);
    system(cmd);
    unlink(pname);
    system("rm -f /var/tmpcgi");
    return 0;
}

int main (int argc, char *argv[])
{
    int file_begin, file_end;
    int line_begin, line_end;
    char *boundary; int boundary_len;
    char *filename = getenv("UPLOAD_FILENAME");

    printf(
"\
Server: %s\n\
Pragma: no-cache\n\
Content-type: text/html\n",
getenv("SERVER_SOFTWARE"));

    printf("\n\
<html>\n\
<head>\n\
</head>\n\
<body>\n");

    if(!filename){
        printf("<script language=\"JavaScript\">\n\
		alert(\"配置文件格式错误\");\n\
		self.location=\"/DeviceConfig.asp\";\n\
	    </script>\n");
	printf("</body></html>\n");
	exit(0);
    }

    line_begin = 0;
    if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
        printf("<script language=\"JavaScript\">\n\
		alert(\"配置文件格式错误\");\n\
		self.location=\"/DeviceConfig.asp\";\n\
	    </script>\n");
	printf("</body></html>\n");
	exit(0);
    }
	boundary_len = line_end - line_begin;
    boundary = getMemInFile(filename, line_begin, boundary_len);

    // sth like this..
    // Content-Disposition: form-data; name="filename"; filename="\\192.168.3.171\tftpboot\a.out"
    //
    char *line, *semicolon, *user_filename;
    line_begin = line_end + 2;
    if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
        goto err;
    }
    line = getMemInFile(filename, line_begin, line_end - line_begin);
    if(strncasecmp(line, "content-disposition: form-data;", strlen("content-disposition: form-data;"))){
        goto err;
    }
    semicolon = line + strlen("content-disposition: form-data;") + 1;
    if(! (semicolon = strchr(semicolon, ';'))  ){
        goto err;
    }
    user_filename = semicolon + 2;
    if( strncasecmp(user_filename, "filename=", strlen("filename="))  ){
        goto err;
    }
    user_filename += strlen("filename=");
    //until now we dont care about what  true filename is.
    free(line);

    // We may check a string  "Content-Type: application/octet-stream" here
    // but if our firmware extension name is the same with other known name, 
    // the browser will define other content-type itself instead,
    line_begin = line_end + 2;
    if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
        goto err;
    }

    line_begin = line_end + 2;
    if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
        goto err;
    }

    file_begin = line_end + 2;

    if( (file_end = findStrInFile(filename, file_begin, boundary, boundary_len)) == -1){
        goto err;
    }
    file_end -= 2;		// back 2 chars.(\r\n);
    if(file_end - file_begin > SETTING_LENGTH - 416)
    {
	printf("<script language=\"JavaScript\">\n\
		alert(\"配置文件太大\");\n\
		self.location=\"/DeviceConfig.asp\";\n\
	    </script>\n");
	printf("</body></html>\n");
	exit(0);
    }
    if((import(filename, file_begin, file_end - file_begin))==-1)
	goto err;
    printf("<script language=\"JavaScript\">\n\
		self.location=\"/DeviceConfig.asp\";\n\
	    </script>\n");
    printf("</body></html>\n");
    goto out;
err:
    printf("<script language=\"JavaScript\">\n\
		alert(\"配置文件格式错误\");\n\
		self.location=\"/DeviceConfig.asp\";\n\
	    </script>\n");
    printf("</body></html>\n");
    free(boundary);
    exit(0);
out:
    free(boundary);
    exit(0);
}

