#include "../include/led.h"

#define TIM1_PERIOD 16400
#define TIM1_PRESCALER 1
#define TIM1_PULSE 16401
#define STEP 64

uint8_t InitLeds(){
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  TIM_TimeBaseInitTypeDef tim_struct = {0};
  TIM_OCInitTypeDef TIM_OCInitStruct = {0};

  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable peripheral clock for timer 1*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* Init LEDs */  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Init timer 1 */
  tim_struct.TIM_Period        = TIM1_PERIOD - 1;
  tim_struct.TIM_Prescaler     = TIM1_PRESCALER - 1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &tim_struct);
  TIM_Cmd(TIM1, ENABLE);

  TIM_OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM2;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_Pulse       = TIM1_PULSE - 1;

  TIM_OC1Init(TIM1, &TIM_OCInitStruct);
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC2Init(TIM1, &TIM_OCInitStruct);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC3Init(TIM1, &TIM_OCInitStruct);
  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  return 0;
}

uint8_t SetColor(uint32_t color)
{
  // Red
  TIM_SetCompare1(TIM1, ((color >> 16) & 0xFF) * STEP);
  // Green 
  uint8_t green =  ((color >> 8) & 0xFF);
  //0x92 = 147 === 0x00
  //255 - 147 = 108
  green = 147 + (uint8_t)(108.0 / 255.0 * green);
  TIM_SetCompare2(TIM1, green * STEP);
  // Blue
  TIM_SetCompare3(TIM1, (color & 0xFF) * STEP);
  return 0;
}