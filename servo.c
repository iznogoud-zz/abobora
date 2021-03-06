#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_dma.h>
#include <stm32f1xx_hal_tim.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_rcc.h>
#include "servo.h"
#include "defs.h"

#define SERVO_PRESCALER 39
#define SERVO_PERIOD 36000

static TIM_HandleTypeDef  TimHandle;

extern void Error_Handler();

void servo_set_channel(uint8_t channel, unsigned pulse)
{
    TIM_OC_InitTypeDef  sConfig;
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

    /* Set the pulse value for channel A */
    sConfig.Pulse = pulse;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, channel ? SERVO_TIM_CHANNEL_B:SERVO_TIM_CHANNEL_A) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
}

void servo_set_channel_a(unsigned pulse)
{
    if (HAL_TIM_PWM_Stop(&TimHandle, SERVO_TIM_CHANNEL_A) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    servo_set_channel(SERVO_CHANNEL_A, pulse);
    if (HAL_TIM_PWM_Start(&TimHandle, SERVO_TIM_CHANNEL_A) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
}

void servo_set_channel_b(unsigned pulse)
{
    if (HAL_TIM_PWM_Stop(&TimHandle, SERVO_TIM_CHANNEL_B) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    servo_set_channel(SERVO_CHANNEL_B, pulse);
    if (HAL_TIM_PWM_Start(&TimHandle, SERVO_TIM_CHANNEL_B) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

}

static void servo_enable_setup()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = SERVO_ENABLE_GPIO_PIN;

    HAL_GPIO_WritePin( SERVO_ENABLE_GPIO, SERVO_ENABLE_GPIO_PIN, 0);

    HAL_GPIO_Init(SERVO_ENABLE_GPIO, &GPIO_InitStruct);
}

void servo_enable()
{
    HAL_GPIO_WritePin( SERVO_ENABLE_GPIO, SERVO_ENABLE_GPIO_PIN, 1);
}

void servo_disable()
{
    HAL_GPIO_WritePin( SERVO_ENABLE_GPIO, SERVO_ENABLE_GPIO_PIN, 0);
}


void servo_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = SERVO_GPIO_PIN_A | SERVO_GPIO_PIN_B;
    HAL_GPIO_Init(SERVO_GPIO, &GPIO_InitStruct);

    servo_enable_setup();

    SERVO_TIM_CLK_ENABLE();

    TimHandle.Instance = SERVO_TIM;

    TimHandle.Init.Prescaler         = SERVO_PRESCALER;
    TimHandle.Init.Period            = SERVO_PERIOD;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    servo_set_channel(SERVO_CHANNEL_A, SERVO_DEFAULT_PULSE_A);
    servo_set_channel(SERVO_CHANNEL_B, SERVO_DEFAULT_PULSE_B);
    /* Start channels */
    if (HAL_TIM_PWM_Start(&TimHandle, SERVO_TIM_CHANNEL_A) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    /* Start channels */
    if (HAL_TIM_PWM_Start(&TimHandle, SERVO_TIM_CHANNEL_B) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    // Let PWM settle
    HAL_Delay(100);
    servo_enable();
    HAL_Delay(2000); // Let motors settle
    //servo_disable();
    //HAL_Delay(2000); // Let motors settle

}
