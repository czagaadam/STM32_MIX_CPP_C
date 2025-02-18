/*
 *  main.cpp
 *
 *  Author: Adam Czaga czagaadam@gmail.com
 *
 *  Demonstration of how to use GPIO parent class and Din,Doutt inherited classes
 *
 */

#include "main.h"
#include "GPIObase.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "gpio.h"
#include "i2c.h"
#include "usart.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Inc/vl53l0x.h"
#include "Inc/vl53l0x/vl53l0x_platform.h" //typedef VL53L0X_Dev_t
#include "Inc/vl53l0x/vl53l0x_api.h"
#include "Inc/vl53l0xHandlerClass.h"
void SystemClock_Config(void);

#define UART_MAX_UINT_32 10
#define UART_TIMEOUT 10
#define UART_MAX_TRANSMISSION 255

void UART_SendNewLine() {
	HAL_UART_Transmit(&huart5, (uint8_t*) "\r\n", 2, UART_TIMEOUT);
}

uint8_t UART_SendString(const char *sdata) {
	if (sdata == NULL)
		return 1;
	size_t len = strlen(sdata);
	if (HAL_UART_Transmit(&huart5, (uint8_t*) sdata,
			len < UART_MAX_TRANSMISSION ? len : UART_MAX_TRANSMISSION,
			UART_TIMEOUT))
		return 1;
	return 0;
}

uint8_t UART_Send_UInt(UART_HandleTypeDef *huart, uint32_t num) {
	if (huart == NULL)
		return 1;
	char sdata[UART_MAX_UINT_32 + 1] = { 0 };
	if (snprintf(sdata, UART_MAX_UINT_32 + 1, "%lu", num) < 0)
		return 1;
	if (HAL_UART_Transmit(huart, (uint8_t*) (sdata), strlen(sdata),
			UART_TIMEOUT))
		return 1;
	return 0;
}

void I2C3_EV_IRQHandler(void) {
	HAL_I2C_EV_IRQHandler(&hi2c3);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	char ConvertBuffer2[30] = "";
	if (hi2c->Devaddress == 0x92) {
		sprintf(ConvertBuffer2, "Tenp sensor callback");
		UART_SendString(ConvertBuffer2);
	} else if (hi2c->Devaddress == 0x40) {
		sprintf(ConvertBuffer2, "Port exp. callback");
		UART_SendString(ConvertBuffer2);
	} else if (hi2c->Devaddress == 0x9A) {
		sprintf(ConvertBuffer2, "ADC callback");
		UART_SendString(ConvertBuffer2);
	} else {
		sprintf(ConvertBuffer2, "Unknown dev. callback");
		UART_SendString(ConvertBuffer2);
	}
}

#ifdef __cplusplus
}
#endif

Din button1{GPIOD, GPIO_PIN_10, GPIO_Inverted_Logic_Off};
Dout led1{GPIOD, GPIO_PIN_9, GPIO_Inverted_Logic_Off, GPIO_PIN_RESET};

void Btn1_call_back(void) {
	led1.write(GPIO_PIN_SET);
}

void HAL_GPIO_EXTI_Callback(gpio_num_t gpio_pin) {
	if(gpio_pin == GPIO_PIN_10)	Btn1_call_back();
}

int main(void) {

	//GPIOD->BSRR = (uint32_t)GPIO_PIN_15 << 16U;
	GPIOD->BSRR = (uint32_t) GPIO_PIN_14 << 16U;
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_I2C3_Init();
	MX_UART5_Init();

	button1.init();
	led1.init();

	uint16_t sensor1_mm = 0;
	uint16_t sensor2_mm = 0;
	const uint8_t numofsensors = 1;

	//HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

	led1.write(GPIO_PIN_SET);
	//memory allocation for VL53L0X_Dev_t structures and gpio pins
	VL53L0X_InitHandlerClass(numofsensors);
	//initialize VL53L0X_Dev_t structure, shutdown pin, comm type, speed, address
	//todo: return index number
	//VL53L0X_DefineAndPutDevice(GPIO_PIN_15, 1, 100, 0x60);
	HAL_Delay(500);
	VL53L0X_APPSConfigDevice(&hi2c3, GPIOD, GPIO_PIN_14, 1, 100, 0x22);
	HAL_Delay(500);
	//shutdown pins low state
	VL53L0X_APPSResetNodes();
	HAL_Delay(500);
	//set new address for 1st sensor
	//VL53L0X_ActiveNodeAddress(1);
	HAL_Delay(500);
	//initialize sensor with custom address
	//VL53L0X_APPStartCustomAddress(1);
	HAL_Delay(500);
	//set new address for 2nd sensor
	VL53L0X_APPSSetAddress(0);
	HAL_Delay(500);
	//initialize sensor with custom address
	VL53L0X_APPStartWithCustomAddress(0);
	HAL_Delay(500);

	while (1) {
		led1.toggle();
		HAL_Delay(sensor1_mm);
		led1.toggle();
		HAL_Delay(sensor1_mm);
		sensor1_mm = VL53L0X_APPSNodeMeasure(0);
  }
}

#ifdef __cplusplus
extern "C" {
#endif
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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

  /** Initializes the CPU, AHB and APB buses clocks
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
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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

#ifdef __cplusplus
}
#endif

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
