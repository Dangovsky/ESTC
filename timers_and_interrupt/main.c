#include "main.h"

#define TIM3_PERIOD 1000
#define TIM3_PRESCALER 0xffff
#define TIM2_PERIOD 2000
#define TIM2_PRESCALER 42000

#define LED_COLORS_LENGTH 10
#define PERIOD 2000
#define PRESCALER 42000

/* Color to toggle during iteration */
uint8_t current_color = 0;
/* current_color addend. 1 or -1.       *
 * To define direction of color rotation*/
uint8_t addend = 1;
/* LEDs colors */
uint32_t leds_colors[LED_COLORS_LENGTH] = {GPIO_Pin_8, GPIO_Pin_8 | GPIO_Pin_9, GPIO_Pin_8 | GPIO_Pin_10,
                                           GPIO_Pin_9, GPIO_Pin_9 | GPIO_Pin_8, GPIO_Pin_9 | GPIO_Pin_10,
                                           GPIO_Pin_10, GPIO_Pin_10 | GPIO_Pin_8, GPIO_Pin_10 | GPIO_Pin_9,
                                           GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10};
/* Counts timer ticks*/
uint32_t timer_cnt = 0;
/* Remember timer_cnt on button click*/
uint32_t timer_first = 0;
uint32_t timer_second = 0;

void TIM3_IRQHandler(void)
{
   if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
   {
      timer_cnt++;
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
   }
}

void TIM2_IRQHandler(void)
{
   if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
   {      
      current_color += addend;
      if (current_color > LED_COLORS_LENGTH - 2)
      {
         if(addend == 1) 
            current_color = 0;
         else 
            current_color = LED_COLORS_LENGTH - 2;
      }
      //  Switch the LED off
      GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
      // Switch the LED on
      GPIO_ResetBits(GPIOA, leds_colors[current_color]);
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
   }
}

void EXTI0_IRQHandler(void)
{   
   if (EXTI_GetITStatus(EXTI_Line0) != RESET)
   {
      if (timer_cnt - timer_first < 1000)
      {
         //  Switch the LED off
         GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
         // Switch the LED on
         GPIO_ResetBits(GPIOA, leds_colors[LED_COLORS_LENGTH - 1]);
      }
      timer_first = timer_cnt;
      EXTI_ClearITPendingBit(EXTI_Line0);
   }
}

void EXTI1_IRQHandler(void)
{   
   if (EXTI_GetITStatus(EXTI_Line1) != RESET)
   {  
      if (timer_cnt - timer_second < 1000)
      {
         /* Change direction of color rotation*/
         addend = -addend;
      }
      timer_second = timer_cnt;
      EXTI_ClearITPendingBit(EXTI_Line1);      
   }
}

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  TIM_TimeBaseInitTypeDef tim_struct = {0};
  NVIC_InitTypeDef nvic_struct = {0};
  EXTI_InitTypeDef EXTI_InitStruct = {0};
  
  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable peripheral clock for buttons port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  /* Enable peripheral clock for timer 3*/
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* Enable peripheral clock for timer 2*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Enable peripheral clock for SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Init LEDs */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
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
  /* Init timer 3 */
  tim_struct.TIM_Period        = TIM3_PERIOD - 1;
  tim_struct.TIM_Prescaler     = TIM3_PRESCALER - 1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &tim_struct);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM3, ENABLE);
  /* Configurate interrupts for timer 3 */
  nvic_struct.NVIC_IRQChannel = TIM3_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
  /* Init timer 2*/
  tim_struct.TIM_Period = TIM2_PERIOD - 1;
  tim_struct.TIM_Prescaler = TIM2_PRESCALER - 1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &tim_struct);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  /*  Configurate interrupts for timer 2 */
  nvic_struct.NVIC_IRQChannel = TIM2_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
  /* Using GPIOE0 as EXTI_Line0 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
  /* Init EXTI_Line0 */
  EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStruct);
  /*  Configurate interrupts for button 0 */
  nvic_struct.NVIC_IRQChannel = EXTI0_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 0;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
  /* Using GPIOE0 as EXTI_Line1 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);
  /* Init EXTI_Line1 */
  EXTI_InitStruct.EXTI_Line = EXTI_Line1;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStruct);
  /*  Configurate interrupts for button 1 */
  nvic_struct.NVIC_IRQChannel = EXTI1_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 0;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);

  while (1)
  {
  }
}
