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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "application.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define WELCOME_MSG "Welcome to the Nucleo management console\r\n"
#define MAIN_MENU   "Select the option you are interested in:\r\n\t1. Toggle LD2 LED\r\n\t2. Read USER BUTTON status\r\n\t3. Clear screen and print this message "
#define PROMPT "\r\n> "
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */
Client* g_client;
Topic g_topic;
uint8_t status;
uint8_t g_buttCode = 0;
char readBuf[1];
char rxDataBuffer[1];
char rxCapBuffer[50];
char txDataBuffer[10];
uint8_t intrxDataBuffer = 0x0;
uint8_t inttxDataBuffer = 0x0;
__IO ITStatus UartReady = SET;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//static void MX_RTC_Init(void);
//static void MX_USB_PCD_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void performCriticalTasks(void);
void printWelcomeMessage(void);
uint8_t readButton1(void); 
int8_t readUserInput(void);
uint8_t readUserCommand(void);
uint8_t processUserCommand(uint8_t);
uint8_t processUserInput(int8_t);
char* removeCRNL(char* p_buffer);
void clearRxBuffer(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  //uint8_t opt = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  int8_t errCode = 0;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /**Configure the Systick interrupt time */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  /**Configure the Systick */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //MX_RTC_Init();
  //MX_USB_PCD_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* Enable USART2 interrupt */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  /* Enable GPIOB Pin 7 Button interrupt */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  
  printWelcomeMessage();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
    /*
    if(g_buttCode==1)
	  {
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rButton 1 has been pressed\n\r", strlen("\n\rButton 1 has been pressed\n\r"), HAL_MAX_DELAY);
		  //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
		  //runApplication(&huart2, &huart1);
      g_buttCode = 0;
	  }
    */

    //Poll uart1 for a command
    status = readUserCommand();
    
    //Process that command
    processUserCommand(status);

  }

}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{
  if(GPIO_Pin == GPIO_PIN_7)
    g_buttCode = 1;
}

uint8_t readButton1(void) 
{
  uint8_t buttonResult;
  buttonResult = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
  return buttonResult;
}

int8_t readUserInput(void) {
  int8_t retVal = -1;

  if(UartReady == SET) {
    UartReady = RESET;
    HAL_UART_Receive_IT(&huart2, (uint8_t*)readBuf, 1);
    retVal = atoi(readBuf);
  }
  return retVal;
}


uint8_t readUserCommand(void) 
{
  HAL_StatusTypeDef retVal;
  uint8_t lineIsValid = 0;
  char cr = '\r';
  char nl = '\n';
  //retVal = HAL_UART_Receive(&huart1, (uint8_t*)rxDataBuffer, sizeof(rxDataBuffer), HAL_MAX_DELAY);
  int i;
  for(i=0;i<sizeof(rxCapBuffer);i++) //Run through the entire buffer 
  {
    retVal = HAL_UART_Receive(&huart1, (uint8_t*)rxDataBuffer, 1, HAL_MAX_DELAY); //Recieve one byte at a time
    rxCapBuffer[i] = rxDataBuffer[0];

    if(rxCapBuffer[i-1]==cr && rxCapBuffer[i]==nl)
    {
      lineIsValid = 1;
      break;
    }
  }
  return lineIsValid;
}

// Process the user commmand by checking for a valid line(End with Carriage Return)  
// and then perform an opertation based on that command
uint8_t processUserCommand(uint8_t p_status) 
{
  uint8_t cmdStatus = 0;
  char* userCMD;

  userCMD = strtok(rxCapBuffer, " ");

  if(p_status == 1) //1 = line is valid
  {
    if(strcmp(userCMD,"ledOn\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rTurn LED on\n\r", strlen("\n\rTurn LED on\n\r"), HAL_MAX_DELAY);
    	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    }
    else if(strcmp(userCMD,"ledOff\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rTurn LED off\n\r", strlen("\n\rTurn LED off\n\r"), HAL_MAX_DELAY);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    }
    else if(strcmp(userCMD,"runApp\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rRun Thingstream Demo\n\r", strlen("\n\rRun Thingstream Demo\n\r"), HAL_MAX_DELAY);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
      runApplication(&huart2, &huart1);
    }
    else if(strcmp(userCMD,"publish\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rPublish Message\n\r", strlen("\n\rPublish Message\n\r"), HAL_MAX_DELAY);
      publishMessage(g_client, g_topic, "Hello From Device1");
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rMessage Sent!\n\r", strlen("\n\rMessage Sent!\n\r"), HAL_MAX_DELAY);
    }
    else if(strcmp(userCMD,"ShowInfo\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rDisplay some info\n\r", strlen("\n\rDisplay some info\n\r"), HAL_MAX_DELAY);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    }
    else if(strcmp(userCMD,"connect\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rClient connection check\n\r", strlen("\n\rClient connection check\n\r"), HAL_MAX_DELAY);
      g_client = setupTSStack(&huart2, &huart1);
    }
    else if(strcmp(userCMD,"SubD\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rSubscribe to a Default Topic\n\r", strlen("\n\rSubscribe to a Default Topic\n\r"), HAL_MAX_DELAY);
      g_topic = subscribeTopic(g_client, EXAMPLE_TOPIC);
    }
    else if(strcmp(userCMD,"Sub")==0)
    {
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rSubscribe to Specified Topic\n\r", strlen("\n\rSubscribe to Specified Topic\n\r"), HAL_MAX_DELAY);
      uint8_t count = 0;
      while (userCMD != NULL)
      {
        if (count == 1) 
        {
          char* topicName = userCMD;
          topicName = removeCRNL(topicName);
          g_topic = subscribeTopic(g_client, topicName);
        }
        userCMD = strtok(NULL, " ");  //Go to next string in command buffer
        ++count;
      }
      clearRxBuffer();
    }
    else if(strcmp(userCMD,"Send")==0)
    {
      //HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rThe Command param is: \n\r", strlen("\n\rThe Command param is: \n\r"), HAL_MAX_DELAY);
      uint8_t count = 0;
      while (userCMD != NULL)
      {
        if (count == 1) 
        {
          publishMessage(g_client, g_topic, userCMD);
        }
        userCMD = strtok(NULL, " ");  //Go to next string in command buffer
        ++count;
      }
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rMessage Sent!\n\r", strlen("\n\rMessage Sent!\n\r"), HAL_MAX_DELAY);
    }
    else if(strcmp(userCMD,"wait\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rWait for a message to be received from subbed topic\n\r", strlen("\n\rWait for a message to be received from subbed topic\n\r"), HAL_MAX_DELAY);
      waitForMessage(g_client);
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rDone and still connected and subbed\n\r", strlen("\n\rDone and still connected and subbed\n\r"), HAL_MAX_DELAY);
    }
    else if(strcmp(userCMD,"disconnect\r\n")==0)
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rDisconnecting from Client\n\r", strlen("\n\rDisconnecting from Client\n\r"), HAL_MAX_DELAY);
      disconnectClient(g_client);
    }
    else
    {
      clearRxBuffer();
      HAL_UART_Transmit(&huart1, (uint8_t*)"\n\rError Command\n\r", strlen("\n\rError Command\n\r"), HAL_MAX_DELAY);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    }
    cmdStatus = 1;
  }
  else
  {
    HAL_UART_Transmit(&huart1, (uint8_t*)"UART ERROR\r\n", strlen("UART ERROR\r\n"), HAL_MAX_DELAY);
    cmdStatus = 0;
  }
  return cmdStatus;
}

uint8_t processUserInput(int8_t opt) {
  char msg[30];

  if(!(opt >=1 && opt <= 3))
    return 0;

  sprintf(msg, "%d", opt);
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  switch(opt) {
  case 1:
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
    break;
  case 2:
    sprintf(msg, "\r\nUSER BUTTON status: %s",
        HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET ? "PRESSED" : "RELEASED");
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    break;
  case 3:
    return 2;
  };

  HAL_UART_Transmit(&huart2, (uint8_t*)PROMPT, strlen(PROMPT), HAL_MAX_DELAY);
  return 1;
}

char* removeCRNL(char* p_buffer)
{
  int n;
  char cr = '\r';
  char nl = '\n';
  int length = sprintf(p_buffer, "%s", p_buffer);
  for(n=0;n<length;n++)
  {
    if(p_buffer[n]==cr || p_buffer[n]==nl)
    {
      p_buffer[n] = '\0';
    }
  }
  return p_buffer;
}

void clearRxBuffer(void)
{
  uint8_t n;
  for(n=0;n<sizeof(rxCapBuffer);n++)
  {
    rxCapBuffer[n] = '\0';
  } 
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
 /* Set transmission flag: transfer complete*/
// UartReady = SET;
//}

void performCriticalTasks(void) {
  HAL_Delay(100);
}

void printWelcomeMessage(void) 
{
  HAL_UART_Transmit(&huart1, (uint8_t*)"\033[0;0H", strlen("\033[0;0H"), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1, (uint8_t*)"\033[2J", strlen("\033[2J"), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1, (uint8_t*)WELCOME_MSG, strlen(WELCOME_MSG), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1, (uint8_t*)MAIN_MENU, strlen(MAIN_MENU), HAL_MAX_DELAY);
  /*
  char *strings[] = {"\033[0;0H", "\033[2J", WELCOME_MSG, MAIN_MENU, PROMPT};

  for (uint8_t i = 0; i < 5; i++) {
    HAL_UART_Transmit_IT(&huart2, (uint8_t*)strings[i], strlen(strings[i]));
    while (HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX || HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX_RX);
  }
  */
}
  

  /* USER CODE END 3 */


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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  huart1.Init.BaudRate = 115200;
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
  huart2.Init.BaudRate = 115200;
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
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  //GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  //GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);  

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

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
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
