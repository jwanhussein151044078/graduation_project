/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "math.h"
#include "DCMotor.h"
#include "Tracker.h"
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
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;
TIM_HandleTypeDef htim12;
TIM_HandleTypeDef htim13;
TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart6_rx;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */


struct T_data{
  int16_t x ;        // pos
  int16_t y ;        // pos
  int16_t is_sun ;  // false if the robot in the shadow , true if the robot in the sunlight
  int16_t temp ;
  int16_t hum ;
  int16_t soil ;
  int16_t water_levet ;
  int16_t is_operate ;
  int16_t battery_percentage ;
  int16_t error ;
};

struct R_data{
	bool    enforce;          // to enforce the robot to operate in any condition .
	bool    ok     ;          // if 0 mean that is no sunlight due to weather condition or day time .
	bool    is_night  ;
	int16_t sunlight_amount ; // the period to keep the plant in the sunlight (in minutes).
    int16_t water_scedual   ; // how many time should the plant be water in a day .
};

uint8_t msg[2]; //////// buffer to store the msg from the esp a

const size_t size_of_R = sizeof(struct R_data);
const size_t size_of_T = sizeof(struct T_data);

struct T_data t_data ;
struct R_data r_data ;

uint32_t encoderL = 0 ;
uint32_t encoderR = 0 ;

DCMotor dcright = DCMotor(htim9.Instance->CCR2);
DCMotor dcleft  = DCMotor(htim9.Instance->CCR1);
DCSystem dcSys  = DCSystem(dcright,dcleft);

MPU6050 mpu = MPU6050();

uint16_t buffer[8];
uint16_t adcval[8];

GPIO_PinState mz80FR = GPIO_PIN_RESET;
GPIO_PinState mz80FL = GPIO_PIN_RESET;
GPIO_PinState mz80L  = GPIO_PIN_RESET;
GPIO_PinState mz80R  = GPIO_PIN_RESET;


uint32_t IC_Val12 = 0;
uint32_t IC_Val22 = 0;
uint32_t Difference2 = 0;
uint8_t Is_First_Captured2 = 0;  // is the first value captured ?
uint8_t Distance2  = 0;

uint32_t IC_Val13 = 0;
uint32_t IC_Val23 = 0;
uint32_t Difference3 = 0;
uint8_t Is_First_Captured3 = 0;  // is the first value captured ?
uint8_t Distance3  = 0;

uint8_t Fdistance = 0;



//uint8_t durum=0;//Humidity=0,Temperature=0;
uint16_t Humidity=0,Temperature=0;
uint16_t tempVal=0,humVal=0;
uint8_t dhtVal[2];
uint8_t mData[40];
uint16_t mTime1 = 0, mTime2 = 0;
uint16_t mbit = 0;
uint8_t  parityVal = 0, genParity = 0;

uint8_t batteryP = 0;


Tracker tracker = Tracker(dcSys);


int water_schaduale = -1 ;
int watering_counter = 0 ;
int watering_period = 2147483646 ;

int sunlight_schaduale = -1 ;
int sunlight_counter   = 0  ;


int demo_counter = 0 ;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM9_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM14_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM13_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM12_Init(void);
static void MX_TIM11_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */
uint8_t DHT11_Read (void);



int test = 0 ;
uint8_t chp = 0;
uint8_t ch  = 0;

uint8_t flag ;
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
	test = size_of_T ;
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
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_TIM3_Init();
  MX_TIM9_Init();
  MX_I2C1_Init();
  MX_TIM14_Init();
  MX_ADC3_Init();
  MX_TIM13_Init();
  MX_TIM8_Init();
  MX_TIM12_Init();
  MX_TIM11_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_TIM10_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  	  /*******           adc start              **********/
  HAL_ADC_Start_DMA(&hadc3, (uint32_t*)buffer, 8);

  	  /*******           encoder config         **********/
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_4);
  	  /***************************************************/

  	  /***********          mpu config        ************/
  mpu.set_i2c(&hi2c1);
  	  /***************************************************/

  	  /******            dc motor config           *******/
  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);
  HAL_TIM_Base_Start_IT(&htim14);
  dcright.setCCRx(htim9.Instance->CCR2);
  dcright.setGPIO(GPIOG, GPIOG,GPIO_PIN_11,GPIO_PIN_13);
  dcleft.setCCRx(htim9.Instance->CCR1);
  dcleft.setGPIO(GPIOG, GPIOG,GPIO_PIN_10,GPIO_PIN_15);
  dcSys.set_encoders(&encoderL, &encoderR);
  dcSys.setMPU(&mpu);
  dcSys.setTIM(&htim14);
  	  /***************************************************/

	  /******  start timer 12 for microsecond delay ******/
  HAL_TIM_Base_Start_IT(&htim12);
	  /***************************************************/

  	  /******  start timer 13 for distance readings ******/
  HAL_TIM_Base_Start_IT(&htim13);
  	  /***************************************************/

  	  /** start timer 8 for hc-04 in input capture mode left **/
  HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_1);
  //HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_2);
  	  /***************************************************/

  	  /** start timer 4 for hc-04 in input capture mode right **/
  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_4);
  //HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_2);
      /***************************************************/

  	  /******  start timer 10 for second interrupt ******/
  HAL_TIM_Base_Start_IT(&htim10);
  	  /***************************************************/

  	  /******  start timer 10 for second interrupt ******/
  HAL_TIM_Base_Start_IT(&htim2);
      /***************************************************/




	  /******  enable timer 1 for the demo  ******/
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  htim1.Instance->CCR3 = 200 ;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
	/***************************************************/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  ////////////////////////////////////// wait for msg from the esp ////////////////////////////////////////

  while(HAL_UART_Receive(&huart2, &ch, 1, -1)== HAL_OK){
	  if(chp == 'O' && ch == 'K'){
  		break ;
  	  }
  	  chp = ch ;
  }
  ch = 0 ; chp = 0 ;
  while(HAL_UART_Receive(&huart2, &ch, 1, -1)== HAL_OK){
  	  if(chp == 'O' && ch == 'K'){
    	break ;
  	  }
  	  chp = ch ;
  }
  HAL_UART_Receive_DMA(&huart6, (uint8_t*)&r_data,size_of_R);




  //a = HAL_UART_Receive(&huart5, msg, 2, -1);


  //test = dcSys.turnBy(90);



 // HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_SET);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */



	  t_data.error = 0 ;
	  /*
	  t_data.soil = 0 ;
	  t_data.water_levet = 0 ;
	  */
	  flag = tracker.traverse();
	  	  //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	  if (flag == 1){                     // no sunlight was found
		  /**** the area was traversed but no sunlight was found ****/
		  /**** sleep for 1 hour then try again ****/
		  int c = 0 ;
		  while(c < 60){
			  /**** sleep for 1 min ****/
			  HAL_Delay(60000);

			  tracker.watering_palnt();
			  c++;
		  }
		  tracker.restart();
	  }else if(flag == 5){                // sunlight found
	  	//HAL_GPIO_WritePin(GPIOC,YELLOW_Pin, GPIO_PIN_RESET);
		  sunlight_counter++;
		  tracker.set_sunlight_counter(sunlight_counter);
		  HAL_Delay(60000);
	  }else if(flag == 2 ){                // error

	  }else if(flag == 7){                 // the robot in the shadow

	  }

	  //HAL_UART_Transmit(&huart2, (uint8_t*)&t_data, size_of_T, -1);


	  /*
	  dcright.moveForword(100);
	  dcleft.moveForword(100);
	  HAL_UART_Transmit(&huart3,(uint8_t*)& t_data, sizeof(struct T_data), 100);
	  HAL_GPIO_TogglePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin);
	  HAL_Delay(500);
	  */
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

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc3.Init.Resolution = ADC_RESOLUTION_10B;
  hadc3.Init.ScanConvMode = ENABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 8;
  hadc3.Init.DMAContinuousRequests = ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 6;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 7;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 8;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 554-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 256-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  htim2.Init.Prescaler = 9999-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 431999-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 8;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 72-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0xffff-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 72-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 0xffff-1;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 554-1;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 256-1;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */
  HAL_TIM_MspPostInit(&htim9);

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 9999-1;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 7199-1;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 72-1;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 0xffff-1;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief TIM12 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM12_Init(void)
{

  /* USER CODE BEGIN TIM12_Init 0 */

  /* USER CODE END TIM12_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM12_Init 1 */

  /* USER CODE END TIM12_Init 1 */
  htim12.Instance = TIM12;
  htim12.Init.Prescaler = 72-1;
  htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim12.Init.Period = 0xffff-1;
  htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim12, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM12_Init 2 */

  /* USER CODE END TIM12_Init 2 */

}

/**
  * @brief TIM13 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM13_Init(void)
{

  /* USER CODE BEGIN TIM13_Init 0 */

  /* USER CODE END TIM13_Init 0 */

  /* USER CODE BEGIN TIM13_Init 1 */

  /* USER CODE END TIM13_Init 1 */
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 9999-1;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 1439-1;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM13_Init 2 */

  /* USER CODE END TIM13_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 720-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 10000-1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

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
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|lefttrig_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, temphumsens_Pin|GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, USB_PowerSwitchOn_Pin|righttrig_Pin|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_13|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_MDC_Pin RMII_RXD0_Pin RMII_RXD1_Pin */
  GPIO_InitStruct.Pin = RMII_MDC_Pin|RMII_RXD0_Pin|RMII_RXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RMII_REF_CLK_Pin RMII_MDIO_Pin RMII_CRS_DV_Pin */
  GPIO_InitStruct.Pin = RMII_REF_CLK_Pin|RMII_MDIO_Pin|RMII_CRS_DV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin lefttrig_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|lefttrig_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PG0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : RMII_TXD1_Pin */
  GPIO_InitStruct.Pin = RMII_TXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(RMII_TXD1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : temphumsens_Pin PD0 */
  GPIO_InitStruct.Pin = temphumsens_Pin|GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : mz80sl_Pin mz80fl_Pin mz80sr_Pin mz80fr_Pin 
                           USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = mz80sl_Pin|mz80fl_Pin|mz80sr_Pin|mz80fr_Pin 
                          |USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_PowerSwitchOn_Pin righttrig_Pin PG10 PG11 
                           PG13 PG15 */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin|righttrig_Pin|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_13|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */
void set_output(){
	GPIO_InitTypeDef DHT11_DATA={0};

	DHT11_DATA.Pin= temphumsens_Pin;
	DHT11_DATA.Mode=GPIO_MODE_OUTPUT_PP;
	DHT11_DATA.Pull=GPIO_NOPULL;
	DHT11_DATA.Speed=GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(temphumsens_GPIO_Port,&DHT11_DATA);
}
void set_input(){
	GPIO_InitTypeDef DHT11_DATA={0};

	DHT11_DATA.Pin= temphumsens_Pin;
	DHT11_DATA.Mode=GPIO_MODE_INPUT;
	DHT11_DATA.Pull=GPIO_NOPULL;
	DHT11_DATA.Speed=GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(temphumsens_GPIO_Port,&DHT11_DATA);
}

void delay2(uint16_t time){
	__HAL_TIM_SET_COUNTER(&htim11,0);
	while(__HAL_TIM_GET_COUNTER(&htim11) < time );
}

void delay(uint16_t time){
	__HAL_TIM_SET_COUNTER(&htim12,0);
	while(__HAL_TIM_GET_COUNTER(&htim12) < time );
}

void HCSR04_Read (){
	/*
	switch(num){
		case 2:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);  // pull the TRIG pin HIGH
			delay(20);  // wait for 10 us
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);  // pull the TRIG pin low
			__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC3);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);  // pull the TRIG pin HIGH
			delay(20);  // wait for 10 us
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the TRIG pin low
			__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC3);
			break;
		case 0:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);  // pull the TRIG pin HIGH
			delay(20);  // wait for 10 us
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);  // pull the TRIG pin low
			__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC2);
			break;
	}
	*/
	HAL_GPIO_WritePin(GPIOB, lefttrig_Pin, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	HAL_GPIO_WritePin(GPIOG, righttrig_Pin, GPIO_PIN_SET);
	delay(20);  // wait for 10 us
	HAL_GPIO_WritePin(GPIOB, lefttrig_Pin, GPIO_PIN_RESET);  // pull the TRIG pin low
	HAL_GPIO_WritePin(GPIOG, righttrig_Pin, GPIO_PIN_RESET);  // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim8, TIM_IT_CC1 );
	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC4 );

}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	adcval[0] = buffer[0];
	adcval[1] = buffer[1];
	adcval[2] = buffer[2];
	adcval[3] = buffer[3];
	adcval[4] = buffer[4];
	adcval[5] = buffer[5];
	adcval[6] = buffer[6];
	adcval[7] = buffer[7];
	//set_panels(uint16_t tl , uint16_t tr , uint16_t bl , uint16_t br );
	tracker.set_panels(adcval[4] , adcval[3] , adcval[5] , adcval[6] );
	//Fdistance = 29.988 * pow(buffer[0]*0.003 , -1.173);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim3){
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1 || htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 ){
			encoderR+=1;
		}else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3 || htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4 ){
			encoderL+=1;
		}
	}else if (htim == &htim8){

		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){  // left distance
			//HAL_GPIO_TogglePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin);
			if (Is_First_Captured2==0){
				IC_Val12 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
				Is_First_Captured2 = 1;  // set the first captured as true
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			}else if (Is_First_Captured2==1){
				IC_Val22 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
				__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
				if (IC_Val22 > IC_Val12){
					Difference2 = IC_Val22-IC_Val12;
				}else if (IC_Val12 > IC_Val22){
					Difference2 = (0xffff - IC_Val12) + IC_Val22;
				}
				Distance2 = Difference2 * .034/2;

				if(mz80L == GPIO_PIN_RESET && Distance2 >= 30){
					Distance2 = 15 ;
				}else if(mz80R == GPIO_PIN_SET && Distance2 <= 30){
					Distance2 = 35;
				}
				tracker.set_Ldis(Distance2);
				Is_First_Captured2 = 0; // set it back to false
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
				__HAL_TIM_DISABLE_IT(&htim8, TIM_IT_CC1);
			}
		}
	}else if(htim == &htim4){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4){
			if (Is_First_Captured3==0){
				IC_Val13 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4); // read the first value
				Is_First_Captured3 = 1;  // set the first captured as true
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
			}else if (Is_First_Captured3==1){
				IC_Val23 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);  // read second value
				__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
				if (IC_Val23 > IC_Val13){
					Difference3 = IC_Val23-IC_Val13;
				}else if (IC_Val13 > IC_Val23){
					Difference3 = (0xffff - IC_Val13) + IC_Val23;
				}
				Distance3 = Difference3 * .034/2;

				if(mz80R == GPIO_PIN_RESET && Distance3 >= 30){
					Distance3 = 15 ;
				}else if(mz80R == GPIO_PIN_SET && Distance3 <= 30){
					Distance3 = 35 ;
				}
				tracker.set_Rdis(Distance3);
				Is_First_Captured3 = 0; // set it back to false
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
				__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC4);
			}

		}
	}
}

void battery_calc(){
	float V = 0.00966796875 * adcval[7] ;
	if(V >= 8.35 ){
		batteryP = 100 ;
	}else if(V >= 8.3){
		batteryP = 95 ;
	}else if(V >= 8.22){
		batteryP = 90 ;
	}else if(V >= 8.16){
		batteryP = 85 ;
	}else if(V >= 8.05){
		batteryP = 80 ;
	}else if(V >= 7.97){
		batteryP = 75 ;
	}else if(V >= 7.91){
		batteryP = 70 ;
	}else if(V >= 7.83){
		batteryP = 65 ;
	}else if(V >= 7.75){
		batteryP = 60 ;
	}else if(V >= 7.71){
		batteryP = 55 ;
	}else if(V >= 7.67){
		batteryP = 50 ;
	}else if(V >= 7.63){
		batteryP = 45 ;
	}else if(V >= 7.59){
		batteryP = 40 ;
	}else if(V >= 7.57){
		batteryP = 35 ;
	}else if(V >= 7.53){
		batteryP = 30 ;
	}else if(V >= 7.49){
		batteryP = 25 ;
	}else if(V >= 7.45){
		batteryP = 20 ;
	}else if(V >= 7.41){
		batteryP = 15 ;
	}else if(V >= 7.37){
		batteryP = 10 ;
	}else if(V >= 7.22){
		batteryP = 5 ;
	}else if(V >= 6.55){
		batteryP = 0 ;
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim13){
		mz80FL = HAL_GPIO_ReadPin(GPIOG, mz80fl_Pin);
		mz80FR = HAL_GPIO_ReadPin(GPIOG, mz80fr_Pin);
		mz80L  = HAL_GPIO_ReadPin(GPIOG, mz80sl_Pin);
		mz80R  = HAL_GPIO_ReadPin(GPIOG, mz80sr_Pin);

		//HCSR04_Read(num);
		HCSR04_Read();

		//num = (num+1)%2;
		Fdistance = 29.988 * pow(buffer[0]*0.003 , -1.173);
		if(mz80FL == GPIO_PIN_RESET || mz80FR == GPIO_PIN_RESET){
			Fdistance = 0 ;
		}
		tracker.set_Fdis(Fdistance);

		t_data.x = tracker.getX() ;
		t_data.y = tracker.gety() ;
		t_data.is_sun = tracker.isSun() ;
		//HAL_GPIO_TogglePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin);
	}else if (htim == &htim10){
		//HAL_GPIO_TogglePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin);
		DHT11_Read();
		t_data.hum = Humidity ;
		t_data.temp = Temperature ;

		t_data.is_operate = tracker.is_OP();
		//HAL_GPIO_TogglePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin);
	}else if (htim == &htim2){

		battery_calc();
		t_data.battery_percentage = batteryP ;
		watering_counter++;
		demo_counter++;
		t_data.water_levet = (int16_t)(adcval[1]*0.09765625) ;
		t_data.soil = (int16_t)(100-(adcval[2]*0.09765625)) ;
		if(watering_counter >= watering_period){
			tracker.setWater();
			watering_counter = 0 ;
		}

		if(demo_counter == 2 ){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);
		}else if(demo_counter == 6){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
		}else if(demo_counter == 7){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);
		}else if(demo_counter == 9){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
			demo_counter = 0 ;
		}

	}
}

uint8_t DHT11_Read (void){

	for(int a=0;a<40;a++) mData[a]=0;

	HAL_TIM_Base_Start(&htim11);

	mTime1 = 0, mTime2 = 0,/* durum=0, tempVal=0, humVal=0,*/ parityVal = 0, genParity = 0,  mbit = 0;

	set_output();
	HAL_GPIO_WritePin(temphumsens_GPIO_Port,temphumsens_Pin,GPIO_PIN_RESET);
	delay2(18000);
   	set_input();

	__HAL_TIM_SET_COUNTER(&htim11, 0);				//set timer counter to zero
	while(HAL_GPIO_ReadPin(temphumsens_GPIO_Port,temphumsens_Pin) == GPIO_PIN_SET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim11) > 500) return 0;

	__HAL_TIM_SET_COUNTER(&htim11, 0);
	while(HAL_GPIO_ReadPin(temphumsens_GPIO_Port,temphumsens_Pin) == GPIO_PIN_RESET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim11) > 500) return 0;
	mTime1 = (uint16_t)__HAL_TIM_GET_COUNTER(&htim11);

	__HAL_TIM_SET_COUNTER(&htim11, 0);
	while(HAL_GPIO_ReadPin(temphumsens_GPIO_Port,temphumsens_Pin) == GPIO_PIN_SET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim11) > 500) return 0;
    mTime2 = (uint16_t)__HAL_TIM_GET_COUNTER(&htim11);

	if(mTime1 < 75 && mTime1 > 85 && mTime2 < 75 && mTime2 > 85){return 0;}

	for(int j = 0; j < 40; j++){
		__HAL_TIM_SET_COUNTER(&htim11, 0);
		while(HAL_GPIO_ReadPin(temphumsens_GPIO_Port,temphumsens_Pin) == GPIO_PIN_RESET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim11) > 500) return 0;
		__HAL_TIM_SET_COUNTER(&htim11, 0);
		while(HAL_GPIO_ReadPin(temphumsens_GPIO_Port,temphumsens_Pin) == GPIO_PIN_SET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim11) > 500) return 0;
		mTime1 = (uint16_t)__HAL_TIM_GET_COUNTER(&htim11);

		//check pass time in high state
		//if pass time 25uS set as LOW
		if(mTime1 > 20 && mTime1 < 30){
			mbit = 0;
		}
		else if(mTime1 > 60 && mTime1 < 80){ //if pass time 70 uS set as HIGH
			 mbit = 1;
		}
		//set i th data in data buffer
		mData[j] = mbit;
	}

	//get hum value from data buffer
	humVal = 0 ;
	for(int i = 0; i < 8; i++){
		humVal += mData[i];
		humVal = humVal << 1;
	}

	//get temp value from data buffer
	tempVal = 0 ;
	for(int i = 16; i < 24; i++){
		tempVal += mData[i];
		tempVal = tempVal << 1;
	}

	//get parity value from data buffer
	for(int i = 32; i < 40; i++){
		parityVal += mData[i];
		parityVal = parityVal << 1;
	}

	parityVal = parityVal >> 1;
	humVal = humVal >> 1;
	tempVal = tempVal >> 1;
	Humidity = humVal ;
	Temperature = tempVal ;
	genParity = humVal + tempVal;

	dhtVal[0]= tempVal;
	dhtVal[1] = humVal;
	HAL_TIM_Base_Stop(&htim11);
	return 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if(GPIO_Pin == GPIO_PIN_0) // If The INT Source Is EXTI Line9 (A9 Pin)
    {
    	//HAL_GPIO_TogglePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin); // Toggle The Output (LED) Pin
    	HAL_UART_Transmit(&huart2, (uint8_t*)&t_data, size_of_T, -1);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){


	tracker.set_enf(r_data.enforce);
	tracker.set_ok(r_data.ok);
	tracker.set_sunlight_scheduale(r_data.sunlight_amount);
	if(r_data.water_scedual != water_schaduale){
		water_schaduale = r_data.water_scedual ;
		watering_counter = 0 ;
		watering_period = (int)(1440/water_schaduale) ;
		sunlight_schaduale = r_data.sunlight_amount ;
	}
	if(r_data.is_night == true){
		//sunlight_counter = 0 ;
		//tracker.set_sunlight_counter(0);
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
