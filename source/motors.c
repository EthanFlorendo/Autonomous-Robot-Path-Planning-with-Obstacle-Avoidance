#include "MKL46Z4.h"
#include "motors.h"

void init_pwmIO(void) {
	//clock gating for B and C
    SIM->SCGC5 |= (1 << 10) | (1 << 11);

    //PWM
	SIM->SCGC6 |= (1 << 26); // TMP2 enable for motors
	SIM->SOPT2 |= (1 << 24); // Set TPMSRC as 8 MHz

    //Setup left motor
	PORTB->PCR[0] &= ~0x700; // Clear MUX
	PORTB->PCR[0] |= 0x700 & (1 << 8); // Set MUX to GPIO
	PORTB->PCR[1] &= ~0x700; // Clear MUX
	PORTB->PCR[1] |= 0x700 & (1 << 8); // Set MUX to GPIO
	GPIOB->PDDR |= (1 << 0) | (1 << 1); // Set as output

	//Setup right motor (same as before)
	PORTC->PCR[1] &= ~0x700;
	PORTC->PCR[1] |= 0x700 & (1 << 8);
	PORTC->PCR[2] &= ~0x700;
	PORTC->PCR[2] |= 0x700 & (1 << 8);
	GPIOC->PDDR |= (1 << 1) | (1 << 2);

	//Setup Switch
	PORTC->PCR[3] &= ~0x703; // Clear MUX
	PORTC->PCR[3] |= (1 << 8) | 0x03;  // Set MUX to GPIO and  pull up
	GPIOC->PDDR &= ~(1 << 3); // Clear  as input

	// Enable Motors for PWM
	PORTB->PCR[2] &= ~(0x700);   // clear MUX
	PORTB->PCR[2] |= 0x300;      // TPM2_CH0
	PORTB->PCR[3] &= ~(0x700);   // clear MUX
	PORTB->PCR[3] |= 0x300;      // TPM2_CH1


	// Config TPM2 for Motors
	TPM2->SC = 0; // Start Config at Zero
	TPM2->MOD = 999; // Choose MOD

	//Edge aligned PWM, POS High
	TPM2->CONTROLS[0].CnSC = (1 << 5) | (1 << 3);
	TPM2->CONTROLS[1].CnSC = (1 << 5) | (1 << 3);

	TPM2->CONTROLS[0].CnV = 999; // set CnV to desired
	TPM2->CONTROLS[1].CnV = 999; // set CnV to desired

	TPM2->SC = (1 << 3) | 0x03; //clock enable TPM, prescaler to 2^(3) = 8

	//stop Motors
	GPIOB->PDOR &= ~(1<<0);
	GPIOB->PDOR &= ~(1<<1);
	GPIOC->PDOR &= ~(1<<1);
	GPIOC->PDOR &= ~(1<<2);
}

void init_servo(void) {
	//Initialize Port A  Gating
	SIM->SCGC5 |= (1 << 9);
	//Setup PWM for TMP1
	SIM->SCGC6 |= (1 << 25);
	SIM->SOPT2 |= (1 << 24); // Set TPMSRC to 8MHz

	// Enable Servo for PWM control
	PORTA->PCR[12] &= ~(0x700);   // clear MUX bits
	PORTA->PCR[12] |= 0x300;      // ALT3 TPM1

	// Configure TPM1 module for PWM
	TPM1->SC = 0x04; // prescaler of 4
	TPM1->MOD = 9999; // /to get 50 hz

	//Edge aligned PWM, POS High
	TPM1->CONTROLS[0].CnSC = (1 << 5) | (1 << 3);
	TPM1->CONTROLS[0].CnV = center; //set to middle

	TPM1->SC |= (1 << 3); //enable tpm
}


//four functions to simplify movement
void motors_forward(void) {
    GPIOB->PDOR &= ~(1 << 0);
    GPIOB->PDOR |=  (1 << 1);
    GPIOC->PDOR &= ~(1 << 1);
    GPIOC->PDOR |=  (1 << 2);
}
void motors_stop(void) {
	GPIOB->PDOR &= ~(1 << 0);
	GPIOB->PDOR &= ~(1 << 1);
	GPIOC->PDOR &= ~(1 << 1);
	GPIOC->PDOR &= ~(1 << 2);
}
void motors_right(void) {
    GPIOB->PDOR |=  (1 << 0);
    GPIOB->PDOR &= ~(1 << 1);
    GPIOC->PDOR &= ~(1 << 1);
    GPIOC->PDOR |=  (1 << 2);
}
void motors_left(void) {
    GPIOB->PDOR &= ~(1 << 0);
    GPIOB->PDOR |=  (1 << 1);
    GPIOC->PDOR |=  (1 << 1);
    GPIOC->PDOR &= ~(1 << 2);
}
