/******************************************************************************
 *
 * Module: DC MOTOR
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for the DC MOTOR driver
 *
 * Author: Mostafa Mahmoud - Abdelrahman Ali
 *
 *******************************************************************************/

#include "dc_motor.h"

/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/

/*
 Description
 1) The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 2) Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DcMotor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	__MOTOR_PORT_CLK_ENABLE();

	/* Configure GPIO pins */
	GPIO_InitStruct.Pin = MOTOR_IN1_PIN_ID | MOTOR_IN2_PIN_ID | MOTOR_EN1_PIN_ID;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(MOTOR_GPIO_PORT, &GPIO_InitStruct);

	/* Configure GPIO pin Output Level */
	/* Stop the DC-Motor at the beginning (IN1 = 0, IN2 = 0) */
	HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_IN1_PIN_ID, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_IN2_PIN_ID, GPIO_PIN_RESET);
}

/*
 Description:
 1) The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.

 2) Send the required duty cycle to the PWM driver based on the required speed value.

 Inputs:
 1) State: The required DC Motor state, it should be CW or A-CW or stop.
 DcMotor_State data type should be declared as enum or uint8.

 Return: None
 */
void DcMotor_Rotate(DcMotor_State state) {

	HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_EN1_PIN_ID, GPIO_PIN_SET);

	/* Setting the DC Motor rotation direction (CW/ or A-CW or stop) based on the state value. */
	switch (state) {
	case STOP:
		/* Stop the DC-Motor (IN1 = 0, IN2 = 0) */
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_IN1_PIN_ID, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_IN2_PIN_ID, GPIO_PIN_RESET);
		break;
	case ClockWise:
		/* DC-Motor Mode --> ClockWise Rotation (IN1 = 0, IN2 = 1) */
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_IN1_PIN_ID, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_IN2_PIN_ID, GPIO_PIN_SET);
		break;
	case Anti_ClockWise:
		/* DC-Motor Mode --> Anti_ClockWise Rotation (IN1 = 1, IN2 = 0) */
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_IN1_PIN_ID, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_IN2_PIN_ID, GPIO_PIN_RESET);
		break;
	default:
		break;
	}
}
