/******************************************************************************
 *
 * Module: LIMIT SWITCH
 *
 * File Name: limit_switch.c
 *
 * Description: Source file for TivaC Limit Switch Driver.
 *
 * Authors: Mostafa Mahmoud Ali
 *
 ******************************************************************************/

#include "limit_switch.h"

/*******************************************************************************
 *                           Functions Definitions                             *
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
 *     GPIO Port Must be Enabled Before calling this function.
 *     Example: if LimitSwitch will be connected to PortA
 *              then Inside MX_GPIO_Init() -> Call __HAL_RCC_GPIOA_CLK_ENABLE(); first
 * 
 * Return:
 * - None
 */

void LimitSwitch_Init(LimitSwitch_TypeDef *LimitSwitch)
{
    if (LimitSwitch == NULL)
        return;

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* Configure GPIO pins */
    GPIO_InitStruct.Pin = LimitSwitch->GPIO_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

#if (C_PIN_IS_CONNECTED_TO_GND)
    // Enable internal pull-up resistor for the pin
    GPIO_InitStruct.Pull = GPIO_PULLUP;
#else
    // Enable internal pull-down resistor for the pin
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
#endif // C_PIN_IS_CONNECTED_TO_GND

    HAL_GPIO_Init(LimitSwitch->GPIOx, &GPIO_InitStruct);
}

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
uint8_t LimitSwitch_IsPressed(LimitSwitch_TypeDef *limitSwitch)
{
    if (limitSwitch == NULL)
        return 0xFF; // Return an error value if the limit switch pointer is NULL

#if ((C_PIN_IS_CONNECTED_TO_GND && NO_PIN_IS_CONNECTED) || (!C_PIN_IS_CONNECTED_TO_GND && !NO_PIN_IS_CONNECTED))

    // Read the state of the limit switch pin and return the inverted value
    return (!HAL_GPIO_ReadPin(limitSwitch->GPIOx, limitSwitch->GPIO_pin));

#elif ((C_PIN_IS_CONNECTED_TO_GND && !NO_PIN_IS_CONNECTED) || (!C_PIN_IS_CONNECTED_TO_GND && NO_PIN_IS_CONNECTED))

    // Read the state of the limit switch pin and return the original value
    return (HAL_GPIO_ReadPin(limitSwitch->GPIOx, limitSwitch->GPIO_pin));

#endif

}

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
LimitSwitchState_e LimitSwitch_GetState(LimitSwitch_TypeDef *limitSwitch)
{
	LimitSwitchState_e currentState;
	
    if (limitSwitch == NULL)
        return UNDEFINED; // Return UNDEFINED State if the limit switch pointer is NULL

    currentState = (LimitSwitch_IsPressed(limitSwitch))? TOUCHED: UNTOUCHED;

    return currentState;
}
