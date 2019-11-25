// CPE287 Lab7 main
// James Lumpp
// 10/21/2019

#include "PLL.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"

//IMPORT   red_led_on
//IMPORT   red_led_off

// Define port specific addressing for some of he Port F pins
#define PF1 (*((unsigned long volatile *)(0x40025008))) //0 0010 00
#define RED_LED PF1
#define PF4 (*((unsigned long volatile *)(0x40025040))) //1 0000 00
#define SW1 PF4


// Prototypes for functions in other files 
void Delay(uint32_t);
uint32_t Ultrasonic_Echo(void);
void Distance_Report(uint32_t);
uint32_t Ultrasonic_Ticks_to_CM(uint32_t);       // assembly version

// Functions you will write
void PortB_Init(void);                           // C version
uint32_t Ultrasonic_Distance(void);
uint32_t Ultrasonic_SysTicks_to_CM(uint32_t);    // C version

// delay function for testing from sysctl.c
// which delays 3*ulCount cycles
                            //Keil uVision Code example for in-line assembly
	__asm void                // ignore the red X
	Delay(uint32_t ulCount)
	{
    subs    r0, #1          // ignore the red X
    bne     Delay
    bx      lr
	}
	
void Test_PortB_Init_C(void){	// test code for PortB_Init_C
	while (1){

		uint32_t X;	// INSTANTIATE X
		X = GPIO_PORTB_DATA_R;	// X = GPIO DATA
		X &= 0x08;	// X = output pin
		X = X<<1;	// shift
		GPIO_PORTB_DATA_R &= ~0x10;	// gpio = input pin
		GPIO_PORTB_DATA_R |= X;	// or 
		}
	
	}
	
void SysTick_Init(void){
           // disable SysTick during setup
    NVIC_ST_CTRL_R = 0;
           // set reload to maximum reload value
    NVIC_ST_RELOAD_R = 0x00FFFFFF;
           // writing any value to CURRENT clears it
    NVIC_ST_CURRENT_R = 0;
           // enable SysTick with core clock
    NVIC_ST_CTRL_R = 0x0005;    //Enable but no interrupts (later)
                                //ENABLE and CLK_SRC bits set
}

// *************** Report_Distance ***************
// Flash the LED for 1/2 seond for every 10 cm and .2 seconds for every cm modulo 10
// Inputs: integer numer of centemeters
// Outputs: None
// Side Effect: Flashes RED LED 

void Report_Distance_AsmEdit(uint32_t count){
	uint32_t i;
	
//	for (i=0;i<(count);i++){

//	  GPIO_PORTF_DATA_R |= 0x03;  // turn on RED for every 10 cm
//    Delay(13333333);            // delay ~0.5 sec at 80 MHz
//    GPIO_PORTF_DATA_R &= ~0x03; // turn off RED
//    Delay(13333333);            // delay ~0.5 sec at 80 MHz
//	}

	for (i=0;i<(count);i++){
		GPIO_PORTF_DATA_R |= 0x03;  // turn on RED for each cm mod 10
    Delay(5333333);             // delay ~0.2 sec at 80 MHz
    GPIO_PORTF_DATA_R &= ~0x03; // turn off RED
    Delay(5333333);             // delay ~0.2 sec at 80 MHz
	}
		
}

// *************** Report_Distance ***************
// Flash the LED for 1/2 seond for every 10 cm and .2 seconds for every cm modulo 10
// Inputs: integer numer of centemeters
// Outputs: None
// Side Effect: Flashes blue LED 

void Report_Distance(uint32_t count){
	uint32_t i;
	
//	for (i=0;i<count;i++){

//	  GPIO_PORTF_DATA_R |= 0x04;  // turn on Blue for every 10 cm
//    Delay(13333333);            // delay ~0.5 sec at 80 MHz
//    GPIO_PORTF_DATA_R &= ~0x04; // turn off Blue
//    Delay(13333333);            // delay ~0.5 sec at 80 MHz
//	}

	for (i=0;i<(count);i++){
		GPIO_PORTF_DATA_R |= 0x04;  // turn on Blue for each cm mod 10
    Delay(5333333);             // delay ~0.2 sec at 80 MHz
    GPIO_PORTF_DATA_R &= ~0x04; // turn off Blue
    Delay(5333333);             // delay ~0.2 sec at 80 MHz
	}
		
}

// Subroutine to initialize port F pins for input and output
// PF3-1 set for digital output (LEDs) and PF4, PF0 input with pull-up (SW2-1)
// Inputs: None
// Outputs: None

void PortF_Init(void){ 
	volatile uint32_t delay;

  SYSCTL_RCGC2_R     |= 0x00000020; // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           //    allow time for clock to start
  delay = delay;                    //    allow time for clock to start
  GPIO_PORTF_LOCK_R   = 0x4C4F434B; // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R    |= 0x1F;       //    allow changes to PF4-0 (really only PF0 is locked)
  GPIO_PORTF_AMSEL_R &= ~0x01F;     // 3) disable analog on PF4-0
  GPIO_PORTF_PCTL_R  &= ~0x000FFFFF;// 4) PCTL set to GPIO on PF4-0
  GPIO_PORTF_DIR_R   |= 0x0E;       // 5) PF3-1 output
  GPIO_PORTF_DIR_R   &= ~0x11;      //    PF4,PF0 input
  GPIO_PORTF_AFSEL_R &= ~0x1F;      // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R   |= 0x11;       // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R   |= 0x1F;       // 7) enable digital I/O on PF4-0
}

// Main function to test lab code
// 
int main(void){  
uint32_t distance_c, distance_asm;
uint32_t ticks_c, ticks_asm;
uint32_t difference;
	
  PLL_Init();
	PortB_Init_C();
	Ultrasonic_Distance();
	//Test_PortB_Init_C();
	PortF_Init();
	SysTick_Init();	
	
  while(1){
    GPIO_PORTF_DATA_R |= 0x08;         // turn on GREEN to let user know code has started
        
		while(SW1 != 0)                    // wait for a button press
		  {};                // same as while((GPIO_PORTF_DATA_R&0x80) != 0)  {}; but easier 
				                 // to read with port specific addressing             
				
	  GPIO_PORTF_DATA_R &= ~0x08;        // turn off GREEN
				
		ticks_c = Ultrasonic_Distance();// get distance from C funtion
		ticks_asm = Ultrasonic_Echo();  // get distance from assembly function
		distance_c = Ultrasonic_SysTicks_to_CM(ticks_c);
		distance_asm = Ultrasonic_SysTicks_to_CM(ticks_asm);
		Report_Distance(distance_c);       // report C distance with C function
		Report_Distance_AsmEdit(distance_asm);     //report asm distance with asm function
				
		difference = distance_c - distance_asm;     // BREAKPONT HERE or uncomment the following line
		difference = difference;                    // avoid compiler warning about unused variable
		Report_Distance(difference);                // display the differene in cm

	}  // while(1)

}  // main
