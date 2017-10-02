/*
 * uart.c
 *
 *  Created on: 9 ago. 2017
 *      Author: Larraitz
 */

#include "hw.h"
#include "uart.h"

UART_HandleTypeDef huart2;

void UART_Config(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIOC_CLK_ENABLE();

    /* Peripheral clock enable */
    __USART2_CLK_ENABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART6_TX
    PA3     ------> USART6_RX
    */
    GPIO_InitStruct.Pin 	  	= GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode   		= GPIO_MODE_AF_PP; //GPIO_MODE_ANALOG?
    GPIO_InitStruct.Pull   		= GPIO_NOPULL;
    GPIO_InitStruct.Speed 	  	= GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate 	= GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/**
  * @brief  Inicializacion de la UART
  * @param  None
  * @retval None
  */
void UART2_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);

  HAL_NVIC_SetPriority(USART2_IRQn, 0x1, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}
