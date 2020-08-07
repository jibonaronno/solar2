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

extern atcmd_t atcmdtable[];

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

UART_HandleTypeDef *huart;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

volatile uint8_t flag_uart1_rx = 0;
volatile uint8_t flag_uart5_rx = 0;
volatile uint8_t flag_uart4_rx = 0;

uint32_t uart1_drip_counter = 0;
uint32_t uart4_drip_counter = 0;

int systick_clock_01 = 0;
uint8_t flag_systick_01 = 0;
char rx1buff[10];
char Rx1buff[100];
char rx1buffindex = 0;

char rx5buff[10];
char Rx5buff[100];
char rx5buffindex = 0;

char rx4buff[10];
char Rx4buff[100];
char rx4buffindex = 0;

volatile uint8_t flag_atcommand_responded = 0;
volatile int at_timeout_counter = 0;
volatile int atcmd_idx = 0;
volatile uint8_t flag_ego = 0;

unsigned char pData[] = {0x1A, 0x00};

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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
	char *needle;
	int lidx01 = 0;
	char buf01[150];

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	
	huart = &huart1;
	printf("Starting Code ... ");
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)rx1buff, 1);
	
	//HAL_UART_Receive_IT(&huart5, (uint8_t *)rx5buff, 1);
	
	HAL_UART_Receive_IT(&huart4, (uint8_t *)rx4buff, 1);
	
	at_timeout_counter = 20000;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(flag_systick_01 == 1)
		{
			systick_clock_01 = 0;
			flag_systick_01 = 0;
			//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
		}
		
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
							//huart = &huart4;
							lidx01 = strlen(atcmdtable[atcmd_idx].cmd);
							strcpy(buf01, atcmdtable[atcmd_idx].cmd);
							buf01[lidx01] = 0x1A;
							buf01[lidx01+1] = 0;
							lidx01++;
							//printf("%s",atcmdtable[atcmd_idx].cmd);
							HAL_UART_Transmit(&huart4, (unsigned char*)buf01, lidx01, 0x0fff);
							HAL_Delay(1500);
							huart = &huart1;
							printf("SENDING DATA + CTRL+Z\r\n");
						}
						else
						{
							huart = &huart4;
							printf("%s",atcmdtable[atcmd_idx].cmd);
						}
						
						HAL_Delay(500);
						huart = &huart1;
						printf("%s -- %s\r\n",atcmdtable[atcmd_idx].cmd, atcmdtable[atcmd_idx].ret);
						HAL_Delay(500);
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
		
		/****************************************************/
		/* DATA CAME FROM MODEM															*/
		/****************************************************/
		if(flag_uart4_rx == 1)
		{
			flag_uart4_rx = 0;
			
			if(strstr(atcmdtable[atcmd_idx].ret, "__EGO"))
			{
				flag_ego = 1;
			}
			else if(strstr(atcmdtable[atcmd_idx].cmd, "CIPSTATUS") && strstr(Rx4buff, "ERROR"))
			{
				flag_atcommand_responded = 1;
				at_timeout_counter = 2000;
				atcmd_idx = -1;
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
				}
			}
			
			huart = &huart1;
			printf("%s", Rx4buff);
			rx4buffindex = 0;
		}
		
		/*********************************************************/
		
		if(flag_uart5_rx == 1)
		{
			flag_uart5_rx = 0;
			huart = &huart1;
			printf("%s", Rx5buff);
			rx5buffindex = 0;
		}
		
		if(flag_uart1_rx == 1)
		{
			flag_uart1_rx = 0;
			huart = &huart4;
			printf("%s", Rx1buff);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC7 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
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
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
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
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
		rx5buff[1] = 0;
		Rx5buff[rx5buffindex] = rx5buff[0];
		rx5buffindex++;
		Rx5buff[rx5buffindex] = 0;
		HAL_UART_Receive_IT(&huart5, (uint8_t *)rx5buff, 1);
		if(rx5buff[0] == 10)
		{
			flag_uart5_rx = 1;
		}
	}
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
	
	if(uart4_drip_counter > 0)
	{
		if(uart4_drip_counter == 1)
		{
			flag_uart4_rx = 1;
		}
		uart4_drip_counter--;
	}
	
	if(at_timeout_counter > 0)
	{
		if(at_timeout_counter == 1)
		{
		}
		at_timeout_counter--;
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
