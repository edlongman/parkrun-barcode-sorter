
#if F0
#include "stm32f0xx_hal.h"
#elif F1
#include "stm32f1xx_hal.h"
#else
#error "Unsupported STM32 Family"
#endif

#define Board_LED_Pin                                GPIO_PIN_13
#define Board_LED_GPIO_Port                          GPIOC
#define LED_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOC_CLK_ENABLE()

int main(void)
{

  HAL_Init();

  /* Configure the system clock */
  //SystemClock_Config();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LED_GPIO_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Board_LED_GPIO_Port, Board_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Board_LED_Pin */
  GPIO_InitStruct.Pin = Board_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(Board_LED_GPIO_Port, &GPIO_InitStruct);

  while (1)
  {
    HAL_GPIO_TogglePin(Board_LED_GPIO_Port, Board_LED_Pin);
    HAL_Delay(200);
  }
}


extern "C" void SysTick_Handler(void)
{
  HAL_IncTick();
}
