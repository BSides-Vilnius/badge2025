/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "errno.h" //EBADF
//#include "unistd.h" //STD*_FILENO
#include "effects.h"
#include "console.h"
#include "settings.h"
#include "sao.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BTN_M_LIMIT			6
#define BTN_A_LIMIT			2
#define BTN_D_LIMIT			5
#define BTN_DEBOUNCE_DELAY	200
#define BTN_SEQ_LEN		21
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t btn_m = 0; //brightness
uint16_t btn_a = 0; //sparkle toggle
uint16_t btn_d = 0; //effect
uint32_t btn_last_tick = 0; //used for debouncing

// record button sequence
uint8_t btn_seq[BTN_SEQ_LEN] = {0};
uint8_t btn_seq_idx =0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t check_sparkle_secret() {
	const uint8_t key[BTN_SEQ_LEN]={1,1,2,2,3,3,1,2,1,2,3,2,3,2,3,3,2,2,1,1,2};
	int queue_idx;
	for (uint8_t idx=0; idx<BTN_SEQ_LEN; idx++) {
		queue_idx = (idx+btn_seq_idx)%BTN_SEQ_LEN;
		if (key[idx] != btn_seq[queue_idx]) {
//			printf("fail at %d (%d vs %d).\r\n", idx, key[idx], btn_seq[queue_idx]);
			return 0;
		}
	}
	printf("Sparkle mode unlocked.\r\n");
	return 1;
}

// return pointer to a function based on button D value
effect_function_t button_vaule_to_effect() {
	while (1) {
		switch (btn_d) {
			case 0:
				return &effect_pulse_green;
			case 1:
				return &effect_pulse_blue;
			case 2:
				return &effect_pulse_red;
			case 3:
				if (settings.speaker==1) {
					return &effect_wave;
				} else {
					btn_d++;
					btn_d%=BTN_D_LIMIT;
				}
				break;
			case 4:
				if (settings.sponsor==1) {
					return &effect_fire;
				} else {
					btn_d++;
					btn_d%=BTN_D_LIMIT;
				}
				break;
			default:
				btn_d=0;
				break;
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_ADC_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  const uint32_t data_size = (LED_COUNT+2)*4;
  uint8_t data[data_size];
  memset(data, 0, data_size);
  void * data_led_p = data+4;
  sk9822_t leds[LED_COUNT];
  static effect_function_t active_effect;

  settings_load();
  sao_activate();

  //adjust btn_d value to set the default effect
  switch (sao_active_driver_id) {
  	  case 2:
		  btn_d=3;
		  break;
  	  case 3:
  		  btn_d=4;
  		  break;
  }

  while (1)
  {
	active_effect = button_vaule_to_effect();
	active_effect(leds, LED_COUNT, btn_m+1);

	if (btn_a == 1) {
		if (settings.sparkle == 1) {
			effect_sparkle(leds, LED_COUNT, btn_m+1);
		} else if (check_sparkle_secret() == 1) {
			settings.sparkle=1;
			settings_save();
			btn_m = 0; //reset brightness
		} else {
			btn_a = 0;
		}
	}

	//write led values into the SPI data struct
	//data = start frame + data_led_p + end frame
	memcpy(data_led_p, leds, LED_COUNT*4);

	HAL_Delay(50);
	HAL_SPI_Transmit_DMA(&hspi1, data, data_size);
	HAL_SPI_Transmit_DMA(&hspi2, data, data_size); //duplicate effect on

	console_run_pending();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_USB;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	uint32_t current = HAL_GetTick();
//	printf("%lu-%lu=%ld\r\n", current, btn_last, current-btn_last);
	if (current - btn_last_tick < BTN_DEBOUNCE_DELAY) {
		return;
	}
	switch (GPIO_Pin) {
		case BTN_M_Pin:
			btn_m = (btn_m+1)%BTN_M_LIMIT;
			btn_seq[btn_seq_idx]=1;
			break;
		case BTN_A_Pin:
			btn_a = (btn_a+1)%BTN_A_LIMIT;
			btn_seq[btn_seq_idx]=2;
			break;
		case BTN_D_Pin:
			btn_d = (btn_d+1)%BTN_D_LIMIT;
			btn_seq[btn_seq_idx]=3;
			break;
	}
	btn_last_tick = current;
//	int queue_idx;
//	for (int i=0; i<BTN_QUEUE_LEN; i++) {
//		queue_idx = (i+btn_queue_idx+1)%BTN_QUEUE_LEN;
//		printf("%d ", btn_queue[queue_idx]);
//	}
//	printf("\r\n");
	btn_seq_idx++;
	btn_seq_idx%=BTN_SEQ_LEN;
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
  __disable_irq();
  while (1)
  {
  }
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
