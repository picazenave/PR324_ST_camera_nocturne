#include "usart.h"
#include "fatfs.h"
#include "stdio.h"
#include <string.h>
#include "main.h"

enum frame_size_t
{
    FRAMESIZE_UXGA = 0,
    FRAMESIZE_QVGA = 1,
    FRAMESIZE_CIF = 2,
    FRAMESIZE_VGA = 3,
    FRAMESIZE_SVGA = 4,
    FRAMESIZE_XGA = 5,
    FRAMESIZE_SXGA = 6
};
struct serial_camera_config_t
{
    uint8_t brightness;
    uint8_t special_effect;
    uint8_t jpg_quality;
    uint8_t frame_size;
};
struct serial_camera_config_t camera_config = {.brightness = 0, .special_effect = 2, .jpg_quality = 20, .frame_size = FRAMESIZE_SVGA};
uint8_t use_default_camera_config = 0;

uint8_t jpg_header[5] = {0x11, 0x12, 0x55, 0x44, 0xFF};

extern volatile uint8_t uart2_tx_done;
extern volatile uint8_t uart1_rx_done;

// 1 for default config
HAL_StatusTypeDef camera_init(uint8_t default_config)
{
    uint8_t to_transmit[2] = {0xA1, 0};
    uint8_t received = 0;
    uint32_t uart1_timeout = 1000;
    HAL_StatusTypeDef status = HAL_ERROR;
    /*
     * Camera init
     */
    printf("\r\n=====Camera Init=====\r\n\r\n");
    // 1 for default values
    // serial_camera_config_t camera_config = {.brightness = 0, .special_effect = 0, .jpg_quality = 30, .frame_size = FRAMESIZE_UXGA};
    printf("RESET CAM\r\n");
    HAL_GPIO_WritePin(CAMERA_RST_N_GPIO_Port, CAMERA_RST_N_Pin, GPIO_PIN_RESET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(CAMERA_RST_N_GPIO_Port, CAMERA_RST_N_Pin, GPIO_PIN_SET);
    HAL_Delay(1000);
    printf("RESET CAM DONE\r\n");
    uint8_t temp[200] = {0};
    status = HAL_UART_Receive(&huart1, temp, 200, 500); // dont check this is to flush input buffer
    // CHECK_HAL_STATUS_OR_PRINT(status);

    if (default_config) // check if camera is already configured
    {
        to_transmit[0] = 0xAA;
        status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
        CHECK_HAL_STATUS_OR_PRINT(status);
        status = HAL_UART_Receive(&huart1, &received, 1, uart1_timeout);
        CHECK_HAL_STATUS_OR_PRINT(status);

        // already configured nothing to do (setup is already done on camera)
        if (received == 0x04)
            return HAL_OK;
        else
        {
            // signal end of config
            to_transmit[0] = 0xA2;
            status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
            CHECK_HAL_STATUS_OR_PRINT(status);
            // check camera status
            to_transmit[0] = 0xAA;
            status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
            CHECK_HAL_STATUS_OR_PRINT(status);
            status = HAL_UART_Receive(&huart1, &received, 1, uart1_timeout);
            CHECK_HAL_STATUS_OR_PRINT(status);

            if (received == 0x04)
                return HAL_OK;
            else
                return HAL_ERROR;
        }
    }
    /**
     * Not default config
     */
    to_transmit[0] = 0xA1;
    status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);

    HAL_Delay(1000);
    to_transmit[0] = 0xAA;
    while (status != HAL_OK)
    {
        status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
        CHECK_HAL_STATUS_OR_PRINT(status);
        status = HAL_UART_Receive(&huart1, &received, 1, uart1_timeout);
        CHECK_HAL_STATUS_OR_PRINT(status);
    }

    if (status != HAL_OK)
    {
        printf("ACK receive error:0x%X\r\n", status);
        while (1)
            ;
    }
    if (received != 0x00)
    {
        printf("ACK receive not 0x00:0x%X\r\n", received);
        while (1)
            ;
    }
    // brightness
    to_transmit[0] = 0x34;
    to_transmit[1] = camera_config.brightness;
    status = HAL_UART_Transmit(&huart1, to_transmit, 2, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    if (status != HAL_OK)
    {
        printf("brightness transmit error:0x%X\r\n", status);
        while (1)
            ;
    }
    // special effect
    to_transmit[0] = 0x33;
    to_transmit[1] = camera_config.special_effect;
    status = HAL_UART_Transmit(&huart1, to_transmit, 2, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    if (status != HAL_OK)
    {
        printf("special effect transmit error:0x%X\r\n", status);
        while (1)
            ;
    }
    // jpg_quality
    to_transmit[0] = 0x22;
    to_transmit[1] = camera_config.jpg_quality; // 30
    status = HAL_UART_Transmit(&huart1, to_transmit, 2, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    if (status != HAL_OK)
    {
        printf("jpg_quality transmit error:0x%X\r\n", status);
        while (1)
            ;
    }
    // frame_size
    to_transmit[0] = 0x11;
    to_transmit[1] = camera_config.frame_size;
    status = HAL_UART_Transmit(&huart1, to_transmit, 2, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    if (status != HAL_OK)
    {
        printf("frame_size transmit error:0x%X\r\n", status);
        while (1)
            ;
    }
    // end of config
    to_transmit[0] = 0xA2;
    status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    if (status != HAL_OK)
    {
        printf("end of config transmit error:0x%X\r\n", status);
        while (1)
            ;
    }
    to_transmit[0] = 0xAA;
    while (status != HAL_OK || received == 0x00)
    {
        status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
        CHECK_HAL_STATUS_OR_PRINT(status);
        status = HAL_UART_Receive(&huart1, &received, 1, uart1_timeout);
        CHECK_HAL_STATUS_OR_PRINT(status);
        printf("received:0x%02X\r\n", received);
    }
    if (status != HAL_OK)
    {
        printf("ACK end config receive error:0x%02X\r\n", status);
        while (1)
            ;
    }
    if (received != 0x04)
    {
        printf("ACK end config receive not 0x04:0x%X\r\n", received);
        while (1)
            ;
    }
    printf("Wait 2 sec for camera awb to stabilize\r\n");
    HAL_Delay(2000);
    printf("OK\r\n");
    printf("Camera init DONE\r\n");
    return HAL_OK;
}

HAL_StatusTypeDef get_camera_jpg(struct img_struct_t *img_struct)
{
    // DMA receive
    HAL_StatusTypeDef status = HAL_ERROR;
    uint16_t uart1_timeout = 1000;
    uint8_t to_transmit[2] = {0, 0};
    to_transmit[0] = 0x55;
    status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    uint8_t img_len_buffer[2] = {0, 0};
    status = HAL_UART_Receive(&huart1, img_len_buffer, 2, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    img_struct->img_len = (img_len_buffer[0] << 8) + img_len_buffer[1];
    printf("im_len DMA=%u\r\n", img_struct->img_len);

    status = HAL_UART_Receive_DMA(&huart1, img_struct->img_buffer, img_struct->img_len);
    CHECK_HAL_STATUS_OR_PRINT(status);
    printf("DMA NDTR before get jpg:%lu\r\n", DMA2_Stream2->NDTR);
    // start get jpg
    status = HAL_UART_Transmit(&huart1, to_transmit, 1, uart1_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    // wait end of transfer
    while (!uart1_rx_done)
        ;
    uart1_rx_done = 0;
    printf("jpg received --> DMA\r\n");

    return status;
}

HAL_StatusTypeDef send_jpg_uart2(struct img_struct_t *img_struct, uint8_t blocking)
{
    uint16_t uart2_timeout = 1000;
    uint8_t to_transmit[2] = {0};
    HAL_StatusTypeDef status;
    status = HAL_UART_Transmit(&huart2, jpg_header, sizeof(jpg_header), uart2_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    to_transmit[0] = (uint8_t)(img_struct->img_len >> 8);
    to_transmit[1] = (uint8_t)(img_struct->img_len & 0xFF);
    status = HAL_UART_Transmit(&huart2, to_transmit, 2, uart2_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    status = HAL_UART_Transmit_DMA(&huart2, img_struct->img_buffer, img_struct->img_len);
    CHECK_HAL_STATUS_OR_PRINT(status);

    if (blocking)
    {   
        uart2_tx_done = 0;
        while (!uart2_tx_done)
            ;
        //uart2_tx_done = 0;
    }

    // for (u_int32_t i = 0; i < img_len; i++)
    //   printf("%.2X", img_buffer[i]);
    return status;
}