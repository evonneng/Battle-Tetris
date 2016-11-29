#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "UART.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

int main(void) {
	TExaS_Init();  // set system clock to 80 MHz
  Random_Init(1);

  ST7735_InitR(INITR_REDTAB);
  ADC_Init();    // initialize to sample ADC1
  UART_Init();       // initialize UART
}
