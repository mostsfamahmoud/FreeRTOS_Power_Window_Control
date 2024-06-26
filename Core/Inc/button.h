/******************************************************************************
 *
 * Module: BUTTON
 *
 * File Name: button.h
 *
 * Description: Header file for TivaC External Button Driver.
 *
 * Author: Mostafa Mahmoud Ali
 *
 ******************************************************************************/

#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f429xx.h"     // Include necessary STM32F4xx headers
#include "stm32f4xx_hal.h"   // Include necessary STM32F4xx HAL headers
#include <stdint.h>          // Include standard integer types

typedef struct
{
    GPIO_TypeDef *GPIOx;    // Pointer to the GPIO port of the button
    uint16_t GPIO_pin;      // Pin number of the button
} BUTTON_TypeDef;           // Define BUTTON_TypeDef structure for button configuration

/********************************************************************
 *                              Function Prototypes               *
 *******************************************************************/

/*
 * Function: BUTTON_Init
 * ----------------------------
 *   Initialize the Button Pin.
 *
 *   This function configures the GPIO port for the button pin. It sets up the pin
 *   for digital input, disables analog functionality, enables the internal pull-up resistor,
 *   and configures the pin direction.
 *
 *   BUTTONx: A pointer to the Button structure containing the port and pin information.
 *
 *   Returns: None
 */
void BUTTON_Init(BUTTON_TypeDef *BUTTONx);

/*
 * Function: BUTTON_IsPressed
 * ----------------------------
 *   Check if the Button is pressed.
 *
 *   This function reads the state of the button connected to the specified pin and returns
 *   the state as a uint8_t value (0 if pressed, 1 if not pressed).
 *
 *   BUTTONx: A pointer to the Button structure containing the port and pin information.
 *
 *   Returns:
 *     uint8_t: The state of the button (0 if pressed, 1 if not pressed).
 */
uint8_t BUTTON_IsPressed(BUTTON_TypeDef *BUTTONx);

#endif // BUTTON_H
