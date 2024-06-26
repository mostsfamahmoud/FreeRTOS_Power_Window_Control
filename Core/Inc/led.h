/******************************************************************************
 *
 * Module: LED
 *
 * File Name: led.h
 *
 * Description: Header file for the LED driver
 *
 * Author: Mostafa Mahmoud
 *
 *******************************************************************************/

#ifndef INC_LED_H_  // Header guard to prevent multiple inclusions
#define INC_LED_H_

#include "stm32f429xx.h"     // Include necessary STM32F4xx headers
#include "stm32f4xx_hal.h"   // Include necessary STM32F4xx HAL headers
#include <stdint.h>          // Include standard integer types


/*******************************************************************************
 *                                 Definitions                                  *
 *******************************************************************************/
#define LED_ON        GPIO_PIN_SET   // Macro definition for LED ON state
#define LED_OFF       GPIO_PIN_RESET // Macro definition for LED OFF state

typedef struct
{
    GPIO_TypeDef *GPIOx;    // Pointer to the GPIO port of the LED
    uint16_t GPIO_pin;      // Pin number of the LED
} LED_TypeDef;              // Define LED_TypeDef structure for LED configuration


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/**
 * @brief  Initializes the LED pin configuration
 * @param  LEDx: Pointer to a structure that holds the GPIO port and pin number of the LED
 * @retval None
 */
void LED_Init(LED_TypeDef *LEDx);

/**
 * @brief  Sets the state of the LED
 * @param  LEDx: Pointer to a structure that holds the GPIO port and pin number of the LED
 * @param  LedState: State to set the LED (LED_ON or LED_OFF)
 * @retval None
 */
void LED_Output(LED_TypeDef *LEDx, GPIO_PinState LedState);

#endif /* INC_LED_H_ */
