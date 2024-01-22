#include "main.h"

#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

UART_HandleTypeDef uart2;

int main(void)
{
  HAL_Init();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  char a[7] = "hello\r\n";
  while (1)
  {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    HAL_UART_Transmit(&uart2, (uint8_t *)a, 7, 1000);
    HAL_Delay(1000);
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
//======================
// Drivers init
//======================
static void MX_USART2_UART_Init(void)
{

  __USART2_CLK_ENABLE();
  uart2.Instance = USART2;
  uart2.Init.BaudRate = 115200;
  uart2.Init.WordLength = UART_WORDLENGTH_8B;
  uart2.Init.StopBits = UART_STOPBITS_1;
  uart2.Init.Parity = UART_PARITY_NONE;
  uart2.Init.Mode = UART_MODE_TX_RX;
  uart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart2.Init.OverSampling = UART_OVERSAMPLING_16;

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = USART_TX_Pin | USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  if (HAL_UART_Init(&uart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}