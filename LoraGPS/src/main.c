/*
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "hw.h"
#include "radio.h"
#include "timeServer.h"
#include "delay.h"
#include "low_power.h"
#include "vcom.h"
#include "lora.h"
#include "version.h"
#include "gps.h"
#include "display.h"

/** Lora **/

/*!
 * Defines the application data transmission duty cycle. 5s, value in [ms].
 */
#define APP_TX_DUTYCYCLE                            10000
/*!
 * LoRaWAN Adaptive Data Rate
 * @note Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON                              1
/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG                    DISABLE
/*!
 * LoRaWAN application port
 * @note do not use 224. It is reserved for certification
 */
#define LORAWAN_APP_PORT                            2

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* call back when LoRa will transmit a frame*/
static void LoraTxData(lora_AppData_t *AppData, FunctionalState* IsTxConfirmed);

/* call back when LoRa has received a frame*/
static void LoraRxData(lora_AppData_t *AppData);

/* Private variables ---------------------------------------------------------*/
/* load call backs*/
static LoRaMainCallback_t LoRaMainCallbacks = { HW_GetBatteryLevel,
		HW_GetUniqueId, HW_GetRandomSeed, LoraTxData, LoraRxData };

/* !
 *Initialises the Lora Parameters
 */
static LoRaParam_t LoRaParamInit = { TX_ON_TIMER,
APP_TX_DUTYCYCLE, CLASS_A,
LORAWAN_ADR_ON,
DR_0,
LORAWAN_PUBLIC_NETWORK };

/**
 * Main application entry point.
 */

extern UART_HandleTypeDef huart1;
__IO ITStatus UartReady = RESET;

#if defined( USE_BAND_868 )

#define RF_FREQUENCY                                868000000 // Hz

#else
    #error "Please define a frequency band in the compiler options."
#endif

#define TX_OUTPUT_POWER                             14        // dBm

#if defined( USE_MODEM_LORA )

#define LORA_BANDWIDTH                              0         	// [0: 125 kHz,
																//  1: 250 kHz,
																//  2: 500 kHz,
																//  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         	// [SF7..SF12]
#define LORA_CODINGRATE                             1         	// [1: 4/5,
																//  2: 4/6,
																//  3: 4/7,
																//  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         	// Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         	// Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#else
    #error "Please define a modem in the compiler options."
#endif

typedef enum {
	LOWPOWER, RX, RX_TIMEOUT, RX_ERROR, TX, TX_TIMEOUT,
} States_t;

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 64 // Define the payload size here
uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

States_t State = LOWPOWER;

int8_t RssiValue = 0;
int8_t SnrValue = 0;

/* Private function prototypes -----------------------------------------------*/
/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void);

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout(void);

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout(void);

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError(void);

/* SPI handler declaration */
extern SPI_HandleTypeDef hspi2;

/* Buffer used for transmission */
#define BUFFERSIZE                       40
//#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Buffer used for transmission */
uint8_t aTxBuffer[] =" ";

/* Buffer used for reception */
uint8_t buffLora[40];
char buffGPS[80];
uint8_t RxReady[5];
uint8_t parsingBuff[BUFFERSIZE];
//uint8_t parsingBuff[];
uint8_t BuffDatos[BUFFERSIZE];

uint8_t ReadyMsg[] = "READY";
uint8_t OKMsg[2] = "OK";

extern UART_HandleTypeDef huart1;
//__IO ITStatus UartReady = RESET;

int recibidoMaster = 0;
int recibidoSlave = 0;
int recibidoReady = 0;
int enviadoReady = 0;
int errorReady = 0;
int transmite = 0;
int estado = 0;
int i = 0;

char hora[8];
char lat[10];
char latC[1];
char lon[10];
char lonC[1];
char* a;
char ReadyID[6];

int ID;
char IDLora[1];

int IDSlave;
char IDSlaveLora[1];

struct datosMicro {
	char datos[100];
};

struct datosMicro misDat[150];
int dat = 0;

uint8_t init_nmea[] = "$PSRF100,0,9600,8,1,0*04\r\n";
uint8_t init_idle[] = "$PSRF150,1*3E\r\n";
uint8_t init_GGA[] = "$PSRF103,00,00,01,01*25\r\n";
uint8_t deinit_GGA[] = "$PSRF103,00,00,00,01*25\r\n";
uint8_t init_GLL[] = "$PSRF103,01,00,01,01*24\r\n";
uint8_t deinit_GLL[] = "$PSRF103,01,00,00,01*25\r\n";
uint8_t init_GSA[] = "$PSRF103,02,00,01,01*27\r\n";
uint8_t deinit_GSA[] = "$PSRF103,02,00,00,01*26\r\n";
uint8_t init_GSV[] = "$PSRF103,03,00,05,01*22\r\n";
uint8_t deinit_GSV[] = "$PSRF103,03,00,00,01*27\r\n";
uint8_t init_RMC[] = "$PSRF103,04,00,01,01*21\r\n";
uint8_t deinit_RMC[] = "$PSRF103,04,00,00,01*20\r\n";
uint8_t init_VTG[] = "$PSRF103,05,00,01,01*20\r\n";
uint8_t deinit_VTG[] = "$PSRF103,05,00,00,01*21\r\n";

int main(void) {

	HAL_Init();
	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	SystemClock_Config();

	DBG_Init();
	HW_Init();

	LCD_Config();
	LCD_Init();
	LCD_Command(LCD_CLEAR_DISPLAY);

	SPI_Config();
	SPI_Init();

	PRINTF("VERSION: %X\n\r", VERSION);

//	 Radio initialization
	RadioEvents.TxDone = OnTxDone;
	RadioEvents.RxDone = OnRxDone;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = OnRxTimeout;
	RadioEvents.RxError = OnRxError;

	Radio.Init(&RadioEvents);

	Radio.SetChannel( RF_FREQUENCY);

#if defined( USE_MODEM_LORA )

	Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
	LORA_SPREADING_FACTOR, LORA_CODINGRATE,
	LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
	true, 0, 0, LORA_IQ_INVERSION_ON, 3000000);

	Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
	LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
	LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON, 0, true, 0, 0,
	LORA_IQ_INVERSION_ON, true);
#endif

	//Establece la radio en modo de recepcion durante un tiempo
	Radio.Rx( RX_TIMEOUT_VALUE);

	/* Master */
	bool isMaster = true;

	ID = 0;
	sprintf(IDLora,"%d", ID);
	IDSlave = ID+1;
	sprintf(IDSlaveLora,"%d", IDSlave);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(&hspi2, (uint8_t *) init_nmea, sizeof(init_nmea), 2000) == HAL_OK) {
		while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
		}
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(&hspi2, (uint8_t *) init_idle, sizeof(init_idle), 2000) == HAL_OK) {
		while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
		}
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);


	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(&hspi2, (uint8_t *) init_GGA, sizeof(init_GGA), 2000) == HAL_OK) {
		while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
		}
		PRINTF("Recibido Ready\r\n");
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(&hspi2, (uint8_t *) init_RMC, sizeof(init_RMC), 2000) == HAL_OK) {
		while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
		}
		PRINTF("Recibido Ready\r\n");
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	while (1) {


//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
//		if (HAL_SPI_Transmit(&hspi2, (uint8_t *) init_GGA, 26, 2000) == HAL_OK) {
//			while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
//			}
//			PRINTF("Recibido Ready\r\n");
//		}
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);


		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		if (HAL_SPI_Receive(&hspi2, (uint8_t*) buffGPS, 77, 500) == HAL_OK) {
			PRINTF("%s\r\n", buffGPS);
		}
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

//		memset(buffGPS, '\0', 80);

//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
//		if (HAL_SPI_Receive_IT(&hspi2, (uint8_t*) buffGPS, 80) == HAL_OK) {
//			PRINTF("%s\r\n", buffGPS);
//		}
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);


//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
//		if (HAL_SPI_TransmitReceive(&hspi2, (uint8_t*) init_GGA, (uint8_t*) buffGPS, 80, 2000) == HAL_OK) {
//			while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
//			}
//			PRINTF("Recibido Ready\r\n");
//		}
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

//		if (recibidoReady == 0) {
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
//			if (HAL_SPI_TransmitReceive(&hspi2, (uint8_t*) ReadyMsg, (uint8_t *) RxReady, 5, 2000) == HAL_OK) {
//				while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
//				}
//				if (strncmp((const char*) RxReady, (const char*) ReadyMsg, 5) == 0) {
//					recibidoReady = 1;
//					PRINTF("Recibido Ready\r\n");
//				}
//			}
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
//		} else if (recibidoReady == 1) {
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
//			if (HAL_SPI_TransmitReceive(&hspi2, (uint8_t*) OKMsg, (uint8_t *) parsingBuff, 40, 2000) == HAL_OK) {
//				while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
//				}
//				strncpy(BuffDatos, parsingBuff + 1, 38);
//				strcpy(misDat[i].datos, BuffDatos);
////				strcpy(misDat[i].datos, parsingBuff);
//				if (strncmp((const char*) BuffDatos, (const char*) "\nGPS", 4)	== 0) {
//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
//					if (HAL_SPI_TransmitReceive(&hspi2, (uint8_t*) BuffDatos,(uint8_t *) OKMsg, 40, 2000) == HAL_OK) {
//						while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
//						}
//					}
//				}
//			}
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
//		}

//		switch (State) {
//		case RX:
//			if (isMaster == true) {
//				if (BufferSize > 0) {
////					PRINTF(" Master: %s\r\n", Buffer);
//					if ((strncmp((const char*) Buffer, (const char*) ReadyMsg, 5) == 0) && Buffer[5] == IDSlaveLora[0]) {
//						DelayMs(1);
//						PRINTF(" Master: %s\r\n", Buffer);
//						Radio.Send(misDat[i].datos, BUFFERSIZE);
//						enviadoReady = 1;
//						recibidoMaster = 1;
//						errorReady = 1;
//					}
//					if ((recibidoMaster == 1) && (strncmp((const char*) Buffer,(const char*) OKMsg, 2) == 0) && Buffer[2] == IDSlaveLora[0]) {
//						DelayMs(1);
//						PRINTF(" Master: %s\r\n", Buffer);
//						Radio.Send(misDat[i].datos, BUFFERSIZE);
//					}
//					Radio.Rx( RX_TIMEOUT_VALUE);
//					memset(Buffer, '\0', BUFFER_SIZE);
//				}
//			}
//			State = LOWPOWER;
//			break;
//		case TX:
//			Radio.Rx( RX_TIMEOUT_VALUE);
//			State = LOWPOWER;
//			break;
//		case RX_TIMEOUT:
//		case RX_ERROR:
//			if (isMaster == true) {
//				if (enviadoReady == 0) {
//					sprintf(ReadyID, "%s%d", ReadyMsg, ID);
//					Radio.Send(ReadyID, 6);
//					PRINTF("Master Ready\r\n");
//				}
//				if (errorReady == 1) {
//					Radio.Send(misDat[i].datos, BUFFERSIZE);
//				}
//				// Send the next PING frame
//				DelayMs(1);
//				Radio.Rx( RX_TIMEOUT_VALUE);
//			} else {
//				Radio.Rx( RX_TIMEOUT_VALUE);
//			}
//			State = LOWPOWER;
//			break;
//		case TX_TIMEOUT:
//			Radio.Rx( RX_TIMEOUT_VALUE);
//			break;
//		case LOWPOWER:
//		default:
//			// Set low power
//			break;
//		}
//		i++;
//		if (i == 149){
//			i = 0;
//		}
//
//		DISABLE_IRQ( );
//		/* if an interupt has occured after __disable_irq, it is kept pending
//		 * and cortex will not enter low power anyway  */
//		if (State == LOWPOWER) {
//#ifndef LOW_POWER_DISABLE
//			LowPower_Handler();
//#endif
//		}
//		ENABLE_IRQ( );
		DelayMs(1000);
	}
}

//La llamada  PRINTF("txDone\n\r"); esta en sx1276.c, funcion SX1276OnDio0Irq
void OnTxDone(void) {
	Radio.Sleep();
	State = TX;
}

//La llamada PRINTF("rxDone\n\r"); esta en sx1276.c, funcion SX1276OnDio0Irq
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
	Radio.Sleep();
	BufferSize = size;
	memcpy(Buffer, payload, BufferSize);
	RssiValue = rssi;
	SnrValue = snr;
	State = RX;
}

void OnTxTimeout(void) {
	Radio.Sleep();
	State = TX_TIMEOUT;

//	PRINTF("OnTxTimeout\n");
}

void OnRxTimeout(void) {
	Radio.Sleep();
	State = RX_TIMEOUT;
//	PRINTF("OnRxTimeout\n");
}

void OnRxError(void) {
	Radio.Sleep();
	State = RX_ERROR;
//	PRINTF("OnRxError\n");
}
