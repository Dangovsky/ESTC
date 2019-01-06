#include "main.h"
#define SWITCH_DELAY 10000
#define BUTTON_DELAY 750000
#define LED_COLORS_LENGTH 10
#define DELAYS_LENGTH 40

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure = {0};

  /* LEDs colors */
  uint32_t leds_colors[LED_COLORS_LENGTH] = {GPIO_Pin_8, GPIO_Pin_8 | GPIO_Pin_9, GPIO_Pin_8 | GPIO_Pin_10,
                                             GPIO_Pin_9, GPIO_Pin_9 | GPIO_Pin_8, GPIO_Pin_9 | GPIO_Pin_10,
                                             GPIO_Pin_10, GPIO_Pin_10 | GPIO_Pin_8, GPIO_Pin_10 | GPIO_Pin_9,
                                             GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10};
  /* Color to toggle during iteration */
  uint8_t current_color = 0;
  /* flag to disable buttons */
  char is_on = 1;
  /* flag to slowly wake up LEDs on color change*/  
  char first_time = 1;
  /* magicly choosen delays for slowly wake up LEDs */
  uint32_t delays[DELAYS_LENGTH];

  delays[0] = 10000;
  for (int i = 1; i < DELAYS_LENGTH; i++)
  {
     delays[i] = delays[i - 1] + 2000;
  }
  
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
    uint32_t i, k;
    char is_changed_0 = 0, 
         is_changed_1 = 0;

    /* Read input from buttons */

    for (i = 0; i < BUTTON_DELAY; i++)
    {
       // change color
       if (!is_changed_0 && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0))
       {
          if (++current_color > LED_COLORS_LENGTH)
          {
             current_color = 0;
          }
          is_changed_0 = 1;
          first_time = 1;
       }
       // on/off LEDs
       if (!is_changed_1 && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1))
       {          
          if (is_on) // why is_on didn't work?
          {
             is_on = 0;
          }
          else
          {
             is_on = 1;
             first_time = 1;
          }
          is_changed_1 = 1;
       }
    }
    
    /* Set LEDs */
    if(is_on)
    {
       /* Slowly wake up LEDs */
       for (k = 0; first_time &&  k < DELAYS_LENGTH; k++)
       {
          // Switch the LED off
          GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
          for (i = 0; i < delays[DELAYS_LENGTH - 1 - k]; i++);

          // Switch the LED on
          GPIO_ResetBits(GPIOA, leds_colors[current_color]);
          for (i = 0; i < delays[k]; i++);
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
