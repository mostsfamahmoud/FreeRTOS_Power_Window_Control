/******************************************************************************
 *
 * Module: BUTTON
 *
 * File Name: button.c
 *
 * Description: Source file for TivaC External Button Driver.
 *
 * Author: Mostafa Mahmoud Ali
 *
 ******************************************************************************/

#include "button.h"

/*******************************************************************************
 *                           Functions Definitions                             *
 ******************************************************************************/

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
 *   Preconditions:
 *     GPIO Port Must be Enabled Before calling this function.
 *     Example: if Button will be connected to PortA
 *              then Inside MX_GPIO_Init() -> Call __HAL_RCC_GPIOA_CLK_ENABLE(); first
 *
 *   Returns: None
 */
void BUTTON_Init(BUTTON_TypeDef *BUTTONx)
{
    if (BUTTONx == NULL)
        return;

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* Configure GPIO pins */
    GPIO_InitStruct.Pin = BUTTONx->GPIO_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(BUTTONx->GPIOx, &GPIO_InitStruct);
}

/*
 * Function: BUTTON_IsPressed
 * ----------------------------
 *   Check if the Button is pressed.
 *
 *   This function reads the state of the button connected to the specified pin and returns
 *   the state as a uint8_t value (1 if pressed, 0 if not pressed).
 *
 *   BUTTONx: A pointer to the Button structure containing the port and pin information.
 *
 *   Returns:
 *     uint8_t: The reversed state of the button (1 if pressed, 0 if not pressed).
 */
uint8_t BUTTON_IsPressed(BUTTON_TypeDef *BUTTONx)
{
    if (BUTTONx == NULL)
        return 0xFF;                    // Return an error value if the button pointer is NULL

    // Read the state of the button pin and return the inverted value (button is in pull-up mode)
    return (!HAL_GPIO_ReadPin(BUTTONx->GPIOx, BUTTONx->GPIO_pin));
}





//void BUTTON_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_pin)
//{
//	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
//
//	/* Configure GPIO pins */
//	GPIO_InitStruct.Pin = GPIO_pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//
//	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
//}
