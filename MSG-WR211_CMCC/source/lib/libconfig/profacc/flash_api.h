
#ifndef __FLASHAPI_H
#define __FLASHAPI_H
extern int flash_write_mtdname(char *mtdname, char *buf, off_t to, size_t len);
extern int flash_read_mtdname(char *mtdname, char *buf, off_t from, size_t len);
extern int conf_flash_write(char *buf, off_t to, size_t len);
extern int conf_flash_read(char *buf, off_t from, size_t len);
extern int mtd_unlock(const char *mtd);
#endif
