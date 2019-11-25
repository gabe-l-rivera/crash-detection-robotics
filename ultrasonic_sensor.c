//;****************** ultrasonic_sensor.c ***************
//; CPE287, Fall 2019, Lab 6
//; NAME: Gabe Rivera
//; DATE: 10/08/19
//; DESCRIPTION: This file holds my solutions to the fucntions PortB_Init and Ultrasonic_Echo, converted from ARM assembly to embedded C
//;*****************************************
#include <stdint.h>
#include "tm4c123gh6pm.h"

//------------PortB_Init_C------------
// Initialize GPIO Port B.  PB4 as a digital output and 
//  PB3 as a digital input
// Input: none
// Output: none

void PortB_Init_C (void){
	volatile uint32_t delay;

  SYSCTL_RCGC2_R     |= 0x2; // 1) activate clock for Port B = 0x02
  delay = SYSCTL_RCGC2_R;           //    allow time for clock to start
  delay = delay;                    //    allow time for clock to start
  GPIO_PORTB_AMSEL_R &= ~0x018;     // 3) disable analog on PB3 & PB4
  GPIO_PORTB_PCTL_R  &= ~0x000FF000;// 4) PCTL set to GPIO on PB3 & PB4
  GPIO_PORTB_DIR_R   |= 0x10;       // 5) PB4 output
  GPIO_PORTB_DIR_R   &= ~0x08;      //    PB3 input
  GPIO_PORTB_AFSEL_R &= ~0x18;      // 6) disable alt funct on PB3 & PB4
  GPIO_PORTB_PUR_R   |= 0x08;       // enable pull-up on PB3 and PB4
  GPIO_PORTB_DEN_R   |= 0x18;       // 7) enable digital I/O on PB3 & PB4
                   
}  // PortB_Init_C

uint32_t Ultrasonic_Distance (void)
{
	uint32_t systickCurrent, beforeSysticks, afterSysticks, finalSysticks;
	systickCurrent = NVIC_ST_CURRENT_R;
	
	GPIO_PORTB_DATA_R |= 0x10;
	int x = 500;
	while(x >= 0)
		x = x-1;
	GPIO_PORTB_DATA_R &= ~0x10;
	
	while((GPIO_PORTB_DATA_R&0x08) == 0){
		NVIC_ST_CURRENT_R = systickCurrent;
	};
		beforeSysticks = NVIC_ST_CURRENT_R;
	while((GPIO_PORTB_DATA_R&0x08) != 0){};
		afterSysticks = NVIC_ST_CURRENT_R;
	finalSysticks = beforeSysticks - afterSysticks;
		
	return(finalSysticks);
}	  // Ultrasonic_Distance


//------------Ultrasonic_SysTicks_to_CM------------
// Initialize GPIO Port B.  PB4 as a digital output and 
//  PB3 as a digital input
// Input: none
// Output: none
uint32_t Ultrasonic_SysTicks_to_CM(uint32_t ticks){
//	ASR R0, R0, #10  ; divide ticks by 0x400 (1K)
//	LSL R1, R0, #8   ; multiply ticks by 0x100
//	LSL R2, R0, #5   ; multiply ticks by 0x020
//	SUB R1, R1, R2
//	ADD R1, R0, R1   ;
//	ASR R0, R1, #10  ; divide result by 0x400 (1K)
//	BX  LR
	uint32_t ticksR0 = ticks;
	uint32_t ticksR1, ticksR2, a, b, c;
	a = 10;
	b = 8;
	c = 5;
	
	// may need to change ticks2 and tick3 to signed ints
	
	ticksR0 = ticksR0>>a; 
	ticksR1 = ticksR0<<b;
	ticksR2 = ticksR0<<c;
	ticksR1 = ticksR1-ticksR2;
	ticksR1 = ticksR0 + ticksR1;
	ticksR0 = ticksR1>>a;
	return(ticksR0);
} // Ultrasonic_SysTicks_to_CM



