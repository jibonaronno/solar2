/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "atcommands.h"
#include "modbusmaster.h"
#include "tm_onewire.h"
#include "trex.h"

TRex *trex;

extern atcmd_t atcmdtable[];
extern atcmd_t atcmdsms[];
extern MBUSPACDEF mbuspac[2];

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ONLINE_LED_ON			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET)
#define ONLINE_LED_OFF		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET)
#define CONNECT_LED_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define CONNECT_LED_OFF		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define ERROR_LED_ON			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
#define ERROR_LED_OFF			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint8_t flash[2048];

UART_HandleTypeDef *huart;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void UX_GPIO_Init(void);

volatile uint8_t flag_uart1_rx = 0;
volatile uint8_t flag_uart2_rx = 0;
volatile uint8_t flag_uart5_rx = 0;
volatile uint8_t flag_uart4_rx = 0;

uint32_t uart1_drip_counter = 0;
uint32_t uart2_drip_counter = 0;
uint32_t uart4_drip_counter = 0;
uint32_t uart5_drip_counter = 0;

int systick_clock_01 = 0;
uint8_t flag_systick_01 = 0;
char rx1buff[20];
char Rx1buff[200];
char rx1buffindex = 0;

char rx5buff[20];
char Rx5buff[200];
char rx5buffindex = 0;

char rx4buff[20];
char Rx4buff[200];
char rx4buffindex = 0;

char rx2buff[20];
char Rx2buff[200];
char rx2buffindex = 0;
char strSunspec[400];

volatile int mb_timeout_counter = 3000; //modbus timeout counter
volatile int mbus_cycle_count = 0;

volatile uint8_t flag_atcommand_responded = 0;
volatile int at_timeout_counter = 0;
volatile int atcmd_idx = 0;
volatile uint8_t flag_ego = 0;

int dccurrentob1 = 0;
int dckwhout1 = 0;
int dcinob1 = 0;
int dcoutob1 = 0;

int dccurrentob2 = 0;
int dckwhout2 = 0;
int dcinob2 = 0;
int dcoutob2 = 0;

int dccurrentob = 0;
int dckwhout = 0;
int dcinob = 0;
int dcoutob = 0;
int ackwhsun = 0;
int alarmsun = 0;
int inpower = 0;
int pv1volt = 0;
int pv2volt = 0;
int avolt = 0;
int bvolt = 0;
int cvolt = 0;
int acur = 0;
int bcur = 0;
int ccur = 0;
int alarm1 = 0;
int alarm2 = 0;
int alarm3 = 0;
char strGet01[500];

int Outback_Rx_watchdog_counter = 0;
uint16_t Outback_PIN = GPIO_PIN_6;
GPIO_TypeDef *Outback_PORT = GPIOC;
uint16_t OutbackIdx = 0;
volatile int flag_lock = 0;

int Retry_Timeout_Counter = 280000;

int obcounter = 0;

unsigned char pData[] = {0x1A, 0x00};

int mbus_index = 0;
int mbus_index02 = 0;

struct __FILE
{
  int dummy;
};

FILE __stdout;

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  HAL_UART_Transmit(huart, (uint8_t*) &ch, 1, 0x0fff);
  /* Loop until the end of transmission */
  //while (USART_GetFlagStatus(Port_USART, USART_FLAG_TC) == RESET)
  //{}

  return ch;
}

#define OPMODE_MODEM			1
#define OPMODE_MODBUS			2
#define OPMODE_OUTBACK		3
#define OPMODE_CHECKSMS		4
#define OPMODE_PAUSE			8

IWDG_HandleTypeDef   IwdgHandle;

INT_ARRAY time_list = {{{94,0},{20,1},{31,0},{10,1},{62,0},{10,1},{104,0},{10,1},{10,0},{10,1},{83,0},{10,1},{104,0},{10,1},{10,0},{83,1},{10,0},{10,1},{10,0},{10,1},{83,0},{10,1},{31,0},{10,1},{62,0},{2000,1},{94,0},{20,1},{10,0},{20,1},{72,0},{10,1},{10,0},{10,1},{83,0},{10,1},{10,0},{10,1},{20,0},{10,1},{20,0},{20,1},{10,0},{10,1},{104,0},{10,1},{10,0},{20,1},{52,0},{10,1},{10,0},{10,1},{10,0},{10,1},{83,0},{10,1},{52,0},{10,1},{10,0},{10,1},{20,0},{1,1}}, 62};
INT_ARRAY time_list2 = {{{94,0},{20,1},{10,0},{20,1},{72,0},{10,1},{10,0},{10,1},{83,0},{10,1},{10,0},{10,1},{20,0},{10,1},{20,0},{20,1},{10,0},{10,1},{104,0},{10,1},{10,0},{20,1},{52,0},{10,1},{10,0},{10,1},{10,0},{10,1},{83,0},{10,1},{52,0},{10,1},{10,0},{10,1},{20,0},{1,1}}, 36};	

int t2tick = 0;
int time_list_idx = 0;
int t_delay = 500;
int tidx = 0;
	
SETTINGS *settings;

void GenerateXor(SETTINGS *set)
{
	
}

/***********************************/
/*		NO NEED THIS FUNCTION				*/
/***********************************/
void MemSet(void *str, size_t n)
{
	memset(str, 0, n);
}

void StoreIp(char *str, char *addr)
{
	char *padle;
	char *needle;
	int lidx01 = 0;
	//char buf01[150];
	char *eptr;
	char ip_1[10];
	int ip1 = 0;
	int ip2 = 0;
	int ip3 = 0;
	int ip4 = 0;
	
	char ipaddr[30];
	
	/*
	strcpy(addr, str);
	addr[3] = 0;
	ip1 = strtol(addr, &eptr, 10);
	addr[7] = 0;
	ip2 = strtol(&addr[4], &eptr, 10);
	addr[11] = 0;
	ip3 = strtol(&addr[8], &eptr, 10);
	addr[15] = 0;
	ip4 = strtol(&addr[12], &eptr, 10);
	sprintf(addr, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	*/
	
	eraseSector(0x8012000);
	HAL_Delay(100);
	writeSector(0x8012000, (void *)str, 17);
	HAL_Delay(100);
	
	strcpy(addr, str);
	addr[3] = 0;
	ip1 = strtol(addr, &eptr, 10);
	addr[7] = 0;
	ip2 = strtol(&addr[4], &eptr, 10);
	addr[11] = 0;
	ip3 = strtol(&addr[8], &eptr, 10);
	addr[15] = 0;
	ip4 = strtol(&addr[12], &eptr, 10);
	sprintf(addr, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char *padle;
	char *needle;
	int lidx01 = 0;
	//char buf01[150];
	char *eptr;
	char ip_1[10];
	int ip1 = 0;
	int ip2 = 0;
	int ip3 = 0;
	int ip4 = 0;
	
	char addr[30];
	
	char ipaddr[30];
	
	uint8_t flag_operation_mode = OPMODE_CHECKSMS; // OPMODE_MODBUS;

  /* USER CODE END 1 */
  

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	
	IwdgHandle.Instance = IWDG;

  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_128;
  IwdgHandle.Init.Reload = 0x0AAA;
	
	if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  //MX_TIM2_Init();
  MX_USART2_UART_Init();
  //MX_IWDG_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	
	UX_GPIO_Init();
	
	huart = &huart1;
	printf("Starting Code ... \r\n");
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)rx1buff, 1);
	
	HAL_UART_Receive_IT(&huart2, (uint8_t *)rx2buff, 1);
	
	HAL_UART_Receive_IT(&huart5, (uint8_t *)rx5buff, 1);
	
	HAL_UART_Receive_IT(&huart4, (uint8_t *)rx4buff, 1);
	
	at_timeout_counter = 20000;
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	HAL_TIM_Base_Start_IT(&htim6);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	
	HAL_UART_Transmit(&huart4, (unsigned char*)"AT+CGMM\r\n", lidx01, 0x0fff);
	
	/*--------------------------------------------------------------------------*/
	//READ STORED DATA FROM FLASH LOCATION 0x8012000 . From hex, end of program location is around 0x80033A0
	
	readSector(0x8012000, (void *)ipaddr, LSIZE);
	
	huart = &huart1;
	printf("GETTING IP FROM FLASH -----------------------------\r\n\r\n");
	
	strncpy(ip_1, ipaddr, 3);
	
	ip1 = strtol(ip_1, &eptr, 10);
	
	printf("IP CONVERT -----------------------------\r\n\r\n");
	
	if((ip1 > 255) || (ip1 < 10))
	{
		eraseSector(0x8012000);
		HAL_Delay(100);
		strncpy((char *)ipaddr, "036.255.068.127", 15);
		printf("strncpy -----------------------------\r\n\r\n");
		writeSector(0x8012000, (void *)ipaddr, LSIZE);
		HAL_Delay(100);
	}
	
	huart = &huart1;
	printf("GETTING IP-----------------------------\r\n\r\n");
	
	strcpy(addr, ipaddr);
	addr[3] = 0;
	ip1 = strtol(addr, &eptr, 10);
	addr[7] = 0;
	ip2 = strtol(&addr[4], &eptr, 10);
	addr[11] = 0;
	ip3 = strtol(&addr[8], &eptr, 10);
	addr[15] = 0;
	ip4 = strtol(&addr[12], &eptr, 10);
	
	sprintf(addr, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	
	huart = &huart1;
	printf("STORED IP : %s", addr);
	
	sprintf(atcmdtable[10].cmd, "AT+CIPSTART=\"TCP\",\"%s\",\"80\"\r\n", addr);
	
	/*
	trex = trex_compile("d{1,3}\\.d{1,3}\\.d{1,3}\\.d{1,3}");
	
	if(trex_match(trex, "10.55.203.91") == TRex_True)
	{
		huart = &huart1;
		printf("REGEX MATCHED");
	}
	else
	{
		huart = &huart1;
		printf("REGEX NOT MATCHED");
	}
	*/
	//if(settings->server_address
	
  while (1)
  {
		if(flag_systick_01 == 1)
		{
			systick_clock_01 = 0;
			flag_systick_01 = 0;
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_14);
			HAL_IWDG_Refresh(&IwdgHandle);
		}
		
		if(Retry_Timeout_Counter == 0)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
			ERROR_LED_OFF;
			CONNECT_LED_OFF;
			ONLINE_LED_OFF;
			HAL_Delay(10000); //Watch Dog Reset
			Retry_Timeout_Counter = 240000;
			at_timeout_counter = 20000;
			atcmd_idx = 0;
		}
		
		/*****************************************************************************/
		/*						MAIN STATE MACHINE START																			 */
		/*****************************************************************************/
		
		if(flag_operation_mode == OPMODE_MODEM)
		{
			//(flag_atcommand_responded == 1)
			if(at_timeout_counter == 0)
			{
				if(flag_atcommand_responded == 1)
				{
					if(atcmd_idx < 15)
					{
						atcmd_idx++;
						
						if(strstr(atcmdtable[atcmd_idx].cmd, "^Z"))
						{
							//huart = &huart1;
							//printf("SENDING CTRL+Z\r\n");
							//HAL_Delay(500);
							//huart = &huart4;
							//printf("%c",0x1A);
							//HAL_UART_Transmit(huart, pData, 1, 0);
						}
						else if(strlen(atcmdtable[atcmd_idx].cmd) > 0)
						{
							if(strstr(atcmdtable[atcmd_idx].ret, "^Z"))
							{
								
								/*********************************************************/
								/*		GATHER OUTBACK AND SUN DATA 											**/
								/*********************************************************/
								//sprintf(strGet01, "%s", "GET /gateway/pinlog.php");
								//sprintf(strGet01, "%s?serial=00000003322&imei=864713033803712&ccid=UDU2013072502&siteid=MYTRL18&devid=0x00000003&cellno=+8801608433197&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003324&imei=864764038322646&ccid=UDU2013072602&siteid=MYTRL12&devid=0x00000005&cellno=+8801825327740&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003326&imei=864764038322646&ccid=UDU2013072602&siteid=PTECH01&devid=0x00000006&cellno=+8801825327740&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003327&imei=864764038322647&ccid=UDU2014072603&siteid=PTECH02&devid=0x00000007&cellno=+8801825327740&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003328&imei=864713033806749&ccid=UDU2014072604&siteid=SITE00000005&devid=0x00000008&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003329&imei=864764039605221&ccid=UDU2014072605&siteid=SITE00000006&devid=0x00000009&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003330&imei=867858032270095&ccid=UDU2014072607&siteid=SITE00000007&devid=0x0000000A&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003331&imei=864764034145660&ccid=UDU2014072608&siteid=SITE00000008&devid=0x0000000B&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003332&imei=864713039870327&ccid=UDU2014072609&siteid=SITE00000009&devid=0x0000000C&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003333&imei=864369031283603&ccid=UDU2014072610&siteid=SITE00000010&devid=0x0000000D&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003334&imei=864764034451605&ccid=UDU2014072611&siteid=SITE00000011&devid=0x0000000E&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003335&imei=864713039889947&ccid=UDU2014072612&siteid=SITE00000012&devid=0x0000000F&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /gateway/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								//sprintf(strGet01, "%s?serial=00000003326&imei=864764038322646&ccid=UDU2013072602&siteid=PTECH01&devid=0x00000006&cellno=+8801825327740&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n", "GET /rms/solarrms/gateway_bonni/pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								
								/****************************************************************************************/
								/*							CLOUD @ 36.255.68.127/pinlog.php						*/
								sprintf(strGet01, "%s?serial=00000003332&imei=864713039870327&ccid=UDU2014072609&siteid=SITE00000009&devid=0x0000000C&cellno=+8800000000000&dccurrentob=%d&dckwhout=%d&dcinob=%d&dcoutob=%d&dccurrentob1=%d&dckwhout1=%d&dcinob1=%d&dcoutob1=%d&dccurrentob2=%d&dckwhout2=%d&dcinob2=%d&dcoutob2=%d&ackwhsun=%d&inpower=%d&alarm1=%d&alarm2=%d&alarm3=%d&pv1volt=%d&pv2volt=%d&avolt=%d&bvolt=%d&cvolt=%d&acur=%d&bcur=%d&ccur=%d\r\n\r\n", "GET /pinlog.php", dccurrentob, dckwhout, dcinob, dcoutob, dccurrentob1, dckwhout1, dcinob1, dcoutob1, dccurrentob2, dckwhout2, dcinob2, dcoutob2, ackwhsun, inpower, alarm1, alarm2, alarm3, pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur);
								
								//sprintf(strGet01, "%s\r\n\r\n", "GET /robi/pinlog.php");
								
								lidx01 = strlen(strGet01);
								strGet01[lidx01] = 0x1A;
								lidx01++;
								strGet01[lidx01] = 0;
								
								huart = &huart1;
								printf("-->> %s", strGet01);
								HAL_Delay(1500);
								
								////<TEMPORARY COMMENTED>
								////Retry_Timeout_Counter = 240000;

								//huart = &huart4;
								//lidx01 = strlen(atcmdtable[atcmd_idx].cmd);
								//strcpy(buf01, atcmdtable[atcmd_idx].cmd);
								//buf01[lidx01] = 0x1A;
								//buf01[lidx01+1] = 0;
								//lidx01++;
								//printf("%s",atcmdtable[atcmd_idx].cmd);
								HAL_UART_Transmit(&huart4, (unsigned char*)strGet01, lidx01, 0x0fff);
								HAL_Delay(1500);
								huart = &huart1;
								printf("SENDING DATA + CTRL+Z\r\n");
								
								flag_operation_mode = OPMODE_MODBUS;
							}
							else
							{
								huart = &huart4;
								printf("%s",atcmdtable[atcmd_idx].cmd);
							}
							
							HAL_Delay(200);
							//huart = &huart1;
							//printf("%s -- %s\r\n",atcmdtable[atcmd_idx].cmd, atcmdtable[atcmd_idx].ret);
							HAL_Delay(200);
						}
						flag_atcommand_responded = 0;
						at_timeout_counter = atcmdtable[atcmd_idx].timeout * 1000;
					} 
					else
					{
						if(atcmd_idx > 9)
						{
							atcmd_idx = 9;
						}
					}
				}
				else //if(flag_atcommand_responded != 0) and timeout happens.
				{
					if(atcmd_idx == 0) //Condition After Start
					{
						flag_atcommand_responded = 1;
						atcmd_idx = -1;
					}
					else
					{
						if(flag_ego == 1)
						{
							flag_ego = 0;
							flag_atcommand_responded = 1;
						}
						else
						{
							flag_atcommand_responded = 1;
							atcmd_idx = 9;
							at_timeout_counter = 20000;
						}
					}
				}
			}
		}
		else if(flag_operation_mode == OPMODE_MODBUS)
		{
			
			if(mb_timeout_counter == 0)
			{
				
				if(mbus_index == 0)
				{
					pv1volt = 0;
					pv2volt = 0;
					avolt = 0;
					bvolt = 0;
					cvolt = 0;
					acur = 0;
					bcur = 0;
					ccur = 0;
					alarm1 = 0;
					alarm2 = 0;
					alarm3 = 0;
					ackwhsun = 0;
					
				}
				
				HAL_UART_Transmit_IT(&huart2, (uint8_t *)mbuspac[mbus_index].txdata, 8);
				HAL_Delay(200);
				huart = &huart1;
				printf("%d :: %02X %02X %02X %02X %02X %02X %02X %02X\r\n", mbus_index, mbuspac[mbus_index].txdata[0], mbuspac[mbus_index].txdata[1], mbuspac[mbus_index].txdata[2], mbuspac[mbus_index].txdata[3], mbuspac[mbus_index].txdata[4], mbuspac[mbus_index].txdata[5], mbuspac[mbus_index].txdata[6], mbuspac[mbus_index].txdata[7]);
				
				//HAL_UART_Transmit_IT(&huart1, (uint8_t *)mbuspac[mbus_index].txdata, 8);
				//HAL_UART_Transmit_IT(&huart1, (uint8_t *)"\r\n", 1);
				mb_timeout_counter = 800;
				
				mbus_cycle_count++;
				
				if(mbus_index < 12)
				{
					/** INCREMENT MODBUS COMMAND INDEX **/
					mbus_index++;
				}
				else
				{
					if(mbus_cycle_count > 12)
					{
						mbus_cycle_count = 0;
						mbus_index = 0;
						flag_operation_mode = OPMODE_MODEM;
					}
				}
			}
			
				/****************************************************/
				/* DATA CAME FROM SUNSPEC														*/
				/****************************************************/
				if(flag_uart2_rx == 1)
				{
					if(mbus_index > 0)
					{
						mbus_index02 = mbus_index - 1;
					}
					else
					{
						mbus_index02 = 12; //maximum index
					}
					
					flag_uart2_rx = 0;
					if(mbuspac[mbus_index02].cmd == VOLT_PV1_CMD)
					{
						pv1volt = (Rx2buff[3] << 8)|(Rx2buff[4]);
					}
					
					if(mbuspac[mbus_index02].cmd == VOLT_PV2_CMD)
					{
						pv2volt = (Rx2buff[3] << 8)|(Rx2buff[4]);
					}
					
					if(mbuspac[mbus_index02].cmd == VOLT_A_CMD)
					{
						avolt = (Rx2buff[3] << 8)|(Rx2buff[4]);
					}
					
					if(mbuspac[mbus_index02].cmd == VOLT_B_CMD)
					{
						bvolt = (Rx2buff[3] << 8)|(Rx2buff[4]);
					}
					
					if(mbuspac[mbus_index02].cmd == VOLT_C_CMD)
					{
						cvolt = (Rx2buff[3] << 8)|(Rx2buff[4]);
					}
					
					if(mbuspac[mbus_index02].cmd == ALARM1_CMD)
					{
						alarm1 = (Rx2buff[3] << 8)|(Rx2buff[4]);
					}
					
					if(mbuspac[mbus_index02].cmd == ALARM2_CMD)
					{
						alarm2 = (Rx2buff[3] << 8)|(Rx2buff[4]);
					}
					
					if(mbuspac[mbus_index02].cmd == ALARM3_CMD)
					{
						alarm3 = (Rx2buff[3] << 8)|(Rx2buff[4]);
					}
					
					if(mbuspac[mbus_index02].cmd == CURR_A_CMD)
					{
						acur = (Rx2buff[3] << 24)|(Rx2buff[4] << 16) | (Rx2buff[5] << 8) | Rx2buff[6];
					}
					
					if(mbuspac[mbus_index02].cmd == CURR_B_CMD)
					{
						bcur = (Rx2buff[3] << 24)|(Rx2buff[4] << 16) | (Rx2buff[5] << 8) | Rx2buff[6];
					}
					
					if(mbuspac[mbus_index02].cmd == CURR_C_CMD)
					{
						ccur = (Rx2buff[3] << 24)|(Rx2buff[4] << 16) | (Rx2buff[5] << 8) | Rx2buff[6];
					}
					
					if(mbuspac[mbus_index02].cmd == APOWER_CMD)
					{
						ackwhsun = (Rx2buff[3] << 24)|(Rx2buff[4] << 16) | (Rx2buff[5] << 8) | Rx2buff[6];
						//sprintf(strSunspec, "PV1-%d PV2-%d Avolt-%d Bvolt-%d Cvolt-%d Acur-%d Bcur-%d Ccur-%d Alrm1-%d Alrm2-%d Alrm3-%d Activepower-%d", pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur, alarm1, alarm2, alarm3, ackwhsun);
						
						////<TEMPORARY COMMENTED>
						////flag_operation_mode = OPMODE_MODEM;
					}
					
					HAL_Delay(100);
					//HAL_UART_Transmit(&huart1, (uint8_t *)strSunspec, strlen(strSunspec), 0x0FFF);
					sprintf(strSunspec, "PV1-%d PV2-%d Avolt-%d Bvolt-%d Cvolt-%d Acur-%d Bcur-%d Ccur-%d Alrm1-%d Alrm2-%d Alrm3-%d Activepower-%d", pv1volt, pv2volt, avolt, bvolt, cvolt, acur, bcur, ccur, alarm1, alarm2, alarm3, ackwhsun);
					huart = &huart1;
					printf("%d | %s\r\n", mbus_index02, strSunspec);
					HAL_Delay(50);
					//huart = &huart2;
					//printf("%s", Rx2buff);
					rx2buffindex = 0;
					
					//else
					//{
						//mbus_index = 0;
						//mb_timeout_counter = 800;
					//}
					
				}
				/*****************************************************/
		} // END OF OPMODE_MODBUS 
		else if(flag_operation_mode == OPMODE_CHECKSMS)
		{
			if(at_timeout_counter == 0)
			{
				if(atcmd_idx < 4)
				{
					atcmd_idx++;
					
					huart = &huart4;
					printf("%s",atcmdsms[atcmd_idx].cmd);
					
					flag_atcommand_responded = 0;
					at_timeout_counter = atcmdtable[atcmd_idx].timeout * 1000;
				}
				else
				{
					flag_atcommand_responded = 0;
					at_timeout_counter = 20000;
					atcmd_idx = -1;
					flag_operation_mode = OPMODE_MODEM;
				}
			}
		}
		else if(flag_operation_mode == OPMODE_PAUSE)
		{
			
		}
		
		/*****************************************************************************/
		/*						MAIN STATE MACHINE END  																			 */
		/*****************************************************************************/
		
		if(Outback_Rx_watchdog_counter == 0)
		{
			flag_lock = 0;
			
			dckwhout = 0;
			dcinob = 0;
			dcoutob = 0;
			dccurrentob = 0;
			
			dckwhout1 = 0;
			dcinob1 = 0;
			dcoutob1 = 0;
			dccurrentob1 = 0;
			
			dckwhout2 = 0;
			dcinob2 = 0;
			dcoutob2 = 0;
			dccurrentob2 = 0;
		}
		
		/****************************************************/
		/* DATA CAME FROM MODEM															*/
		/****************************************************/
		if(flag_uart4_rx == 1)
		{
			flag_uart4_rx = 0;
			
			huart = &huart1;
			printf("MODEM :: %s\r\n", Rx4buff);
			
			
			if(strstr(atcmdtable[atcmd_idx].ret, "__EGO"))
			{
				flag_ego = 1;
			}
			else if(strstr(Rx4buff, "ALREADY CONNECT"))
			{
				flag_atcommand_responded = 1;
				at_timeout_counter = 2000;
				atcmd_idx = 9;
			}
			else if(strstr(atcmdtable[atcmd_idx].cmd, "CIPSTATUS") && strstr(Rx4buff, "TCP CLOSE"))
			{
				flag_atcommand_responded = 1;
				at_timeout_counter = 2000;
				atcmd_idx = 9;
			}
			else if(strstr(Rx4buff, "+PDP: DEACT"))
			{
				flag_atcommand_responded = 0;
				at_timeout_counter = 20000;
				atcmd_idx = 0;
				Retry_Timeout_Counter = 0; //Initiate MODEM RESET
				ERROR_LED_ON;
				CONNECT_LED_OFF;
				ONLINE_LED_OFF;
			}
			else if(strstr(atcmdtable[atcmd_idx].cmd, "CIPSTATUS") && strstr(Rx4buff, "ERROR"))
			{
				flag_atcommand_responded = 1;
				
				if(atcmd_idx == 3)
				{
					ERROR_LED_ON;
					flag_atcommand_responded = 0;
					at_timeout_counter = 20000;
					atcmd_idx = 0;
				}
				flag_atcommand_responded = 0;
				at_timeout_counter = 20000;
				atcmd_idx = 0;
				Retry_Timeout_Counter = 0; //Initiate MODEM RESET
			}
			else
			{
				needle = strstr(Rx4buff, atcmdtable[atcmd_idx].ret);
				if(needle)
				{
					flag_atcommand_responded = 1;
					if(at_timeout_counter > 1000)
					{
						at_timeout_counter = 1000;
					}
					
					if(atcmd_idx >= 7)
					{
						ONLINE_LED_ON;
					}
					
					if(atcmd_idx >= 10)
					{
						CONNECT_LED_ON;
					}
					
					if(atcmd_idx >= 14)
					{
						CONNECT_LED_OFF;
					}
					
				}
				else
				{
					huart = &huart1;
					printf("RETURN :: %s\r\n", Rx4buff);
					
					/*********************************************************************/
					/*  CHECK FOR IP SMS  */
					if(strstr(Rx4buff, "+CMT:"))
					{
						if(strstr(Rx4buff, "ip"))
						{
							padle = strstr(Rx4buff, "ip");
							padle[18] = 0;
							huart = &huart1;
							printf("IP ADDRESS : %s", &padle[3]);
							StoreIp(&padle[3], addr);
							
							flag_atcommand_responded = 0;
							at_timeout_counter = 20000;
							atcmd_idx = 0;
							Retry_Timeout_Counter = 0; //Initiate MODEM RESET
						}
					}
					/*******************************************************************/
					
					if(strstr(Rx4buff, "CLOSED"))
					{
						CONNECT_LED_OFF;
					}
				}
			}
			
			//huart = &huart1;
			//printf("%s", Rx4buff);
			rx4buffindex = 0;
		}
		
		/*********************************************************/
		
		
		/****************************************************/
		/* DATA CAME FROM OutBack														*/
		/****************************************************/
		if(flag_uart5_rx == 1)
		{
			flag_uart5_rx = 0;
			
			if(rx5buffindex > 13)
			{
				
				Outback_Rx_watchdog_counter = 10000;
				
				if(OutbackIdx == 0)
				{
					dcinob = (Rx5buff[12] << 8) | Rx5buff[13];
					dcoutob = (Rx5buff[10] << 8) | Rx5buff[11];
					dckwhout = Rx5buff[9];
					dccurrentob = ((Rx5buff[3] & 0x0F) * 10) + (Rx5buff[1] & 0x0F);
					flag_lock = 0;
				}
				if(OutbackIdx == 1)
				{
					dcinob1 = (Rx5buff[12] << 8) | Rx5buff[13];
					dcoutob1 = (Rx5buff[10] << 8) | Rx5buff[11];
					dckwhout1 = Rx5buff[9];
					dccurrentob1 = ((Rx5buff[3] & 0x0F) * 10) + (Rx5buff[1] & 0x0F);
					flag_lock = 0;
				}
				if(OutbackIdx == 2)
				{
					dcinob2 = (Rx5buff[12] << 8) | Rx5buff[13];
					dcoutob2 = (Rx5buff[10] << 8) | Rx5buff[11];
					dckwhout2 = Rx5buff[9];
					dccurrentob2 = ((Rx5buff[3] & 0x0F) * 10) + (Rx5buff[1] & 0x0F);
					flag_lock = 0;
				}
			}
			
			obcounter++;
			if(obcounter > 5)
			{
				obcounter = 0;
				huart = &huart1;
				printf("IN1::%d OUT1::%d CURR1::%d KWh1::%d IN2:%d OUT2:%d CURR2:%d Kwh2:%d IN3:%d OUT3:%d CURR3:%d Kwh3:%d\r\n", dcinob, dcoutob, dccurrentob, dckwhout, dcinob1, dcoutob1, dccurrentob1, dckwhout1, dcinob2, dcoutob2, dccurrentob2, dckwhout2);
			}
			//HAL_UART_Transmit(&huart1, (uint8_t *)Rx5buff, rx5buffindex, 0x0fff);
			//huart = &huart1;
			//printf("\r\n");
			rx5buffindex = 0;
		}
		
		/*****************************************************/
		
		
		/***********************************************************************************/
		/*		UART1 External Serial Port. Received data redirected to UART4 before. 			 */
		/***********************************************************************************/
		if(flag_uart1_rx == 1)
		{
			flag_uart1_rx = 0;
			//huart = &huart4;
			
			if(strstr(Rx1buff, "PAUSE"))
			{
				flag_operation_mode = OPMODE_PAUSE;
				huart = &huart1;
				printf("-----------PAUSED-----------");
			}
			
			rx1buffindex = 0;
		}
		
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the Systick interrupt time 
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 700;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

	//HAL_TIM_Base_Init(&htim6);
	//__HAL_TIM_ENABLE(&htim6);
  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC6 PC7 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

static void UX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_8|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
		rx1buff[1] = 0;
		Rx1buff[rx1buffindex] = rx1buff[0];
		rx1buffindex++;
		Rx1buff[rx1buffindex] = 0;
		HAL_UART_Receive_IT(&huart1, (uint8_t *)rx1buff, 1);
		uart1_drip_counter = 35;
		if(rx1buff[0] == 10)
		{
			if(rx1buffindex > 4)
			{
				flag_uart1_rx = 1;
				uart1_drip_counter = 0;
			}
			else
			{
				rx1buffindex = 0;
			}
		}
	}
	
	if(huart->Instance == UART4)
	{
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
		rx4buff[1] = 0;
		Rx4buff[rx4buffindex] = rx4buff[0];
		rx4buffindex++;
		Rx4buff[rx4buffindex] = 0;
		HAL_UART_Receive_IT(&huart4, (uint8_t *)rx4buff, 1);
		uart4_drip_counter = 35;
		if(rx4buff[0] == 10)
		{
			if(rx4buffindex > 4)
			{
				flag_uart4_rx = 1;
				uart4_drip_counter = 0;
			}
			else
			{
				rx4buffindex = 0;
			}
		}
	}
	
	if(huart->Instance == UART5)
	{
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
		rx5buff[1] = 0;
		Rx5buff[rx5buffindex] = rx5buff[0];
		rx5buffindex++;
		Rx5buff[rx5buffindex] = 0;
		HAL_UART_Receive_IT(&huart5, (uint8_t *)rx5buff, 1);
		uart5_drip_counter = 50;
		if(rx5buff[0] == 10)
		{
			//flag_uart5_rx = 1;
		}
	}
	
	if(huart->Instance == USART2)
	{
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
		rx2buff[1] = 0;
		Rx2buff[rx2buffindex] = rx2buff[0];
		rx2buffindex++;
		Rx2buff[rx2buffindex] = 0;
		HAL_UART_Receive_IT(&huart2, (uint8_t *)rx2buff, 1);
		uart2_drip_counter = 50;
		if(rx2buff[0] == 10)
		{
			//flag_uart2_rx = 1;
		}
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	
}

void HAL_SYSTICK_Callback(void)
{
	if(systick_clock_01 < 1000)
	{
		systick_clock_01++;
	}
	else
	{
		flag_systick_01 = 1;
	}
	
	if(uart1_drip_counter > 0)
	{
		if(uart1_drip_counter == 1)
		{
			flag_uart1_rx = 1;
		}
		uart1_drip_counter--;
	}
	
	if(uart2_drip_counter > 0)
	{
		if(uart2_drip_counter == 1)
		{
			flag_uart2_rx = 1;
		}
		uart2_drip_counter--;
	}
	
	if(uart4_drip_counter > 0)
	{
		if(uart4_drip_counter == 1)
		{
			flag_uart4_rx = 1;
		}
		uart4_drip_counter--;
	}
	
	if(uart5_drip_counter > 0)
	{
		if(uart5_drip_counter == 1)
		{
			flag_uart5_rx = 1;
		}
		uart5_drip_counter--;
	}
	
	if(mb_timeout_counter > 0)
	{
		mb_timeout_counter--;
	}
	
	if(at_timeout_counter > 0)
	{
		if(at_timeout_counter == 1)
		{
		}
		at_timeout_counter--;
	}
	
	if(Retry_Timeout_Counter > 0)
	{
		Retry_Timeout_Counter--;
	}
	
	if(Outback_Rx_watchdog_counter > 0)
	{
		Outback_Rx_watchdog_counter--;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		
		if(tidx < 101851)
		{
			tidx++;
		}
		else
		{
			if(time_list_idx < time_list.size)
			{
				if(flag_lock == 0)
				{
					if(t2tick < time_list.array[time_list_idx].count)
					{
						if(t2tick == 0)
						{
							if(time_list.array[time_list_idx].state == 1)
							{
								HAL_GPIO_WritePin(Outback_PORT, Outback_PIN, GPIO_PIN_SET);
							}
							else
							{
								HAL_GPIO_WritePin(Outback_PORT, Outback_PIN, GPIO_PIN_RESET);
							}
						}
						t2tick++;
					}
					else
					{
						t2tick = 0;
						time_list_idx++;
					}
				}
			}
			else
			{
				time_list_idx = 0;
				tidx = 0;
				flag_lock = 1;
				
				if(OutbackIdx == 0)
				{
					
					OutbackIdx = 1;
					Outback_PIN = GPIO_PIN_7;
					Outback_PORT = GPIOC;
				}
				else if(OutbackIdx == 1)
				{
					//dckwhout1 = 0;
					//dcinob1 = 0;
					//dcoutob1 = 0;
					//dccurrentob1 = 0;
					
					OutbackIdx = 2;
					Outback_PIN = GPIO_PIN_8;
					Outback_PORT = GPIOA;
				}
				else if(OutbackIdx == 2)
				{
					//dckwhout2 = 0;
					//dcinob2 = 0;
					//dcoutob2 = 0;
					//dccurrentob2 = 0;
					
					OutbackIdx = 0;
					Outback_PIN = GPIO_PIN_6;
					Outback_PORT = GPIOC;
				}
			}
		}
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
