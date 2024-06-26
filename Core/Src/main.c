/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os.h"
#include "led.h";
#include "button.h"
#include "limit_switch.h"
#include "dc_motor.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */

// Motor control Commands
typedef enum {
	OFF = 1, UP, DOWN
} MotorControlCommand_e;

// Semaphore Handles
xSemaphoreHandle xLockSemaphore;    // Semaphore for lock button handling
xSemaphoreHandle xBinarySemaphore; // Semaphore for synchronization between ISR and task
xSemaphoreHandle xJamSemaphore;    // Semaphore for The Jam Task
xSemaphoreHandle xMotorMutex;     // Motor mutex between driver and passenger

// Task Handles
xTaskHandle DriverHandle;         // Handle for driver task

xQueueHandle xQueue;                //Handle for Receive Queue task

/* Note: If you change the used PORTs here, You Must also go to MX_GPIO_Init() to enable that PORT */

BUTTON_TypeDef DriverUpButton = { GPIOB, GPIO_PIN_10 };
BUTTON_TypeDef DriverDownButton = { GPIOB, GPIO_PIN_11 };

BUTTON_TypeDef PassengerUpButton = { GPIOB, GPIO_PIN_12 };
BUTTON_TypeDef PassengerDownButton = { GPIOD, GPIO_PIN_9 };

// Button Configurations
BUTTON_TypeDef LockBtn = { GPIOD, GPIO_PIN_2 };

BUTTON_TypeDef JamButton = { GPIOD, GPIO_PIN_3 };

// Limit Switch Configurations
LimitSwitch_TypeDef LimitUpSwitch = { GPIOD, GPIO_PIN_0 };
LimitSwitch_TypeDef LimitDownSwitch = { GPIOD, GPIO_PIN_1 };

// LED Configurations
LED_TypeDef USER_LD3_GREEN_LED = { GPIOG, GPIO_PIN_13 };
LED_TypeDef USER_LD4_RED_LED = { GPIOG, GPIO_PIN_14 };

TaskHandle_t DriverHandle;         // Handle for driver task

EXTI_HandleTypeDef hextiA;
EXTI_ConfigTypeDef exti_configA;

EXTI_HandleTypeDef hextiB;
EXTI_ConfigTypeDef exti_configB;

///*******************************************************************************
// *                           Functions Definitions                             *
// *******************************************************************************/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void const *argument);

void LockPassengerTask(void *pvParameters);
void JamTask(void *pvParameters);
void DriverTask(void *pvParamters);
void receiveQueue(void *pvParameters);
void PassengerTask(void *pvParamters);

void PWC_motorControl(MotorControlCommand_e command);
void EXTI_Initialization();
static void MX_NVIC_Init(void);

/**
 *
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();

	MX_NVIC_Init();

	BUTTON_Init(&LockBtn);

	BUTTON_Init(&PassengerUpButton);
	BUTTON_Init(&PassengerDownButton);

	BUTTON_Init(&DriverUpButton);
	BUTTON_Init(&DriverDownButton);

	BUTTON_Init(&JamButton);

	LimitSwitch_Init(&LimitUpSwitch);
	LimitSwitch_Init(&LimitDownSwitch);

	LED_Init(&USER_LD3_GREEN_LED);
	LED_Init(&USER_LD4_RED_LED);

	DcMotor_Init();

	EXTI_Initialization();

	HAL_EXTI_SetConfigLine(&hextiA, &exti_configA);

	HAL_EXTI_SetConfigLine(&hextiB, &exti_configB);

	xMotorMutex = xSemaphoreCreateMutex();

	xQueue = xQueueCreate(2, sizeof(long));

	vSemaphoreCreateBinary(xBinarySemaphore);
	vSemaphoreCreateBinary(xLockSemaphore);
	vSemaphoreCreateBinary(xJamSemaphore);

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	if (xBinarySemaphore != NULL) // Check if binary semaphore was created successfully
	{
		// Create tasks
		xTaskCreate(JamTask, "JamTask", 270, NULL, 5, NULL);   //Create Jam Task
		xTaskCreate(LockPassengerTask, "LockTask", 270, NULL, 4, NULL); // Create lock task
		xTaskCreate(receiveQueue, "recieveQueue", 270, NULL, 3, NULL); //Create Receive task
		xTaskCreate(PassengerTask, "passenger", 270, NULL, 1, NULL); // Create passenger task
		xTaskCreate(DriverTask, "driver", 270, NULL, 1, &DriverHandle); // Create driver task

		osKernelStart();
	}

	for(;;);

}

// Helper Function For Power Window Control Module (PWC)
void PWC_motorControl(MotorControlCommand_e command) {
	switch (command) {
	case OFF:
		DcMotor_Rotate(STOP);
		break;
	case UP:
		DcMotor_Rotate(ClockWise);
		break;
	case DOWN:
		DcMotor_Rotate(Anti_ClockWise);
		break;
	default:
		// DO Nothing
		break;
	}
}

void LockPassengerTask(void *pvParameters) {
	xSemaphoreTake(xLockSemaphore, 0); // Attempt to take semaphore (non-blocking)

	while (1) {
		// Take semaphore (blocking)
		xSemaphoreTake(xLockSemaphore, portMAX_DELAY);

		// Check lock button state
		if (BUTTON_IsPressed(&LockBtn)) {
			LED_Output(&USER_LD4_RED_LED, LED_ON); // Turn RED LED ON for indication
			vTaskPrioritySet(DriverHandle, 2); // Change Driver Task Priority to 2
		} else {
			LED_Output(&USER_LD4_RED_LED, LED_OFF); // Turn RED LED OFF for indication
			vTaskPrioritySet(DriverHandle, 1); // Change Driver Task Priority to 1
		}
	}
}

void JamTask(void *pvParameters) {
	xSemaphoreTake(xJamSemaphore, 0);
	for (;;) {
		xSemaphoreTake(xJamSemaphore, portMAX_DELAY);

		/* Turn The motor to simulate the window moving */
		PWC_motorControl(DOWN);

		/* Delay for 2.0 seconds ( to be clearly seen in the video */
		HAL_Delay(2000);

		/* Clear Pins to stop the motor */
		PWC_motorControl(OFF);
	}
}

void receiveQueue(void *pvParameters) {
	int Val;
	BaseType_t xStatus;
	while (1) {
		// Receive from queue (blocking)
		xStatus = xQueueReceive(xQueue, &Val, portMAX_DELAY);
		PWC_motorControl(Val);
	}
}

void DriverTask(void *pvParamters) {

	int Mode = OFF;

	for (;;) {

		xSemaphoreTake(xMotorMutex, portMAX_DELAY);

		//Handle the Up Button
		if (BUTTON_IsPressed(&DriverUpButton)) {
			Mode = UP;
			xQueueSendToBack(xQueue, &Mode, 0);
			vTaskDelay(400); // Debounce delay

			if (BUTTON_IsPressed(&DriverUpButton)) {  			// Manual Mode
				PWC_motorControl(UP);
				while (BUTTON_IsPressed(&DriverUpButton))
					;
				PWC_motorControl(OFF);

			} else { // Automatic Mode
				PWC_motorControl(UP);
				while (!LimitSwitch_IsPressed(&LimitUpSwitch))
					;
				PWC_motorControl(OFF);
			}

			Mode = OFF;
			xQueueSendToBack(xQueue, &Mode, 0);
		}

		//Handle the Down Button
		if (BUTTON_IsPressed(&DriverDownButton)) {
			Mode = DOWN;
			xQueueSendToBack(xQueue, &Mode, 0);
			vTaskDelay(400); // Debounce delay

			if (BUTTON_IsPressed(&DriverDownButton)) {  // Manual Mode
				PWC_motorControl(DOWN);
				while (BUTTON_IsPressed(&DriverDownButton))
					;
				PWC_motorControl(OFF);
			} else { 								// Automatic Mode
				PWC_motorControl(DOWN);
				while (!LimitSwitch_IsPressed(&LimitDownSwitch))
					;
				PWC_motorControl(OFF);
			}
			Mode = OFF;
			xQueueSendToBack(xQueue, &Mode, 0);
		}

		xSemaphoreGive(xMotorMutex);
		HAL_Delay(200);

	}
}

void PassengerTask(void *pvParamters) {
	int Mode = OFF;

	for (;;) {

		xSemaphoreTake(xMotorMutex, portMAX_DELAY);

		//Handle the Up Button
		if (BUTTON_IsPressed(&PassengerUpButton)) {
			Mode = UP;
			xQueueSendToBack(xQueue, &Mode, 0);
			vTaskDelay(400); // Debounce delay

			if (BUTTON_IsPressed(&PassengerUpButton)) {  // Manual Mode
				PWC_motorControl(UP);
				while (BUTTON_IsPressed(&PassengerUpButton))
					;
				PWC_motorControl(OFF);
			} else { // Automatic Mode
				PWC_motorControl(UP);
				while (!LimitSwitch_IsPressed(&LimitUpSwitch))
					;
				PWC_motorControl(OFF);
			}
			Mode = OFF;
			xQueueSendToBack(xQueue, &Mode, 0);
		}

		//Handle the Down Button
		if (BUTTON_IsPressed(&PassengerDownButton)) {
			Mode = DOWN;
			xQueueSendToBack(xQueue, &Mode, 0);
			vTaskDelay(400); // Debounce delay

			if (BUTTON_IsPressed(&PassengerDownButton)) {  // Manual Mode
				PWC_motorControl(DOWN);
				while (BUTTON_IsPressed(&PassengerDownButton))
					;
				PWC_motorControl(OFF);
			} else { // Automatic Mode
				PWC_motorControl(DOWN);
				while (!LimitSwitch_IsPressed(&LimitDownSwitch))
					;
				PWC_motorControl(OFF);
			}
			Mode = OFF;
			xQueueSendToBack(xQueue, &Mode, 0);
		}

		xSemaphoreGive(xMotorMutex);
		HAL_Delay(200);
		//timer0_Delay(200); // Task delay

	}
}

void EXTI_Initialization() {
	//lock
	exti_configA.Line = EXTI_LINE_2;
	exti_configA.Mode = EXTI_MODE_INTERRUPT;
	exti_configA.Trigger = EXTI_TRIGGER_RISING;
	exti_configA.GPIOSel = EXTI_GPIOD;

//jam
	exti_configB.Line = EXTI_LINE_3;
	exti_configB.Mode = EXTI_MODE_INTERRUPT;
	exti_configB.Trigger = EXTI_TRIGGER_RISING;
	exti_configB.GPIOSel = EXTI_GPIOD;
}

void EXTI2_IRQHandler(void) {

	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
	HAL_EXTI_IRQHandler(&hextiA);
}

void EXTI3_IRQHandler(void) {

	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
	HAL_EXTI_IRQHandler(&hextiB);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

// this is what you want to do when the interrupt happen

	if (GPIO_Pin == GPIO_PIN_2) {
		// lock button

		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(xLockSemaphore, &xHigherPriorityTaskWoken); // Give lock semaphore from ISR
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken); // End ISR, possibly switching to a higher priority task
	}

	else if (GPIO_Pin == GPIO_PIN_3) {
		//Jam button
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(xJamSemaphore, &xHigherPriorityTaskWoken); // Give binary semaphore from ISR
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken); // End ISR, possibly switching to a higher priority task
	}

}

static void MX_NVIC_Init(void) {

	//lock
	HAL_NVIC_SetPriority(EXTI2_IRQn, 7, 7);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	//jam
	HAL_NVIC_SetPriority(EXTI3_IRQn, 6, 6);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

}


/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument) {
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for (;;) {
		osDelay(1);
	}
	/* USER CODE END 5 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
