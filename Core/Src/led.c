/******************************************************************************
 *
 * Module: LED
 *
 * File Name: led.c
 *
 * Description: Source file for the LED driver
 *
 * Author: Mostafa Mahmoud
 *
 *******************************************************************************/

#include "led.h"  // Include the header file that defines structures and function prototypes

/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/

/**
 * @brief  Initializes the LED pin configuration
 * @param  LEDx: Pointer to a structure that holds the GPIO port and pin number of the LED
 * @retval None
 */
void LED_Init(LED_TypeDef *LEDx)
{
    if (LEDx == NULL)
        return;  // Exit if LEDx is a null pointer

    GPIO_InitTypeDef GPIO_InitStruct = {0};  // Initialize GPIO structure

    /* Configure GPIO pins */
    GPIO_InitStruct.Pin = LEDx->GPIO_pin;  // Set the pin number
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Set the pin to output push-pull mode
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;  // Set pull-down resistor
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // Set GPIO speed to high frequency

    HAL_GPIO_Init(LEDx->GPIOx, &GPIO_InitStruct);  // Initialize GPIO pin

    // Turn off the LED initially
    HAL_GPIO_WritePin(LEDx->GPIOx, LEDx->GPIO_pin, GPIO_PIN_RESET);
}

/**
 * @brief  Sets the state of the LED
 * @param  LEDx: Pointer to a structure that holds the GPIO port and pin number of the LED
 * @param  LedState: State to set the LED (LED_ON or LED_OFF)
 * @retval None
 */
void LED_Output(LED_TypeDef *LEDx, GPIO_PinState LedState)
{
    if (LEDx == NULL)
        return;  // Exit if LEDx is a null pointer

    HAL_GPIO_WritePin(LEDx->GPIOx, LEDx->GPIO_pin, LedState);  // Set LED state
}
