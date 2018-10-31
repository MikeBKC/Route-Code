#ifndef _USB_USER_
#define _USB_USER_


#define MAX_PATH_LEN 256

#define USB_EXIST 0x01
#define SD_EXIST 0x02

#define USB_STORAGE_PR	    "sd"
#define SD_STORAGE_PR	    "mmcblk"
#define USB_STORAGE_SIGN    "/media/"USB_STORAGE_PR
#define SD_STORAGE_SIGN    "/media/"SD_STORAGE_PR

extern int isUSBStorageExist(char *usb_mount_path);
extern int isStorageExist(char usb_mount_path[][MAX_PATH_LEN],int *count,unsigned int flag);
extern int isStorageExist1(char usb_mount_path[][MAX_PATH_LEN],int *count,unsigned int flag,unsigned int flag1);
extern int storageFree(const char * path, long long *disk_size, long long *disk_free);
extern int copy_file(char* src_file, char* dst_file, int del_src, int append);
extern void getOuterFlashStat(long long *total_space, long long *unused_space);
#endif
