/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */

/* Example: Declarations of the platform and disk functions in the project */
#include "W25N01.h"

/* Example: Mapping of physical drive number for each drive */
#define SPI_FALSH 0


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case SPI_FALSH :
	while(W25N01_isDev_Busy());
	return RES_OK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{

	switch (pdrv) {
	case SPI_FALSH :
		W25N01_Init();
		while (W25N01_isDev_Busy());
	return RES_OK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{

	switch (pdrv) {
	case SPI_FALSH :
	FATFS_W25N01_Read_Sector((uint8_t*)buff,sector,count);
	return RES_OK;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{

	switch (pdrv) {
	case SPI_FALSH :
	if((sector/255)>=1)
	{
	W25N01_Erase_Block(sector/255);
	}
	FATFS_W25N01_Write_Sector((uint8_t*)buff,sector,count);
	return RES_OK;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{

	switch (pdrv)
	{
	case SPI_FALSH:
		switch (cmd)
		{
		case GET_SECTOR_COUNT:  // 总扇区数
				*(DWORD*)buff = 262144;  // 65,536
						break;
		case GET_BLOCK_SIZE:    // 擦除块大小（扇区单位）
				// 128KB擦除块 / 2KB扇区 = 64
				*(DWORD*)buff =256;  // 64
						break;
		case GET_SECTOR_SIZE:   // 扇区大小
				*(WORD*)buff = 512;  // 返回实际扇区大小
						break;
		}
		return RES_OK;
	} 
	return RES_PARERR;
}

