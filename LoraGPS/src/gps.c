/*
 * gps.c
 *
 *  Created on: 1 jun. 2017
 *      Author: Larraitz
 */

#include "gps.h"
#include "hw.h"

SPI_HandleTypeDef hspi2;

void SPI_Config()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __SPI2_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	/**SPI2 GPIO Configuration
	 PB15     ------> SPI2_MOSI
	 PB14     ------> SPI2_MISO
	 PB13     ------> SPI2_SCK
	 PB12     ------> SPI2_NSS
	 */

	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;	//GPIO_NOPULL
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin  = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Peripheral interrupt init*/
	HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(SPI2_IRQn);
}


void SPI_Init(void)
{
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;

  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi2.Init.CRCPolynomial = 10;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.NSS = SPI_NSS_SOFT; //SPI_NSS_HARD_OUTPUT
  hspi2.Init.TIMode = SPI_TIMODE_DISABLED;

  HAL_SPI_Init(&hspi2);
}


//uint8_t aTxBuf[] = "****SPI - Two Boards communication based on Interrupt **** SPI Message ******** SPI Message ******** SPI Message ****";
//extern __IO ITStatus SPIReady;
extern uint8_t aRxBuffer[];
/**
  * @brief  Tx Transfer completed callback
  * @param  hspi: SPI handle.
  * @retval None
  */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
//	SPIReady = SET;
//	HAL_SPI_Transmit_IT(&hspi2, (uint8_t*) aTxBuf, BUFFERSIZE);
//	PRINTF("%s\n\r", RxBuf);
//	if (HAL_SPI_Receive_IT(&hspi2, (uint8_t *) RxBuf, strlen(RxBuf))!= HAL_OK) {
//		/* Transfer error in transmission process */
//		Error_Handler();
//	}
//	while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
//	}
}

/**
  * @brief  Tx Transfer completed callback
  * @param  hspi: SPI handle.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{

}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  Flushes the buffer
  * @param  pBuffer: buffers to be flushed.
  * @param  BufferLength: buffer's length
  * @retval None
  */
void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    *pBuffer = 0;

    pBuffer++;
  }
}
