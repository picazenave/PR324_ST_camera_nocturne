#include "usart.h"
#include "fatfs.h"
#include "stdio.h"
#include <string.h>
#include "main.h"

FATFS FatFs;

HAL_StatusTypeDef sd_init()
{
    /**
     * SD test
     */
    printf("\r\n~ SD card demo by kiwih ~\r\n\r\n");
    // not needed bc camera init is already >1sec
    // HAL_Delay(1000); // a short delay is important to let the SD card settle

    // some variables for FatFs
    FIL fil;      // File handle
    FRESULT fres; // Result after operations

    // Open the file system
    fres = f_mount(&FatFs, "", 1); // 1=mount now
    if (fres != FR_OK)
    {
        printf("f_mount error (%i)\r\n", fres);
        while (1)
            ;
    }

    // Let's get some statistics from the SD card
    DWORD free_clusters, free_sectors, total_sectors;
    FATFS *getFreeFs;
    fres = f_getfree("", &free_clusters, &getFreeFs);
    if (fres != FR_OK)
    {
        printf("f_getfree error (%i)\r\n", fres);
        while (1)
            ;
    }
    total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
    free_sectors = free_clusters * getFreeFs->csize;
    printf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);

    // try to open "test.txt"
    fres = f_open(&fil, "test.txt", FA_READ);
    if (fres != FR_OK)
    {
        printf("f_open error (%i)\r\n", fres);
        while (1)
            ;
    }
    printf("OK open 'test.txt' for reading\r\n");
    BYTE readBuf[30];
    // We can either use f_read OR f_gets to get data out of files
    // f_gets is a wrapper on f_read that does some string formatting for us
    TCHAR *rres = f_gets((TCHAR *)readBuf, 30, &fil);
    if (rres != 0)
    {
        printf("Read string from 'test.txt' contents: %s\r\n", readBuf);
    }
    else
    {
        printf("f_gets error (%i)\r\n", fres);
    }
    f_close(&fil);

    // f_mount(NULL, "", 0); //FIXME demount drive
    return HAL_OK;
}

HAL_StatusTypeDef save_picture_sd(struct img_struct_t *img_struct)
{
    printf("Save picture SD\r\n");
    FIL fil;      // File handle
    FRESULT fres; // Result after operations
    HAL_StatusTypeDef status = HAL_ERROR;
    uint16_t jpg_counter = 0;
    char jpg_name[20] = {0};
    snprintf(jpg_name, 20, "photo%u.jpg", jpg_counter);
    fres = f_open(&fil, jpg_name, FA_READ);
    while (fres == FR_OK)
    {
        snprintf(jpg_name, 20, "photo%u.jpg", jpg_counter);
        fres = f_open(&fil, jpg_name, FA_READ);
        if (fres != FR_OK)
        {
            break;
        }
        f_close(&fil);
        jpg_counter++;
    }
    fres = f_open(&fil, jpg_name, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    if (fres != FR_OK)
    {
        printf("f_open error (%i)\r\n", fres);
    }
    printf("Current img name :%s \r\n", jpg_name);

    UINT bytesWrote = 0;
    fres = f_write(&fil, img_struct->img_buffer, img_struct->img_len, &bytesWrote);
    if (fres == FR_OK)
    {
        printf("Wrote %i bytes to %s!\r\n", bytesWrote, jpg_name);
    }
    else
    {
        printf("f_write error (%i)\r\n", fres);
    }
    f_close(&fil);

    return status;
}