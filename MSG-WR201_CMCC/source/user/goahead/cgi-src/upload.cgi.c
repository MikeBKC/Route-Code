#include "upload.cgi.h"

#define REFRESH_TIMEOUT		"40000"		/* 40000 = 40 secs*/

#define RFC_ERROR "RFC1867 error"
#define ERROR_ASP "SoftwareUpdate.asp"
//#define ERROR_ASP "error.asp"

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
#define MEM_HALF	512
int findStrInFile(char *filename, int offset, unsigned char *str, int str_len)
{
	int pos = 0, rc;
	FILE *fp;
	unsigned char mem[MEM_SIZE];

	if(str_len > MEM_HALF)
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
			pos += MEM_HALF;	// 8
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



#if defined (UPLOAD_FIRMWARE_SUPPORT)

/*
 *  taken from "mkimage -l" with few modified....
 */
int check(char *imagefile, int offset, int len, char *err_msg)
{
	struct stat sbuf;

	int  data_len;
	char *data;
	unsigned char *ptr;
	unsigned long checksum;

	image_header_t header;
	image_header_t *hdr = &header;

	int ifd;

	if ((unsigned)len < sizeof(image_header_t)) {
		sprintf (err_msg, "Bad size: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	ifd = open(imagefile, O_RDONLY);
	if(!ifd){
		sprintf (err_msg, "Can't open %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	if (fstat(ifd, &sbuf) < 0) {
		close(ifd);
		sprintf (err_msg, "Can't stat %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	ptr = (unsigned char *) mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
	if ((caddr_t)ptr == (caddr_t)-1) {
		close(ifd);
		sprintf (err_msg, "Can't mmap %s: %s\n", imagefile, strerror(errno));
		return 0;
    }
	ptr += offset;

	/*
	 *  handle Header CRC32
	 */
    memcpy (hdr, ptr, sizeof(image_header_t));

    data = (char *)(ptr + 8);
    data_len  =  ntohl(hdr->imageLen);
#if 0
    if(data_len > len - 8)
    {
		munmap(ptr, len);
		close(ifd);
		return 0;
    }
#endif
    if (crc32 (0, data, len - 8) != ntohl(hdr->imageCrc)) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has corrupted data!\n", imagefile);
		return 0;
    }
#if 1

	/*
	 * compare MTD partition size and image size
	 */
	if(len > getMTDPartSize("\"Kernel\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than Kernel MTD partition.\n", len);
		return 0;
	}
#endif

	munmap(ptr, len);
	close(ifd);

	return 1;
}


#endif /* UPLOAD_FIRMWARE_SUPPORT */

/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int getIfIp(char *ifname, char *if_addr)
{
    struct ifreq ifr;
    int skfd = 0;

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("getIfIp: open socket error");
        return -1;
    }

    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
    if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
        printf("getIfIp: ioctl SIOCGIFADDR error for %s", ifname);
        return -1;
    }
    strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    close(skfd);
    return 0;
}

/*
 * I'm too lazy to use popen() instead of system()....
 * ( note:  static buffer used)
 */
#define DEFAULT_LAN_IP "10.10.10.254"
char *getLanIP(void)
{
	static char buf[64];
	char *nl;
	FILE *fp;

	memset(buf, 0, sizeof(buf));
	if( (fp = popen("nvram_get 2860 lan_ipaddr", "r")) == NULL )
		goto error;

	if(!fgets(buf, sizeof(buf), fp)){
		pclose(fp);
		goto error;
	}

	if(!strlen(buf)){
		pclose(fp);
		goto error;
	}
	pclose(fp);

	if(nl = strchr(buf, '\n'))
		*nl = '\0';

	return buf;

error:
	fprintf(stderr, "warning, cant find lan ip\n");
	return DEFAULT_LAN_IP;
}

inline void webFoot(void)
{
    printf("</body></html>\n");
}


int main (int argc, char *argv[])
{
    int file_begin, file_end;
    int line_begin, line_end;
    char err_msg[256];
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
<META http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\n\
</head>\n\
<body>\n");

      line_begin = 0;
      if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
            printf("<script language=\"JavaScript\">\n\
		alert(\"�������ѡ�����\");\n\
		self.location=\"/%s\";\n\
	    </script>\n" ,ERROR_ASP);
	    webFoot();
	    exit(-1);
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
      //until now we dont care about what the true filename is.
      free(line);

      // We may check a string  "Content-Type: application/octet-stream" here,
      // but if our firmware extension name is the same with other known ones, 
      // the browser would use other content-type instead.
      // So we dont check Content-type here...
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


    // examination
#if defined (UPLOAD_FIRMWARE_SUPPORT)
    if(!check(filename, file_begin, file_end - file_begin, err_msg) ){
        goto err;
    }

    /*
     * write the current linux version into flash.
     */
    write_flash_kernel_version(filename, file_begin);
//#if 0
#ifdef CONFIG_RT2880_DRAM_8M
    system("killall goahead");
#endif
#if 1 
    // flash write
    if( mtd_write_firmware(filename, file_begin, file_end - file_begin) == -1){
        goto err;
    }
#endif
#elif defined (UPLOAD_BOOTLOADER_SUPPORT)
    mtd_write_bootloader(filename, file_begin, file_end - file_begin);
#else
#error "no upload support defined!"
#endif

//#if 0
#ifdef CONFIG_RT2880_DRAM_8M
    system("sleep 3 && reboot &");
#endif
    webFoot();
    free(boundary);
    system("sleep 3 && reboot &");
    exit(0);

err:
    printf("<script language=\"JavaScript\">\n\
		alert(\"�������ѡ�����\");\n\
		self.location=\"/%s\";\n\
	    </script>\n" ,ERROR_ASP);
    webFoot();
    free(boundary);
    exit(-1);
}

