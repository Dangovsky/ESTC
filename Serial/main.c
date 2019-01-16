/*******************************************************************/
// Цепляем нужные файлы
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"



/*******************************************************************/
// Объявляем переменные
GPIO_InitTypeDef gpio;
USART_InitTypeDef usart;
// Пусть нам надо передать 8 байт, создадим массив для данных
uint8_t sendData[] = "Hello, world!";
uint8_t bytesToSend = sizeof(sendData);
// Счетчик отправленных байт
uint8_t sendDataCounter = 0;



/*******************************************************************/
// Инициализация всего, что нам надо
void initAll()
{
    // Включаем прерывания
    __enable_irq();

    // Запускаем тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // Инициализация нужных пинов контроллера, для USART1 –
    // PA9 и PA10
    GPIO_StructInit(&gpio);

    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);

    /* gpio.GPIO_Mode = GPIO_Mode_AF; */
    /* gpio.GPIO_Pin = GPIO_Pin_10; */
    /* gpio.GPIO_Speed = GPIO_Speed_50MHz; */
    /* gpio.GPIO_OType = GPIO_OType_PP; */
    /* gpio.GPIO_PuPd = GPIO_PuPd_UP; */
    /* GPIO_Init(GPIOA, &gpio); */

    // И вот еще функция, которой не было при работе с STM32F10x,
    // но которую нужно вызывать при использовании STM32F4xx
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

   // А теперь настраиваем модуль USART
    USART_StructInit(&usart);
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_BaudRate = 9600;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_WordLength = USART_WordLength_8b;


    USART_Init(USART1, &usart);

   // Включаем прерывания и запускаем USART
    NVIC_EnableIRQ(USART1_IRQn);
    USART_Cmd(USART1, ENABLE);
}



/*******************************************************************/
// Функция main()
int main()
{

    // Вызываем функцию инициализации
    initAll();

    // Включаем прерывание по окончанию передачи
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    while(1)
    {
    // А тут мы ничего не делаем, вся работа у нас в прерывании
        __NOP();
    }
}



/*******************************************************************/
// Обработчик прерывания
void USART1_IRQHandler()
{
    // Проверяем, действительно ли прерывание вызвано окончанием передачи
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        // Очищаем флаг прерывания
        USART_ClearITPendingBit(USART1, USART_IT_TC);

        // Отправляем байт данных
        USART_SendData(USART1, sendData[sendDataCounter]);

        // Увеличиваем счетчик отправленных байт
        sendDataCounter++;

        // Если отправили все данные, начинаем все сначала
        if  (sendDataCounter == bytesToSend)
        {
            sendDataCounter = 0;
        }
    }
}



/*******************************************************************/
