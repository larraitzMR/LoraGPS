/*
 * gps.h
 *
 *  Created on: 1 jun. 2017
 *      Author: Larraitz
 */

#include "hw.h"

/* Exported functions ------------------------------------------------------- */

void gps_Config(void);
void gps_Init(void);

void SPI_Init();
void SPI_Config(void);
void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);
uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
