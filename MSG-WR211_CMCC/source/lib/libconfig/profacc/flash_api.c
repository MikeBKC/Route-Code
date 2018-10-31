#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/autoconf.h>
#include "mib.h"
#include "profacce.h"

#if ((CONFIG_X86_CPU) == 1)
//#define CONFIG_FILE_PATH "/dev/sda3"
int conf_flash_read(char *buf, off_t from, size_t len)
{
	int fd, ret;

	fd = open(CONFIG_FILE_PATH, O_RDWR|O_CREAT, 0666);
	if (fd < 0) {
		fprintf(stderr, "Could not open config file\n");
		return -1;
	}

	lseek(fd, from, SEEK_SET);
	ret = read(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Reading from config file failed\n");
		close(fd);
		return -1;
	}

	close(fd);
	return ret;
}

int flash_read_mtdname(char* device, char *buf, off_t from, size_t len)
{
	int fd, ret;

	fd = open(device, O_RDWR|O_CREAT, 0666);
	if (fd < 0) {
		fprintf(stderr, "Could not open config file\n");
		return -1;
	}

	lseek(fd, from, SEEK_SET);
	ret = read(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Reading from config file failed\n");
		close(fd);
		return -1;
	}

	close(fd);
	return ret;
}


int conf_flash_write(char *buf, off_t to, size_t len)
{
	int fd, ret = 0;

	fd = open(CONFIG_FILE_PATH, O_RDWR|O_CREAT, 0666);
	if (fd < 0) {
		fprintf(stderr, "Could not open config file\n");
		return -1;
	}
	ret = write(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Writing to config file failed\n");
		close(fd);
		return -1;
	}

	close(fd);
	return ret;
}

int flash_write_mtdname(char* device, char *buf, off_t to, size_t len)
{
	int fd, ret = 0;

	fd = open(device, O_RDWR|O_CREAT, 0666);
	if (fd < 0) {
		fprintf(stderr, "Could not open config file\n");
		return -1;
	}
	lseek(fd, to, SEEK_SET);
	ret = write(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Writing to config file failed\n");
		close(fd);
		return -1;
	}

	close(fd);
	return ret;
}

#else

struct mtd_info_user {
	u_char type;
	u_int32_t flags;
	u_int32_t size;
	u_int32_t erasesize;
	u_int32_t oobblock;
	u_int32_t oobsize;
	u_int32_t ecctype;
	u_int32_t eccsize;
};

struct erase_info_user {
	u_int32_t start;
	u_int32_t length;
};

#define MEMGETINFO	_IOR('M', 1, struct mtd_info_user)
#define MEMERASE	_IOW('M', 2, struct erase_info_user)

int mtd_open(const char *name, int flags)
{
	FILE *fp;
	char dev[80];
	int i, ret;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, name)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				if ((ret = open(dev, flags)) < 0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
					ret = open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}
	return -1;
}


int conf_flash_read(char *buf, off_t from, size_t len)
{
	int fd, ret;
	struct mtd_info_user info;

	fd = mtd_open("xmlConfig", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes - %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	close(fd);
	fd = mtd_open("xmlConfig", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd block device\n");
		return -1;
	}

	lseek(fd, from, SEEK_SET);
	ret = read(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Reading from mtd failed\n");
		close(fd);
		return -1;
	}

	close(fd);
	return ret;
}

#define min(x,y) ({ typeof(x) _x = (x); typeof(y) _y = (y); (void) (&_x == &_y); _x < _y ? _x : _y; })

int conf_flash_write(char *buf, off_t to, size_t len)
{
	int fd, ret = 0;
	char *bak = NULL;
	struct mtd_info_user info;
	struct erase_info_user ei;

	fd = mtd_open("xmlConfig", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes: %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	while (len > 0) {
		if ((len & (info.erasesize-1)) || (len < info.erasesize)) {
			int piece_size;
			unsigned int piece, bakaddr;

			bak = (char *)malloc(info.erasesize);
			if (bak == NULL) {
				fprintf(stderr, "Not enough memory\n");
				close(fd);
				return -1;
			}

			bakaddr = to & ~(info.erasesize - 1);
			lseek(fd, bakaddr, SEEK_SET);

			ret = read(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Reading from mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			piece = to & (info.erasesize - 1);
			piece_size = min(len, info.erasesize - piece);
			memcpy(bak + piece, buf, piece_size);

			ei.start = bakaddr;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			lseek(fd, bakaddr, SEEK_SET);
			ret = write(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			free(bak);
			buf += piece_size;
			to += piece_size;
			len -= piece_size;
		}
		else {
			ei.start = to;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				return -1;
			}

			ret = write(fd, buf, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			buf += info.erasesize;
			to += info.erasesize;
			len -= info.erasesize;
		}
	}

	close(fd);
	return ret;
}

#if (UTT_NV_D908W == FYES)
/*支持AC功能，且把AP设备的配置保存到FLASH中*/

int ap_conf_flash_read(char *buf, off_t from, size_t len, char *mtd_name)
{
	int fd, ret;
	struct mtd_info_user info;
	if((buf != NULL) && (mtd_name != NULL)) {
	fd = mtd_open(mtd_name, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes - %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	close(fd);
	fd = mtd_open(mtd_name, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd block device\n");
		return -1;
	}

	lseek(fd, from, SEEK_SET);
	ret = read(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Reading from mtd failed\n");
		close(fd);
		return -1;
	}

	close(fd);
	}
	return ret;
}

int ap_conf_flash_write(char *buf, off_t to, size_t len, char *mtd_name)
{
	int fd, ret = 0;
	char *bak = NULL;
	struct mtd_info_user info;
	struct erase_info_user ei;

	if((buf !=NULL) && (mtd_name != NULL)) {
	fd = mtd_open(mtd_name, O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes: %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	while (len > 0) {
		if ((len & (info.erasesize-1)) || (len < info.erasesize)) {
			int piece_size;
			unsigned int piece, bakaddr;

			bak = (char *)malloc(info.erasesize);
			if (bak == NULL) {
				fprintf(stderr, "Not enough memory\n");
				close(fd);
				return -1;
			}

			bakaddr = to & ~(info.erasesize - 1);
			lseek(fd, bakaddr, SEEK_SET);

			ret = read(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Reading from mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			piece = to & (info.erasesize - 1);
			piece_size = min(len, info.erasesize - piece);
			memcpy(bak + piece, buf, piece_size);

			ei.start = bakaddr;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			lseek(fd, bakaddr, SEEK_SET);
			ret = write(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			free(bak);
			buf += piece_size;
			to += piece_size;
			len -= piece_size;
		}
		else {
			ei.start = to;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				return -1;
			}

			ret = write(fd, buf, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			buf += info.erasesize;
			to += info.erasesize;
			len -= info.erasesize;
		}
	}

	close(fd);
	}
	return ret;
}
#endif

#define MEMUNLOCK               _IOW('M', 6, struct erase_info_user)
int mtd_unlock(const char *mtd)
{
	int fd;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdLockInfo;

	fd = mtd_open(mtd, O_RDWR | O_SYNC);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info from %s\n", mtd);
		close(fd);
		exit(1);
	}

	mtdLockInfo.start = 0;
	mtdLockInfo.length = mtdInfo.size;
	if(ioctl(fd, MEMUNLOCK, &mtdLockInfo)) {
		close(fd);
		return 0;
	}
		
	close(fd);
	return 0;
}

int flash_write_mtdname(char *mtdname, char *buf, off_t to, size_t len)
{
	int fd, ret = 0;
	char *bak = NULL;
	struct mtd_info_user info;
	struct erase_info_user ei;

	fd = mtd_open(mtdname, O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes: %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	while (len > 0) {
		if ((len & (info.erasesize-1)) || (len < info.erasesize)) {
			int piece_size;
			unsigned int piece, bakaddr;

			bak = (char *)malloc(info.erasesize);
			if (bak == NULL) {
				fprintf(stderr, "Not enough memory\n");
				close(fd);
				return -1;
			}

			bakaddr = to & ~(info.erasesize - 1);
			lseek(fd, bakaddr, SEEK_SET);

			ret = read(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Reading from mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			piece = to & (info.erasesize - 1);
			piece_size = min(len, info.erasesize - piece);
			memcpy(bak + piece, buf, piece_size);

			ei.start = bakaddr;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}
			fprintf(stderr,"e");

			lseek(fd, bakaddr, SEEK_SET);
			ret = write(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}
			fprintf(stderr,"w");

			free(bak);
			buf += piece_size;
			to += piece_size;
			len -= piece_size;
		}
		else {
			ei.start = to;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				return -1;
			}
			fprintf(stderr,"e");

			ret = write(fd, buf, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}
			printf(stderr,"w");

			buf += info.erasesize;
			to += info.erasesize;
			len -= info.erasesize;
		}
	}

	printf("\n");
	close(fd);
	return ret;
}

#if (UTT_SMART_UI == FYES)
/********************************************************************
 * 函数： flash_mtd_size
 * 功能： 获取Flash的mtd kernel size
 * 创建： 2013-12-3
 * 参数： MTD名称
 * 返回： 成功->mtd kernel size，失败->-1
 * 作者： zhao.chong 
 ********************************************************************/ 
size_t flash_mtd_size(char *mtdname)
{
	int fd;
	struct mtd_info_user info;

	fd = mtd_open(mtdname, O_RDONLY);
	if (fd < 0) {
	    fprintf(stderr, "Could not open mtd device\n");
	    return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
	    fprintf(stderr, "Could not get mtd device info\n");
	    close(fd);
	    return -1;
	}

	close(fd);

	return info.size;
}
#endif

int flash_read_mtdname(char *mtdname, char *buf, off_t from, size_t len)
{
	int fd, ret;
	struct mtd_info_user info;

	fd = mtd_open(mtdname, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes - %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}
#if 1
	close(fd);
	fd = mtd_open(mtdname, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd block device\n");
		return -1;
	}
#endif

	lseek(fd, from, SEEK_SET);
	ret = read(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Reading from mtd failed\n");
		close(fd);
		return -1;
	}

	close(fd);
	return ret;
}

int flash_read_image_to_path(char *file, int imageLen)
{
	int fdSrc, fdDst;
	char buf[16384];
	int ret1, ret2, count;
	struct mtd_info_user info;

	fdSrc = mtd_open("Kernel", O_RDONLY);
	if (fdSrc < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	fdDst = open(file, O_RDWR | O_CREAT, 0644);
	if (fdDst < 0) {
		fprintf(stderr, "Could not open %s\n", file);
		return -1;
	}

	if (ioctl(fdSrc, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fdSrc);
		return -1;
	}

	lseek(fdSrc, 0, SEEK_SET);
	count = 0;
	while (1) {
		if (sizeof(buf) <= (imageLen - count)) {
			ret1 = read(fdSrc, buf, sizeof(buf));
		} else {
			ret1 = read(fdSrc, buf, imageLen - count);
		}
		if (ret1 == -1) {
			fprintf(stderr, "Reading from mtd failed\n");
			close(fdSrc);
			close(fdDst);
			return -1;
		}

		count += ret1;
		ret2 = write(fdDst, buf, ret1);
		if (ret2 == -1) {
			fprintf(stderr, "Writing to %s failed\n", file);
			close(fdSrc);
			close(fdDst);
			return -1;
		}

		if (ret2 < sizeof(buf)) {
			break;
		}
	}

	close(fdSrc);
	close(fdDst);
	return 0;
}


#ifdef CONFIG_UTT_PANIC_DEBUG
int flash_erase_mtdname(char *mtdname)
{
	int fd, ret = 0;
	struct mtd_info_user info;
	struct erase_info_user ei;

	fd = mtd_open(mtdname, O_RDWR | O_SYNC);
	if (fd < 0) {
	    fprintf(stderr, "Could not open mtd device\n");
	    return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
	    fprintf(stderr, "Could not get mtd device info\n");
	    close(fd);
	    return -1;
	}

	ei.start = 0;
	ei.length = info.erasesize;
	if (ioctl(fd, MEMERASE, &ei) < 0) {
	    fprintf(stderr, "Erasing mtd failed\n");
	    close(fd);
	    return -1;
	}

	close(fd);
	return ret;
}
#endif

#endif
