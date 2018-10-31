/*
 * some functions for USB operation
 *
 * KeHuatao
 * 2011-11-24
 */
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/autoconf.h>
#include "usb-user.h"




/*
 *�ж�usb�洢���Ƿ����
 *KeHuatao
 */
int isUSBStorageExist(char *usb_mount_path)
{
    char buf[256];
    char device_path[256];
    char mount_path[256];
    FILE *fp=NULL, *fp2=NULL; 

    fp = fopen("/proc/mounts", "r");
    if(!fp){
	perror(__FUNCTION__);
	return 0;
    }

    while(fgets(buf, sizeof(buf), fp)){
	if(strstr(buf, USB_STORAGE_SIGN)){
	    sscanf(buf, "%s %s", device_path, mount_path);
	    fp2 = fopen(device_path, "r");
	    if(fp2!=NULL)
	    {
		if(usb_mount_path != NULL)
		{
		    strcpy(usb_mount_path, mount_path);
		    strcat(usb_mount_path, "/");
		}
		fclose(fp2);
		fclose(fp);
		return 1;
	    }
	}
    }

    fclose(fp);

    return 0;
}

int isStorageExist(char usb_mount_path[][MAX_PATH_LEN],int *count,unsigned int flag)
{
    char buf[256];
    char device_path[256];
    char mount_path[256];
    FILE *fp=NULL, *fp2=NULL;
    int i = 0;
    int device_exist = 0;

    fp = fopen("/proc/mounts", "r");
    if(!fp){
	perror(__FUNCTION__);
	return 0;
    }

    while(fgets(buf, sizeof(buf), fp)){
        device_exist = 0;

        if((flag & USB_EXIST) &&(strstr(buf, USB_STORAGE_SIGN)) ){
            device_exist = 1;
        }
        if((flag & SD_EXIST) &&(strstr(buf, SD_STORAGE_SIGN)) ){
            device_exist = 1;
        }

        if(device_exist){
            sscanf(buf, "%s %s", device_path, mount_path);
            fp2 = fopen(device_path, "r");
            if(fp2!=NULL)
            {
                if(usb_mount_path[i] != NULL)
                {
                    strcpy(usb_mount_path[i], mount_path);
                    strcat(usb_mount_path[i], "/");
                    i++;
                }
                fclose(fp2);
            }
        }
    }

    *count = i;
    fclose(fp);

    if(*count>0){
        return 1;
    }else{
        return 0;
    }
}


/**
 ** ��isStorageExist��д��ͬʱʶ��USB��SD���Ƿ����
 **/
int isStorageExist1(char usb_mount_path[][MAX_PATH_LEN],int *count,unsigned int flag,unsigned int flag1)
{
    char buf[256];
    char device_path[256];
    char mount_path[256];
    FILE *fp=NULL, *fp2=NULL;
    int i = 0;
    int device_exist = 0;

    fp = fopen("/proc/mounts", "r");
    if(!fp){
	perror(__FUNCTION__);
	return 0;
    }   

    while(fgets(buf, sizeof(buf), fp)){
	device_exist = 0;

	if((flag1 & USB_EXIST) &&(strstr(buf, USB_STORAGE_SIGN)) ){
            device_exist = 1;
	    sscanf(buf, "%s %s", device_path, mount_path);
	    fp2 = fopen(device_path, "r");
            if(fp2!=NULL)
            {
                if(usb_mount_path[i] != NULL)
		{
                    strcpy(usb_mount_path[i], mount_path);
                    strcat(usb_mount_path[i], "/");
		    i++;
                }
                fclose(fp2);
            }

	}
        if((flag & SD_EXIST) &&(strstr(buf, SD_STORAGE_SIGN)) ){
	    device_exist = 1;
	    sscanf(buf, "%s %s", device_path, mount_path);
            fp2 = fopen(device_path, "r");
	    if(fp2!=NULL)
	    {
		if(usb_mount_path[i] != NULL)
		{
                    strcpy(usb_mount_path[i], mount_path);
		    strcat(usb_mount_path[i], "/");
		    i++;
                }
                fclose(fp2);
            }

	}

    }
    *count = i;
    fclose(fp);

    if(*count>0){
	return 1;
    }else{
        return 0;
    }
}
/*
 *�����ļ�ϵͳʣ��ռ�
 *KeHuatqo
*2011-04-29
*/
int storageFree(const char * path, long long *disk_size, long long *disk_free)
{
    struct statfs buf;
    int ret=0;

    /*��ʼ��*/
    memset(&buf, 0, sizeof(struct statfs));
    if(statfs(path, &buf)==0)
    {
#if 0
	/*ÿ��block����������ֽ���*/
	printf("block size   = %ld\n", buf.f_bsize);
	printf("total blocks = %ld\n", buf.f_blocks);
	printf("free  blocks = %ld\n", buf.f_bfree);
	printf("total size   = %ld MB\n", (buf.f_bsize * buf.f_blocks)>>20); 
	printf("free size    = %ld MB\n", (buf.f_bsize * buf.f_bfree)>>20);
#endif
	if(disk_size!=NULL)
	{
	    *disk_size = (long long)(buf.f_bsize) * (long long)(buf.f_blocks);
	}
	/*����ʣ��ռ�*/
	if(disk_free!=NULL)
	{
	    *disk_free = (long long)(buf.f_bfree) * (long long)(buf.f_bsize);
	}
#if 0
	printf("disk free size = %lld B\n", freeDisk);
#endif
    }
    else
    {
	/*����*/
	ret = -1;
    }

    return ret;
}

/*
 *�ɿ��ļ�ϵͳ�ĸ��ƻ��ƶ��ļ�
 *
 *KeHuatao
 */
int copy_file(char* src_file, char* dst_file, int del_src, int append)
{
    int src_fd, dst_fd;
    char *in_data_ptr; 
    int len=-1;
    struct stat buf;
    mode_t mode;
    
    /*Դ�ļ�*/
    src_fd = open(src_file, O_RDONLY);
    if(src_fd!=-1)
    {
	/*��ȡԴ�ļ���Ϣ*/
	if((stat(src_file, &buf)==0) && (S_ISREG(buf.st_mode)))
	{
	    /*�����ļ�*/
	    mode = O_WRONLY|O_CREAT;
	    if(append==1)
	    {
		mode |= O_APPEND;
	    }
	    dst_fd = open(dst_file, mode);
	    if(dst_fd!=-1)
	    {
		/*�����ļ�*/
		in_data_ptr = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
		if(in_data_ptr != NULL) 
		{ 
		    /*�����ļ�*/
		    len = write(dst_fd, in_data_ptr, buf.st_size);
		    munmap(in_data_ptr, buf.st_size);
		    /*ɾ��*/
		    if(del_src==1)
		    {
			unlink(src_file);
		    }
		}
		/*�ر�*/
		close(dst_fd);
	    }
	}
	/*�ر�*/
	close(src_fd);
    }

    return len;
}

/**    
 * ��ȡ�������״̬            
 * total_space �ܿռ� unused ���ÿռ� 
 * ��������治������total_spaceΪ0
 **/    
void getOuterFlashStat(long long *total_space, long long *unused_space)
{
    char device_path[256];    

    memset(device_path, 0, sizeof(device_path));
    if(isUSBStorageExist(device_path)==1)
    {  
	storageFree(device_path, total_space, unused_space);
    }  
    return;
}
