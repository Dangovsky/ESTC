#include "main.h"

#define LED_COLORS_LENGTH 10
#define TIM1_PERIOD 16400
#define TIM1_PRESCALER 1
#define TIM1_PULSE 16401
#define STEP 64
#define TIM2_PERIOD 10
#define TIM2_PRESCALER 0xffff

/* color to incriment */
static uint8_t current_color;
/* system led colors RGB */
static uint32_t sys_leds[3];
/* timer for buttons*/
static uint32_t timer_count;
static uint32_t timer_firstb;
static uint32_t timer_secb;
/* comparator for TIM1*/
static uint32_t comparator;
static uint8_t green_comp;

void TIM2_IRQHandler(void)
{
   if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
   {
      timer_count++;
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
   }
}


void EXTI0_IRQHandler(void)
{  
   if (EXTI_GetITStatus(EXTI_Line0) != RESET)
   {
      if (timer_count - timer_firstb > 100)
      {
         if (++current_color > 2)
         {
            current_color = 0;
         }
         /* Turn all the leds off */
         GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
         /* Turn on corrent color*/
         GPIO_SetBits(GPIOD, sys_leds[current_color]);

         timer_count = timer_firstb;
      }
      EXTI_ClearITPendingBit(EXTI_Line0);         
   }
}

void EXTI1_IRQHandler(void)
{   
   if (EXTI_GetITStatus(EXTI_Line1) != RESET)
   {
      switch (current_color)
      {
         case 0:
            comparator = TIM_GetCapture1(TIM1);
            comparator += STEP;
            if (comparator > 16400)
               comparator = 0;
            TIM_SetCompare1(TIM1, comparator);
            break;
         case 1:
            /* comments below does not work */

            /*comparator = TIM_GetCapture2(TIM1);
             *comparator += STEP;
             *if (comparator > 16400)
             *   comparator = 0; 
             */
           /* 9454 === 0x00 of brightness 
             * comparator = (uint32_t)(9454.1 + (6945.8 / 16400.0 * comparator));
             * comparator = 9454 + (uint32_t)(0.4234 * comparator); 
             * TIM_SetCompare2(TIM1, 9454 + (uint32_t)(0.4234 * comparator)); 
             */

            /* 147 === 0x00 of brightness */ 
            /* 255 - 147 = 108 brightness steps left*/
            green_comp++;
            TIM_SetCompare2(TIM1, STEP * (147 + (uint8_t)(108.0 / 255.0 * green_comp))); 
            break;
         case 2:
            comparator = TIM_GetCapture3(TIM1);
            comparator += STEP;
            if (comparator > 16400)
               comparator = 0;
            TIM_SetCompare3(TIM1, comparator);
            break;
      }
      EXTI_ClearITPendingBit(EXTI_Line1);      
   }
}

int main(void)
{
  current_color = 0;    
  sys_leds[0] = GPIO_Pin_14;
  sys_leds[1] = GPIO_Pin_12; 
  sys_leds[2] = GPIO_Pin_15;
  timer_count = 0;
  timer_firstb = 0;
  timer_secb = 0;
  green_comp = 0;

  GPIO_InitTypeDef GPIO_InitStructure = {0};
  TIM_TimeBaseInitTypeDef tim_struct = {0};
  TIM_OCInitTypeDef TIM_OCInitStruct = {0};
  EXTI_InitTypeDef EXTI_InitStruct = {0};  
  NVIC_InitTypeDef nvic_struct = {0};

  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable peripheral clock for timer 1*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  /* Enable peripheral clock for timer 2*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Enable peripheral clock for buttons */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  /* Enable peripheral clock for SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Clock for GPIOD */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Init standart leds */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  /* Init buttons */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  /* Using GPIOE0 as EXTI_Line0 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
  /* Init EXTI_Line0 */
  EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStruct);
  /* Configurate interrupts for button 0 */
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
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
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
  /* Init timer 2 */
  tim_struct.TIM_Period        = TIM2_PERIOD - 1;
  tim_struct.TIM_Prescaler     = TIM2_PRESCALER - 1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &tim_struct);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  /* Configurate interrupts for timer 2 */
  nvic_struct.NVIC_IRQChannel = TIM2_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
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
  TIM_OCInitStruct.TIM_Pulse       = 0;

  TIM_OC1Init(TIM1, &TIM_OCInitStruct);
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC2Init(TIM1, &TIM_OCInitStruct);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_OC3Init(TIM1, &TIM_OCInitStruct);
  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

  TIM_CtrlPWMOutputs(TIM1, ENABLE);

  /* Turn all the leds off */
  GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  GPIO_SetBits(GPIOD, sys_leds[current_color]);

  while (1)
  {
/*
     for(uint32_t i = 0; i < 16400; i+=step){
         TIM_SetCompare1(TIM1, i);
         TIM_SetCompare2(TIM1, i);
         TIM_SetCompare3(TIM1, i);
         for (int k = 0; k < 160000; k++);
     }
*/
  }
}
