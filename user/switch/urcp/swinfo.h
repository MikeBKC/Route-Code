#ifndef __SWINFO_H__
#define __SWINFO_H__

#include <flash_api.h>

#ifdef UBOOT_HEADER

#define IH_MAGIC        0x27051956      /* Image Magic Number           */
#define IH_NMLEN        20          /* Image Name Length            */

typedef struct image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	systemVer;	/* Image Creation Timestamp	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		name[IH_NMLEN];	/* Image Name		*/
	uint32_t    ih_reserve;
	uint32_t	ih_ksz;		/* Kernel Part Size		*/
} image_header_t;
#else
typedef struct image_header {
        uint32_t imageLen;
        uint32_t imageCrc;
        uint32_t arch;
        uint32_t fsLen;
        uint32_t os;
		uint32_t systemVer;  /* 前11位是软件版本，后5位是硬件版本。按53332比例分配，对应 1.1.1  1.1  */
        uint32_t imageComp;
        uint32_t loadAddr;
        uint32_t entryAddr;
        uint8_t  name[20];
        uint32_t kernelLen;
        uint32_t kernelCrc;
} image_header_t;
#endif

#define IMAGE_LENGTH	    0x34A000u
#define IMAGE_ROOT	    "/tmp/urcp/images/"  /*save firmware*/
#define MODEL_1800N	    "WA1800N"
#define MODEL_510W	    "nv510W"


#endif
