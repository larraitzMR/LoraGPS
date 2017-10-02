/*
 * display.h
 *
 *  Created on: 19 jun. 2017
 *      Author: Larraitz
 */
#include "hw.h"
#include "stm32l0xx_hal_gpio.h"

// Control signal data pins

#define RS 	GPIO_PIN_0 	// LCD RS signal
//#define RW 	GPIO_PIN_7 	// LCD R/W signal
#define E 	GPIO_PIN_1 // LCD E signal

#define Control_Port	GPIOH
#define Control_Reg		ODR

#define DB0 GPIO_PIN_12
#define DB1 GPIO_PIN_2

#define DB2 GPIO_PIN_10
#define DB3 GPIO_PIN_8
#define DB4 GPIO_PIN_11
#define DB5 GPIO_PIN_12
#define DB6 GPIO_PIN_9
#define DB7 GPIO_PIN_0

#define LCD_PORT GPIOA

#define GPIOA_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define GPIOA_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()
#define GPIOB_CLK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()
#define GPIOB_CLK_DISABLE()		__HAL_RCC_GPIOB_CLK_DISABLE()
#define GPIOH_CLK_ENABLE()		__HAL_RCC_GPIOH_CLK_ENABLE()
#define GPIOH_CLK_DISABLE()		__HAL_RCC_GPIOH_CLK_DISABLE()

// Clear Display -------------- 0b00000001
#define LCD_CLEAR_DISPLAY       0x01

// Cursor Home ---------------- 0b0000001x
#define LCD_CURSOR_HOME         0x02

// Entry Mode Set ------------- 0b000001xx
#define LCD_SET_ENTRY           0x04
#define LCD_ENTRY_DECREASE      0x00
#define LCD_ENTRY_INCREASE      0x02
#define LCD_ENTRY_NOSHIFT       0x00
#define LCD_ENTRY_SHIFT         0x01

// Set Display ---------------- 0b00001xxx
#define LCD_SET_DISPLAY         0x08
#define LCD_DISPLAY_OFF         0x00
#define LCD_DISPLAY_ON          0x04
#define LCD_CURSOR_OFF          0x00
#define LCD_CURSOR_ON           0x02
#define LCD_BLINKING_OFF        0x00
#define LCD_BLINKING_ON         0x01

// Set Shift ------------------ 0b0001xxxx
#define LCD_SET_SHIFT           0x10
#define LCD_CURSOR_MOVE         0x00
#define LCD_DISPLAY_SHIFT       0x08
#define LCD_SHIFT_LEFT          0x00
#define LCD_SHIFT_RIGHT         0x04

// Function Set --------------- 0b001xxxxx
#define LCD_SET_FUNCTION        0x20
#define LCD_8BIT       			0x10
#define LCD_1LINE      			0x00
#define LCD_2LINE      			0x08
#define LCD_58					0x00
#define LCD_511					0x04

#define DDRAM  	0x80
#define LINEA1 	0x80
#define LINEA2	0xC0
#define LINEA3 	0x94
#define LINEA4 	0xD4


void LCD_Config(void);
void writePin(uint32_t pin, uint8_t state);
void LCD_Enable(void);
void LCD_Out(uint8_t data);
void LCD_Command(uint8_t data);
void LCD_Data(char data);
void LCD_Print_Char(char string);
void LCD_Print(char *string);
void LCD_Print_String(char string[]);
void decToBin(uint32_t dec);
void intToString4(uint32_t val);
void intToString3(uint32_t val);
//void LCD_Print(char string[]);
void LCD_Cursor(int linea);
void LCD_BuildChar(uint16_t loc, unsigned char *p);
void LCD_Init(void);
