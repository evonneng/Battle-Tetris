#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "UART.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 2000000-1;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007;
}

void SysTick_Handler(void) {
	
}

int main(void) {
	TExaS_Init();  // set system clock to 80 MHz
  Random_Init(1);

  ST7735_InitR(INITR_REDTAB);
  ADC_Init();    // initialize to sample ADC1
  UART_Init();       // initialize UART
	SysTick_Init();
	//TODO: initialize buttons
	while(1) {
		
	}
}
