/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "ksj_led.h"
#include "ksj_rssi_led.h"
#include "ksj_sta_led.h"
#include "ksj_adc.h"
#include "ksj_booting.h"
#include "ksj_pwm.h"
#include "ksj_i2c.h"
#include "ksj_uart.h"
#include "D_motor.h"
#include "ksj_timer.h"
#include "D_joystick.h"
#include "ksj_mpu6050.h"
#include "d_global_defs.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_USART1_UART_Init();
  MX_ADC3_Init();
  MX_TIM3_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  //0. all led off
  all_led_off();
  // 1. for printf non buffering
  setvbuf(stdout, NULL, _IONBF, 0); //예외사항중 하나
  // for UART Rx Start
  HAL_UART_Receive_IT(&huart1, &rx_data1, 1); //예외사항으로 꼭 기억! UART에서 인터럽트방식을 사용하려면 꼭 이걸 넣어줘야 함
  //1. booting LED
  Power_on_led_running();
  //2. drone Init
  all_drone_peripheral_init();
  //3. ADC DMA Start
  HAL_ADC_Start_DMA(&hadc3, (uint32_t *)&adc_buf[0], ADC_BUF_LEN);
#if 0//--
  //4. TIM3 PWM Start
#if !defined PWM_DMA_MODE_EN
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
#else
  // direction : memory to peripheral
  pa6_pwm_dma_run();
#endif
  //
#else//--
  //HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  //htim3.Instance->CCR1 = 140; //테스트용
  //htim3.Instance->CCR2 = 140;
  //htim3.Instance->CCR3 = 140;
  //htim3.Instance->CCR4 = 140;
  Motors_Init();
#endif//--

#if defined MPU6050_ENABLE
  // 5. MPU6050
  MPU6050_Initialization();
  mpu6050_offset_datas_get();
  //while(1); // block for normal operation.
#endif

  // 6. DWT 타이머 초기화 및 사이클 카운터 활성화/리셋
  DWT_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //
	  // 1. PE3 led
	  pe3_led_blinking_demo2();

	  // 2. printf
	  //printf("\r\nHello 2025.07.31");

	  // 3. PD1,2,3 RSSI LED
	  pd1_rssi_led1_blinking();
	  pd2_rssi_led2_blinking();
	  pd3_rssi_led3_blinking();

	  // 4. PD4,5,6,7 status LED
	  //ksj_sta_led_blinking();

	  // 5. ADC3
	  //adc_polling();
	  adc3ch0_demo_using_normal_DMA();

	  //6. pwm led with pa6
#if !defined PWM_DMA_MODE_EN
	  pa6_pwm_polling();
#else
	  // run with dma mode
#endif
	  //pa6_tim3_ch1_pwm_lf_motor_run1();
	  //data_pwm_process();
	  Motors_Update_PWM(); // 모터는 명령서(Keyboard_Input_Process();)에 따라 모터를 계속 업데이트

	  // 7. I2C2
	  //lt_eeprom_demo2();

	  // 8. command poser
	  //uart1_rx_process(); 이전 코드

	  //9. rcv data process
	#if defined __RX_CHAR_BUF_USE__
	  //Keyboard_Input_Process();
	  //rcv1_data_process();
	#elif defined __RX_STR_BUF_USE__
		  rcv2_str_process();
	#elif defined __RX_CMD_BUF_USE__
		  cmd2_exec_process();
	#endif
	  //10. D_joystick
	  Keyboard_Input_Process();


	  //11. mpu6050 test
	  //mpu6050_raw_datas_get();
	  //mpu6050_offset_reflect_datas_get(); //offset_reflect
	  //mpu6050_sensor_converted_demo();
	  //mpu6050_sensor_angle_demo();
	  mpu6050_sensor_cf_angle_demo();

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
#ifdef USE_FULL_ASSERT
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
