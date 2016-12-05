// UART.c
// Runs on LM4F120/TM4C123
// Provide functions that setup and interact with UART
// Last Modified: 4/12/2016 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Fifo.h"

// UART initialization function 
// Input: none
// Output: none
void UART_Init(void){ 
	//Transmission Portion
	
	SYSCTL_RCGCUART_R |= 0x0002;   // activate UART1
	SYSCTL_RCGCGPIO_R |= 0x0004; // activate port C
	int delay;
	delay = 420;
	//MAP: 0x4000D000 - 0x4000D044 R/W
	UART1_CTL_R &= ~0x00000001; // disable UART
	UART1_IBRD_R = 50;
	// IBRD = int(80000000/(16*100,000)) = 50
	UART1_FBRD_R = 0;
	// FBRD = round(0 * 64) = 0
	UART1_LCRH_R = 0x0070;  // 8-bit length, enable FIFO
  UART1_CTL_R = 0x0301;   // enable RXE, TXE and UART
  GPIO_PORTC_AFSEL_R |= 0x30; // alt funct on PC5-4
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
  GPIO_PORTC_DEN_R |= 0x30;   // digital I/O on PC5-4
  GPIO_PORTC_AMSEL_R &= ~0x30; // No analog on PC5-4
	
	//Receiving Portion
	FiFo_Init();
	UART1_IM_R |= 0x10;
	UART1_IFLS_R &= ~0x28;
	UART1_IFLS_R |= 0x04;
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFF00FFFF)|0x00100000; //UART1 Interrupt Priority 1
	NVIC_EN0_R |= 0x40; //Enable interrupts
}

//------------UART_InChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
// *** Need not be busy-wait if you know when to call
char UART_InChar(void){  
	while((UART1_FR_R & 0x0010) != 0){ //wait for RXFE to be 0
	}
	return (UART1_DR_R & 0xFF);
}

//------------UART_OutChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
void UART_OutChar(char data){  
	while((UART1_FR_R & 0x0020) != 0){ //wait for TXFF to be 0
	}
	UART1_DR_R = data;
}

void UART1_Handler(void){
	while((UART1_FR_R & 0x10) != 0){
	}
	FiFo_Put(UART_InChar());
	UART1_ICR_R = 0x10; //Acknowledge interrupt
}

char send_index = 4;
char receive_index = 0;

void UART_send_acknowledge(void) {
	UART_OutChar(0x2);
	UART_OutChar(send_index++);
	for(int i = 0; i < 5; i++)
		UART_OutChar('A');
	UART_OutChar(0x3);
}

// -1 = no messages, 0 = acknowledge msg, 1 = real message
int UART_receive_message(char* msg) {
	char header;
	if(FiFo_Get(&header) == 0) {
		return -1;
	}
	/*if(header != 0x2) {
		while(FiFo_Get(&header) != 0 && header != 0x3);
		return -1;
	}*/
	char receive;
	FiFo_Get(&receive);
	int return_value;
	char junk;
	if(receive != receive_index) {
		FiFo_Get(msg);
		receive_index = receive;
		return_value = 1;
	} else {
		FiFo_Get(&junk);
		return_value = -1;
	}
	for(int i = 0; i < 5; i++)
		FiFo_Get(&junk);
	if(return_value == 1 && *msg == 'A')
		return 0;
	UART_send_acknowledge();
	return return_value;
}

void UART_send_message(uint8_t x) {
	char receive;
	do {
		if(x == 'R' && receive == 'R')
			break;
		UART_OutChar(0x2);
		UART_OutChar(send_index);
		for(int i = 0; i < 5; i++)
			UART_OutChar(x);
		UART_OutChar(0x3);
	} while(UART_receive_message(&receive) != 0);
	send_index++;
}