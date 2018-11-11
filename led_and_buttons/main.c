#include "main.h"

#define SWITCH_DELAY 10000
#define LED_COLORS_LENGTH 10
#define DALAYS_LENGTH 16

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* LEDs array to toggle between them */
  //
  /* LEDs colors */
  uint32_t leds_colors[LED_COLORS_LENGTH] = {GPIO_Pin_8, GPIO_Pin_8 | GPIO_Pin_9, GPIO_Pin_8 | GPIO_Pin_10,
                                             GPIO_Pin_9, GPIO_Pin_9 | GPIO_Pin_8, GPIO_Pin_9 | GPIO_Pin_10,
                                             GPIO_Pin_10, GPIO_Pin_10 | GPIO_Pin_8, GPIO_Pin_10 | GPIO_Pin_9,
                                             GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10};
  /* Color to toggle during iteration */
  uint8_t current_color = 0;
  /* LED to toggle during iteration */
  //uint8_t  current_led = 0;
  /* flag to disable buttons */
  char is_on = 1;
  /* flag to slowly wake up LEDs on color change*/  
  char first_time = 1;
  /* magicly choosen delays for slowly wake uping LEDs */
  uint32_t delays[DALAYS_LENGTH] = {250000,230000,210000,190000,170000,150000,130000,110000,90000,70000,
                                     60000,50000,40000,30000,20000,10000};

  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable peripheral clock for buttons port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

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

  /* Turn all the leds off */
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);

  while (1)
  {
    int i, k;
    char is_changed_0 = 0, 
         is_changed_1 = 0;

    /* Read input from buttons */
    for (i = 0; i < SWITCH_DELAY; i++)
    {
       if (!is_changed_0 && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0))
       {
          if (++current_color > LED_COLORS_LENGTH)
          {
             current_color = 0;
          }
          is_changed_0 = 1;
          first_time = 1;
       }
       if (!is_changed_1 && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1))
       {          
          if (is_on) // why is_on didn't work?
          {
             is_on = 0;
          }
          else
          {
             is_on = 1;
          }
          is_changed_1 = 1;
          first_time = 1;
       }
    }
    
    /* Set LEDs */
    if(is_on)
    {
       /* Slowly wake up LEDs */
       for (k = 0; first_time &&  k < DALAYS_LENGTH; k++)
       {
          // Switch the LED off
          GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
          for (i = 0; i < delays[k]; i++);

          // Switch the LED on
          GPIO_ResetBits(GPIOA, leds_colors[current_color]);
          for (i = 0; i < delays[DALAYS_LENGTH - 1 - k]; i++);
       }
       first_time = 0;
       
       //  Switch the LED off 
       GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
       for (i = 0; i < SWITCH_DELAY; i++);

       // Switch the LED on 
       GPIO_ResetBits(GPIOA, leds_colors[current_color]);      
    }
    else 
    {
       //  Switch the LED off 
       GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
    }   
  }
}
