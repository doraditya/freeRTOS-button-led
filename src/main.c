/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "task.h"

#define TRUE 			1
#define FALSE			0
#define NOT_PRESSED		FALSE
#define PRESSED 		TRUE

void printmsg(char *msg);
static void prvSetupHardware(void);
static void prvSetupUART(void);
void prvSetupGPIO(void);

void led_task_handler(void *params);
void button_task_handler(void *params);

 uint8_t button_status_flag = NOT_PRESSED;
int main(void)
{
	//Reset RCC clock
	RCC_DeInit();
	SystemCoreClockUpdate();

	prvSetupHardware();

	//led task
	xTaskCreate(led_task_handler,"LED-TASK",configMINIMAL_STACK_SIZE,NULL,1,NULL);
	//button task
	xTaskCreate(button_task_handler,"BUTTON-TASK",configMINIMAL_STACK_SIZE,NULL,1,NULL);
	// Start scheduler
	vTaskStartScheduler();

	for(;;);
}

void led_task_handler(void *params)
{
	while(1)
	{
		if(button_status_flag == PRESSED)
		{
			GPIO_WriteBit(GPIOD,GPIO_Pin_15,Bit_SET);
		}
		else
		{
			GPIO_WriteBit(GPIOD,GPIO_Pin_15,Bit_RESET);
		}
	}
}

void button_task_handler(void *params)
{
	while(1)
	{
		if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			button_status_flag = NOT_PRESSED;
		}
		else
		{
			button_status_flag = PRESSED;
		}
	}
}


static void prvSetupUART(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;
	//1. Enable USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	// PA2
	memset(&gpio_uart_pins,0,sizeof(gpio_uart_pins));
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	// UART pheripheral configuration
	memset(&uart2_init,0,sizeof(gpio_uart_pins));
	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&uart2_init);

	USART_Cmd(USART2,ENABLE);
}
static void prvSetupHardware(void)
{
	//initialize and setup gpios to be used for the applciation
	prvSetupGPIO();
	//initialize and setup UART for debugging
	prvSetupUART();
}

void printmsg(char *msg)

{
	for(uint32_t i=0;i<strlen(msg);i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
		USART_SendData(USART2,msg[i]);
	}

}

void prvSetupGPIO(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	GPIO_InitTypeDef led_init, button_init;
	led_init.GPIO_Mode = GPIO_Mode_OUT;
	led_init.GPIO_OType = GPIO_OType_PP;
	led_init.GPIO_Pin = GPIO_Pin_15;
	led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	led_init.GPIO_Speed = GPIO_Low_Speed;

	GPIO_Init(GPIOD,&led_init);

	button_init.GPIO_Mode = GPIO_Mode_IN;
	button_init.GPIO_OType = GPIO_OType_PP;
	button_init.GPIO_Pin = GPIO_Pin_0;
	button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	button_init.GPIO_Speed = GPIO_Low_Speed;

	GPIO_Init(GPIOA,&button_init);



}
