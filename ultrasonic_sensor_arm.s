;****************** ultrasonic_sensor_arm.s ***************
; CPE287, Fall 2019, Lab 6
; NAME: Gabe Rivera
; DATE: 10/08/19
; DESCRIPTION: This file holds my solutions to the fucntions PortB_Init and Ultrasonic_Echo.
;*****************************************


;//
;//
;// GPIO registers EQU PORTB
;//
;//
GPIO_PORTB_DATA_BITS_R  EQU  0x40005000
GPIO_PORTB_DATA_R       EQU  0x400053FC
GPIO_PORTB_DIR_R        EQU  0x40005400
GPIO_PORTB_IS_R         EQU  0x40005404
GPIO_PORTB_IBE_R        EQU  0x40005408
GPIO_PORTB_IEV_R        EQU  0x4000540C
GPIO_PORTB_IM_R         EQU  0x40005410
GPIO_PORTB_RIS_R        EQU  0x40005414
GPIO_PORTB_MIS_R        EQU  0x40005418
GPIO_PORTB_ICR_R        EQU  0x4000541C
GPIO_PORTB_AFSEL_R      EQU  0x40005420
GPIO_PORTB_DR2R_R       EQU  0x40005500
GPIO_PORTB_DR4R_R       EQU  0x40005504
GPIO_PORTB_DR8R_R       EQU  0x40005508
GPIO_PORTB_ODR_R        EQU  0x4000550C
GPIO_PORTB_PUR_R        EQU  0x40005510
GPIO_PORTB_PDR_R        EQU  0x40005514
GPIO_PORTB_SLR_R        EQU  0x40005518
GPIO_PORTB_DEN_R        EQU  0x4000551C
GPIO_PORTB_LOCK_R       EQU  0x40005520
GPIO_PORTB_CR_R         EQU  0x40005524
GPIO_PORTB_AMSEL_R      EQU  0x40005528
GPIO_PORTB_PCTL_R       EQU  0x4000552C
GPIO_PORTB_ADCCTL_R     EQU  0x40005530
GPIO_PORTB_DMACTL_R     EQU  0x40005534

SYSCTL_RCGC2_R          EQU  0x400FE108
NVIC_ST_CURRENT_R  		EQU    0xE000E018





        AREA    |.text|, CODE, READONLY, ALIGN=2
		PRESERVE8
		REQUIRE8
        THUMB
			
        EXPORT   PortB_Init
        EXPORT   Ultrasonic_Echo
        EXPORT   Distance_Report
		EXPORT 	 Test_B_init
		EXPORT   Clock_Freq_Test
		IMPORT   red_led_on
		IMPORT   red_led_off
	
Clock_Freq_Test
	LDR R1, =GPIO_PORTB_DATA_R
theloop
	LDR R2, [R1]
	EOR R2, R2, #0x10
	STR R2, [R1]
	LDR R0, =5333
spin_loop
	SUBS R0, R0, #1
	BNE spin_loop
	B theloop
	BX LR		
				
Test_B_init
	LDR R1, =GPIO_PORTB_DATA_R
bloop
	LDR R2, [R1]
	LSL R3, R2, #1 ;LSL instead of LSR
	BIC R2, R2, #0x10 ; clear pb4
	ORR R2, R2, R3
	STR R2, [R1]
	B bloop
	BX LR			
					
;------------PortB_Init------------
; Initialize GPIO Port B bit 4 (PB4) as a digital output and 
; Port B Pin 3 (PB3) as a digital input for use with ultrasonic sensor 
; NOTE: This function should only configures PB3 and PB4 and thus 
; must preserve the contents of any of the other control registers.
; Input: none
; Output: none
; Modifies: R0, R1, R2

; UNCOMMENT ALL THE LINES BELOW TO GENERATE YOUR SOLUTION

PortB_Init
    LDR R1, =SYSCTL_RCGC2_R         ; 1) activate clock for Port B
    LDR R0, [R1]                 
    ORR R0, R0, #0x02              ; set bit ___ to turn on clock for PortB
    STR R0, [R1]                  
    NOP
    NOP                             ; allow time for clock to finish startup
    LDR R1, =GPIO_PORTB_AMSEL_R     ; 3) disable analog functionality
    LDR R0, [R1]
    BIC R0,R0, #0x18             ; AMSEL off for bits 3,4(0 for analog off)
    STR R0, [R1]                    
    LDR R1, =GPIO_PORTB_PCTL_R      ; 4) configure as GPIO
    LDR R0, [R1]
    BIC R0, R0, #0x18           ; PB3,4 GPIO: 0 means config as GPIO
    STR R0, [R1]                
    LDR R1, =GPIO_PORTB_DIR_R       ; 5) set direction register
    LDR R0, [R1]
    ORR R0, R0, #0x10           ; PB4 set as output, PB3 as input
    BIC R0, R0, #0x08          ; PB4 set as output, PB3 as input
    STR R0, [R1]                    
    LDR R1, =GPIO_PORTB_AFSEL_R     ; 6) regular port function
    LDR R0, [R1]
    BIC R0, R0, #0x18             ; No AF PB3,4, 0 means disable alt. func.
    STR R0, [R1]                    
    LDR R1, =GPIO_PORTB_DEN_R       ; 7) enable Port B digital port
    LDR R0, [R1]
    ORR R0, R0, #0x18             ; 8) Enable PB3,4: 1 means enable digital I/O
    STR R0, [R1]                   
    BX  LR

;------------Ultrasonic_Echo------------
; Place your solution below

Ultrasonic_Echo
	PUSH {R1-R12, LR}
	LDR R4, =GPIO_PORTB_DATA_R	; load port b data
	LDR R5, =NVIC_ST_CURRENT_R	; load systick current value
	LDR R1, =125 ; value to be decrimented 
	LDR R2, [R4]
	ORR R2, R2, #0x10
	STR R2, [R4]
loop
	SUB R1, R1, #1	
	CMP R1, #0
	BNE loop
	LDR R2, [R4]
	BIC R2, R2, #0x10 ; bitclear for PB4
	STR R2, [R4]
wait_for_press
	STR R5, [R5]
	LDR R2, [R4]
	ANDS R2, R2, #0x08	; AND PB3 to look for a 1
	BEQ wait_for_press ; if 0 loop back in wait_for_press
	LDR R6, [R5]
wait_for_release
	LDR R2, [R4]
    ANDS R2, R2, #0x08 
	BNE wait_for_release ; if 1 loop back to wait_for_release
	LDR R7, [R5]
	SUB R8, R6, R7
	MOV R0, R8
	;BP test
	POP {R1-R12, LR}
    BX LR
	
    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
