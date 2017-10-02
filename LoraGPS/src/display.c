/*
 * display.c
 *
 *  Created on: 19 jun. 2017
 *      Author: Larraitz
 */

#include "display.h"

/**
  * @brief  Configuracion del LCD
  * @param  None
  * @retval None
  */
void LCD_Config(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOH_CLK_ENABLE();
	GPIO_InitStruct.Pin = RS | E; // pin que desamos configurar
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // lo vamos a usar como salida en push - pull
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(Control_Port, &GPIO_InitStruct);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = DB7 | DB6 | DB5 | DB4 | DB3 | DB2; // pin que desamos configurar
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // lo vamos a usar como salida en push - pull
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin =  DB1 | DB0; // pin que desamos configurar
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // lo vamos a usar como salida en push - pull
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



}

/**
  * @brief  Se escribe el estado (Set,Reset) en el pin seleccionado
  * @param  pin: ID del pin, state: estado (set, reset) al que se quiere poner
  * @retval None
  */
void writePin(uint32_t pin, uint8_t state) {
//	if (pin == RS || pin == RW || pin == E) {
	if (pin == RS || pin == E) {
		HAL_GPIO_WritePin(Control_Port, pin, state);
	} else {
		HAL_GPIO_WritePin(LCD_PORT, pin, state);
	}
}

/**
  * @brief  Se activa el pin E para que funcione el módulo o no acepte ordenes de funcionamiento
  * @param  None
  * @retval None
  */
void LCD_Enable(void) {
	writePin(E, GPIO_PIN_SET);
	HAL_Delay(2);
	writePin(E, GPIO_PIN_RESET);
}

/**
  * @brief  Se encienden o apagan los pines necesarios segun el dato que le pasamos
  * @param  data: datos
  * @retval None
  */
void LCD_Output(uint8_t data) {
	if (data & 0x80)
		HAL_GPIO_WritePin(LCD_PORT, DB7, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_PORT, DB7, GPIO_PIN_RESET);
	if (data & 0x40)
		HAL_GPIO_WritePin(LCD_PORT, DB6, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_PORT, DB6, GPIO_PIN_RESET);
	if (data & 0x20)
		HAL_GPIO_WritePin(LCD_PORT, DB5, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_PORT, DB5, GPIO_PIN_RESET);
	if (data & 0x10)
		HAL_GPIO_WritePin(LCD_PORT, DB4, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_PORT, DB4, GPIO_PIN_RESET);
	if (data & 0x08)
		HAL_GPIO_WritePin(LCD_PORT, DB3, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_PORT, DB3, GPIO_PIN_RESET);
	if (data & 0x04)
		HAL_GPIO_WritePin(LCD_PORT, DB2, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_PORT, DB2, GPIO_PIN_RESET);
	if (data & 0x02)
		HAL_GPIO_WritePin(GPIOB, DB1, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOB, DB1, GPIO_PIN_RESET);
	if (data & 0x01)
		HAL_GPIO_WritePin(GPIOB, DB0, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOB, DB0, GPIO_PIN_RESET);
	LCD_Enable();
}

/**
  * @brief  Se envia el comando
  * @param  cmd: comando
  * @retval None
  */
void LCD_Command(uint8_t cmd) {
	writePin(RS, GPIO_PIN_RESET); //comando
	LCD_Output(cmd);
}

/**
  * @brief  Se envia el dato
  * @param  data: dato
  * @retval None
  */
void LCD_Data(char data) {
	writePin(RS, GPIO_PIN_SET); //caracter de datos
	LCD_Output(data);
}

/**
  * @brief  Se visualiza el string
  * @param  string: cadena de caracteres para visualizar
  * @retval None
  */
void LCD_Print_String(char string[]) {
	int i = 0;
	while (string[i] != '\0') {
		LCD_Data(string[i]);
		i++;
	}
}

/**
  * @brief  Se visualiza el string
  * @param  string: cadena de caracteres para visualizar
  * @retval None
  */
void LCD_Print_Char(char string) {
	LCD_Data(string & 0xff);
}

/**
  * @brief  Se visualiza el string
  * @param  string: cadena de caracteres para visualizar
  * @retval None
  */
void LCD_Print(char *string) {
	while (*string != '\0')
	{
		LCD_Data(*string++);
	}
}

/**
  * @brief  Se coloca el cursor en la linea que se ha pasado
  * @param  linea: numero de linea
  * @retval None
  */
void LCD_Cursor(int linea) {
	switch (linea) {
	case 1:
		LCD_Command(DDRAM | LINEA1);
		break;
	case 2:
		LCD_Command(DDRAM | LINEA2);
		break;
	case 3:
		LCD_Command(DDRAM | LINEA3);
		break;
	case 4:
		LCD_Command(DDRAM | LINEA4);
		break;
	default:
		break;
	}
}

/**
  * @brief Funcion para pasar de decimal a binario
  * @param  dec: numero decimal
  * @retval None
  */
void decToBin(uint32_t dec) {
	uint32_t tempa = dec;
	int resto,i;
	char binary[16] = "";
	for (i = 15; i >= 0; --i) {
		if (tempa != 0) {
			resto = tempa % 2;
			tempa = tempa / 2;
			binary[i] = resto +'0';
		} else {
			binary[i] = '0';
		}
	}
	binary[16] = '\0';
	LCD_Print(binary);
}

/**
  * @brief Funcion para pasar de int de 4 digitos a string
  * @param  val: valor
  * @retval None
  */
void intToString4(uint32_t val) {
	char string[5] = "";
	string[0] =	(val/1000) +'0';
	string[1] = ((val%1000)/100) +'0';
	string[2] = (((val%1000)%100)/10) +'0';
	string[3] = (((val%1000)%100)%10) +'0';
	string[4] = '\0';
	LCD_Print_String(string);
}

/**
  * @brief Funcion para pasar de int de 3 digitos a string
  * @param  val: valor
  * @retval None
  */
void intToString3(uint32_t val) {
	char string[4] = "";
	string[0] =	(val/100) +'0';
	string[1] = ((val%100)/10) +'0';
	string[2] = ((val%100)%10) +'0';
	string[3] = '\0';
	LCD_Print_String(string);
}

/**
  * @brief Funcion para crear caracteres personalizados
  * @param  loc: posicion, p: caracter
  * @retval None
  */
void LCD_BuildChar(uint16_t loc, unsigned char *p)
 {
	unsigned char i;

	LCD_Command(0x40 + (loc * 8));    //Posicion de memoria de CGRAM
	for (i = 0; i < 8; i++) {
		LCD_Data(p[i]);               //Escribir el caracter
	}
	LCD_Command(0x80);   //Posicion de la LCD
}

/**
  * @brief  Inicializacion del LCD
  * @param  None
  * @retval None
  */
void LCD_Init(void) {

	/* RS = 0; */
	writePin(RS, GPIO_PIN_RESET);
	/* E = 0;  */
	writePin(E, GPIO_PIN_RESET);

	LCD_Command(0x30);
	HAL_Delay(17);
	LCD_Command(0x30);
	HAL_Delay(5);
	LCD_Command(0x30);
	HAL_Delay(3);
	LCD_Command(LCD_SET_FUNCTION | LCD_8BIT | LCD_2LINE | LCD_58);
	HAL_Delay(2);
	LCD_Command(LCD_DISPLAY_OFF);
	HAL_Delay(2);
	LCD_Command(LCD_CLEAR_DISPLAY);
	HAL_Delay(3);
	LCD_Command(LCD_SET_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_BLINKING_ON);
	HAL_Delay(3);
	LCD_Command(LCD_SET_ENTRY | LCD_ENTRY_INCREASE | LCD_ENTRY_NOSHIFT);
	HAL_Delay(3);
	LCD_Command(LCD_SET_SHIFT | LCD_CURSOR_MOVE | LCD_SHIFT_RIGHT);

}

