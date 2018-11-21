#include "main.h"

#include <string.h>

#define LED_COLORS_LENGTH 10
#define TIM1_PERIOD 4000
#define TIM1_PRESCALER 168
#define TIM1_PULSE 0
#define TIM2_PERIOD 4000
#define TIM2_PRESCALER 42000

uint8_t current_color;
/* LEDs colors */
uint32_t leds_colors[LED_COLORS_LENGTH] = {GPIO_Pin_8, GPIO_Pin_8 | GPIO_Pin_9, GPIO_Pin_8 | GPIO_Pin_10,
                                           GPIO_Pin_9, GPIO_Pin_9 | GPIO_Pin_8, GPIO_Pin_9 | GPIO_Pin_10,
                                           GPIO_Pin_10, GPIO_Pin_10 | GPIO_Pin_8, GPIO_Pin_10 | GPIO_Pin_9,
                                           GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10};

uint32_t compare = 1000;

void TIM2_IRQHandler(void)
{
   if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
   {
/*      if (++current_color > LED_COLORS_LENGTH - 2)
      {
         current_color = 0;
      }
      //  Switch the LED off
      GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
      // Switch the LED on
      GPIO_ResetBits(GPIOA, leds_colors[current_color]);
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
*/
   }
}

void EXTI0_IRQHandler(void)
{   
   if (EXTI_GetITStatus(EXTI_Line0) != RESET)
   {     
      //  Switch the LED off
      GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
      // Switch the LED on
      GPIO_ResetBits(GPIOA, leds_colors[LED_COLORS_LENGTH - 1]);
      EXTI_ClearITPendingBit(EXTI_Line0);      
   }
}

void EXTI1_IRQHandler(void)
{   
   if (EXTI_GetITStatus(EXTI_Line1) != RESET)
   {     
      compare += compare;
      TIM_SetCompare1(TIM1, compare);
      TIM_SetCompare2(TIM1, compare);
      TIM_SetCompare3(TIM1, compare);
      EXTI_ClearITPendingBit(EXTI_Line1);      
   }
}

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef tim_struct;
  NVIC_InitTypeDef nvic_struct;
  EXTI_InitTypeDef EXTI_InitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  
  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable peripheral clock for buttons port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  /* Enable peripheral clock for timer 2*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Enable peripheral clock for timer 2*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  /* Enable peripheral clock for SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

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
  /* Init buttons */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  /* Init timer 2 */
  tim_struct.TIM_Period        = TIM2_PERIOD - 1;
  tim_struct.TIM_Prescaler     = TIM2_PRESCALER - 1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &tim_struct);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  /*  Configurate interrupts for timer 2 */
  nvic_struct.NVIC_IRQChannel                   = TIM2_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority        = 1;
  nvic_struct.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&nvic_struct);
  /* Using GPIOE0 as EXTI_Line0 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
  /* Init EXTI_Line0 */
  EXTI_InitStruct.EXTI_Line    = EXTI_Line0;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStruct);
  /*  Configurate interrupts for button 0 */
  nvic_struct.NVIC_IRQChannel                   = EXTI0_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority        = 0;
  nvic_struct.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&nvic_struct);
  /* Using GPIOE0 as EXTI_Line1 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);
  /* Init EXTI_Line1 */
  EXTI_InitStruct.EXTI_Line    = EXTI_Line1;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStruct);
  /*  Configurate interrupts for button 1 */
  nvic_struct.NVIC_IRQChannel                   = EXTI1_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority        = 0;
  nvic_struct.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&nvic_struct);
  /* Init timer 1 */
  tim_struct.TIM_Period        = TIM1_PERIOD - 1;
  tim_struct.TIM_Prescaler     = TIM1_PRESCALER - 1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &tim_struct);

  TIM_OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_Low;
  TIM_OCInitStruct.TIM_Pulse       = TIM1_PULSE;

  TIM_OC1Init(TIM1, &TIM_OCInitStruct);
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC2Init(TIM1, &TIM_OCInitStruct);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC3Init(TIM1, &TIM_OCInitStruct);
  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
 
  TIM_SetCompare1(TIM1, compare);
  TIM_SetCompare2(TIM1, compare);
  TIM_SetCompare3(TIM1, compare);

  TIM_CtrlPWMOutputs(TIM1, ENABLE);
 
  TIM_Cmd(TIM1, ENABLE);

  while (1)
  {
  }
}
