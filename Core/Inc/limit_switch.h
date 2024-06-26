/******************************************************************************
 *
 * Module: BUTTON
 *
 * File Name: button.c
 *
 * Description: Header file for TivaC Limit Switch Driver.
 *
 * Authors: Mostafa Mahmoud Ali 
 *
 ******************************************************************************/
#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include "stm32f429xx.h"     // Include necessary STM32F4xx headers
#include "stm32f4xx_hal.h"   // Include necessary STM32F4xx HAL headers
#include <stdint.h>          // Include standard integer types

#define NO_PIN_IS_CONNECTED              (0U)
#define C_PIN_IS_CONNECTED_TO_GND        (1U)

typedef struct
{
    GPIO_TypeDef *GPIOx;    // Pointer to the GPIO port of the LimitSwitch
    uint16_t GPIO_pin;      // Pin number of the LimitSwitch
} LimitSwitch_TypeDef;           // Define LimitSwitch_TypeDef structure for LimitSwitch configuration

typedef enum {
    UNTOUCHED , TOUCHED, UNDEFINED
} LimitSwitchState_e;



/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/


/*
 * Description :
 * Initialize the Limit Switch Pin.
 *
 * This function configures the GPIO port for the limit switch pin. It sets up the pin
 * for digital input, disables analog functionality, enables the internal pull-up resistor,
 * and configures the pin direction.
 *
 * Parameters:
 * - limitSwitch: A pointer to the LimitSwitch structure containing the port and pin information.
 *
 * Preconditions:
 * - GPIO Port Must be Enabled Before calling this function.
 * - Example: if Limit Switch will be connected to PortA then Call GPIO_Init(PORTA_ID); first
 * 
 * Return:
 * - None
 */
void LimitSwitch_Init(LimitSwitch_TypeDef *LimitSwitch);


/*
 * Description :
 * Function that checks if the Limit Switch is pressed.
 *
 * This function reads the state of the limit switch connected to the specified pin and returns
 * the state as a uint8_t value (0 if pressed, 1 if not pressed).
 *
 * Parameters:
 * - limitSwitch: A pointer to the LimitSwitch structure containing the port and pin information.
 * 
 * Return:
 * - uint8_t: The state of the limit switch (0 if pressed, 1 if not pressed).
 */
uint8_t LimitSwitch_IsPressed(LimitSwitch_TypeDef *limitSwitch);

/*
 * Description :
 * Function that returns the state of the Limit Switch.
 *
 * This function utilizes LimitSwitch_IsPressed to determine if the Limit Switch is
 * pressed or not, and returns the corresponding state.
 *
 * Parameters:
 * - limitSwitch: A pointer to the LimitSwitch structure containing the port and pin information.
 * 
 * Return:
 * - LimitSwitchState_e: The state of the limit switch (TOUCHED if pressed, UNTOUCHED if not pressed).
 */
LimitSwitchState_e LimitSwitch_GetState(LimitSwitch_TypeDef *limitSwitch);

#endif // LIMIT_SWITCH_H
