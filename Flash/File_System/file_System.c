#include "string.h"
#include "gpio.h"
#include "ff.h"
#include "W25N01.h"
#include "malloc.h"
#include "ST7735.h"
#include "usart.h"
BYTE work_Buf[FF_MAX_SS*10] ;  
 char W_Data[4096];
char R_Date[2048];
UINT bw;
UINT br;
DWORD fre_clust;
MKFS_PARM opt = {
.fmt = FM_FAT32,     // 强制FAT32
.n_fat = 1,          // 双FAT表
.align = 0,
.n_root = 0,
.au_size = 1024     // 4KB簇
};
FSIZE_t pos;
void W25N01_Test_FATFS(void)
{
    memset(W_Data,'i',4096);

    FATFS fs;
    FIL fil_obj;
    FRESULT res=f_mount(&fs,"0:", 1);
    if(res == FR_NO_FILESYSTEM)
     {
    Serial_Printf("Load Fail\n");
    W25N01_Erase_All();

    res = f_mkfs("0:", &opt, work_Buf, sizeof(work_Buf));
    if(res == FR_OK) 
    {
    f_mount(NULL, "0:", 1); // 卸载
    res=f_mount(&fs,"0:", 1); // 重新挂载
    }
    }   
    if(res)
    {
        Serial_Printf("%d\n",res);
    }
    else
    {
        Serial_Printf("Load Success\n");
    }
//     res=f_open(&fil_obj,"0:test_01.txt",FA_CREATE_ALWAYS|FA_WRITE);
//     if(res)
//     {
//         Serial_Printf("%d\n",res);
//         Serial_Printf("Open fail\n");
//     }
//     else
//     {
//         Serial_Printf("Open seccess\n");
//     }
//      pos = f_tell(&fil_obj);
//     Serial_Printf("文件指针位置: %lu\n", pos);

//     res=f_write(&fil_obj,W_Data,sizeof(W_Data),&bw);
//     Serial_Printf("bw=%d\r\n",bw);
//    if(res)
//     {
//         Serial_Printf("%d\n",res);
//         Serial_Printf("Write fail\n");
//     }
//     else
//     {
//         Serial_Printf("Write seccess\n");
//     }
//     res=f_close(&fil_obj);
//     if(res)
//     {
//         Serial_Printf("%d\n",res);
//         Serial_Printf("Close fail\n");
//     }
//     else
//     {
//         Serial_Printf("Close Success\n");
//     }
    res=f_open(&fil_obj,"0:test_01.txt",FA_OPEN_EXISTING|FA_READ);
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Open fail\n");
    }
    else
    {
        Serial_Printf("Open seccess\n");
    }
    // f_lseek(&fil_obj,0);
    res=f_read(&fil_obj,R_Date,f_size(&fil_obj),&br);
    DWORD mun_s=fil_obj.dir_sect;
    DWORD start_cluster = fil_obj.sect;
    ST7735_ShowNum(0,0,00000,8,0xFFFF,0x0000);
    ST7735_ShowNum(0,48,00000,8,0xFFFF,0x0000);
    ST7735_ShowNum(0,0,start_cluster,8,0xFFFF,0x0000);
    ST7735_ShowNum(0,48,mun_s,8,0xFFFF,0x0000);
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Read fail\n");
    }
    else
    {
        Serial_Printf("读取内容:%s,br=%d\n",R_Date,br);
    }
    f_close(&fil_obj);

res=f_open(&fil_obj,"0:test_02.txt",FA_CREATE_ALWAYS|FA_WRITE);
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Open fail\n");
    }
    else
    {
        Serial_Printf("Open seccess\n");
    }
     pos = f_tell(&fil_obj);
    Serial_Printf("文件指针位置: %lu\n", pos);
    f_lseek(&fil_obj,0);

    res=f_write(&fil_obj,W_Data,sizeof(W_Data),&bw);
    Serial_Printf("bw=%d\r\n",bw);
   if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Write fail\n");
    }
    else
    {
        Serial_Printf("Write seccess\n");
    }
    res=f_close(&fil_obj);
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Close fail\n");
    }
    else
    {
        Serial_Printf("Close Success\n");
    }
    res=f_open(&fil_obj,"0:test_02.txt",FA_OPEN_EXISTING|FA_READ);
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Open fail\n");
    }
    else
    {
        Serial_Printf("Open seccess\n");
    }
    // f_lseek(&fil_obj,0);
    res=f_read(&fil_obj,R_Date,f_size(&fil_obj),&br);
     mun_s=fil_obj.dir_sect;
     start_cluster = fil_obj.sect;
    ST7735_ShowNum(90,0,00000,8,0xFFFF,0x0000);
    ST7735_ShowNum(90,48,00000,8,0xFFFF,0x0000);
    ST7735_ShowNum(90,0,start_cluster,8,0xFFFF,0x0000);
    ST7735_ShowNum(90,48,mun_s,8,0xFFFF,0x0000);
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Read fail\n");
    }
    else
    {
        Serial_Printf("读取内容:%s,br=%d\n",R_Date,br);
    }
    f_close(&fil_obj);

//     res=f_open(&fil_obj,"0:test_03.txt",FA_CREATE_ALWAYS|FA_WRITE);
//     if(res)
//     {
//         Serial_Printf("%d\n",res);
//         Serial_Printf("Open fail\n");
//     }
//     else
//     {
//         Serial_Printf("Open seccess\n");
//     }
//      pos = f_tell(&fil_obj);
//     Serial_Printf("文件指针位置: %lu\n", pos);

//     res=f_write(&fil_obj,W_Data,sizeof(W_Data),&bw);
//     Serial_Printf("bw=%d\r\n",bw);
//    if(res)
//     {
//         Serial_Printf("%d\n",res);
//         Serial_Printf("Write fail\n");
//     }
//     else
//     {
//         Serial_Printf("Write seccess\n");
//     }
//     res=f_close(&fil_obj);
//     if(res)
//     {
//         Serial_Printf("%d\n",res);
//         Serial_Printf("Close fail\n");
//     }
//     else
//     {
//         Serial_Printf("Close Success\n");
//     }
    res=f_open(&fil_obj,"0:test_03.txt",FA_OPEN_EXISTING|FA_READ);
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Open fail\n");
    }
    else
    {
        Serial_Printf("Open seccess\n");
    }
    // f_lseek(&fil_obj,0);
    res=f_read(&fil_obj,R_Date,f_size(&fil_obj),&br);
     mun_s=fil_obj.dir_sect;
     start_cluster = fil_obj.sect;
    ST7735_ShowNum(80,16,00000,8,0xFFFF,0x0000);
    ST7735_ShowNum(80,16,start_cluster,8,0xFFFF,0x0000);
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Read fail\n");
    }
    else
    {
        Serial_Printf("读取内容:%s,br=%d\n",R_Date,br);
    }
    f_close(&fil_obj);

    res=f_mount(NULL, "0:", 0);  // 关键步骤！
    if(res)
    {
        Serial_Printf("%d\n",res);
        Serial_Printf("Unload fail\n");
    }
    else
    {
        Serial_Printf("Unload Success\n");
    }
    
}

