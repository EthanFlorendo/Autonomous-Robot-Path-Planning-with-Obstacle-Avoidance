#include "MKL46Z4.h"
#include "ultrasonic.h"

//variable to measure echo time or distance in front
static volatile unsigned int time_now = 0;
static volatile unsigned int ping = 0;
volatile unsigned int distance = 0;

//triggered to when MOD for TPM0 overflows (around a microsecond)
void TPM0_IRQHandler(void){
	TPM0->SC |= (1 << 7); //clear timer flag
	time_now++; //finds the number of microseconds passed
}


void PORTA_IRQHandler(void) {
    // Clear interrupt flag
    PORTA->PCR[13] |= 1<<24;
    //filter out noise (often there are huge spikes when there shouldnt be)
    if (time_now-ping > 100000 || time_now-ping < 10 ){
    	return;
    }
    else{
    	//check distance (time from last ping or echo travel time)
    	distance = time_now-ping;
    }

}
void PIT_IRQHandler(void) {

	//trigger around every 60 ms
    if (PIT->CHANNEL[0].TFLG) { //timer 0 triggered
        PIT->CHANNEL[0].TFLG = 1; //clear flag
        GPIOD->PDOR |= (1 << 2); //pulse trigger on
        PIT->CHANNEL[1].TCTRL = 0x3; //enable timer 1 interrupt, start timer
        ping = time_now;
    }
    //find end of 10 microsecond pulse
    if (PIT->CHANNEL[1].TFLG) { //timer 1 triggered
        PIT->CHANNEL[1].TFLG = 1; //clear flag
        GPIOD->PDOR &= ~(1 << 2); //pulse trigger off
        PIT->CHANNEL[1].TCTRL = 0; //disable timer
    }
}
void init_US_Sensor(void) {
	//init port A and D
	SIM->SCGC5 |= (1 << 9) | (1 << 12);

	//Trigger as output
    PORTD->PCR[2] &= ~0x700; // Clear MUX
	PORTD->PCR[2] |= 0x700 & (1 << 8); // Set MUX bits to GPIO
	GPIOD->PDDR |= (1 << 2);  //set as output

	//Echo as input
	PORTA->PCR[13] &= ~0x703; // Clear MUX and other bits
	PORTA->PCR[13] |= (1 << 8) | 0x03;  // Set MUX bits to GPIO, and set pull up
	GPIOA->PDDR &= ~(1 << 13); // Clear input
	PORTA->PCR[13] |= (0xA << 16); //interrupt on falling edge

	NVIC_EnableIRQ(30); //enable portA interrupt

	//initialize trigger output as low
	GPIOD->PDOR &= ~(1 << 2);

}

void init_pit(void) {
	//enable PIT Clock
    SIM->SCGC6 |= (1<<23);
    //enable PIT module
    PIT->MCR = 0x00;
    //8MHz -> around 60ms " we suggest to use over 60ms cycle"
    PIT->CHANNEL[0].LDVAL = 479999;
    //around 10 us for trigger "You only need to supply a short 10uS pulse to the trigger input to start the ranging"
    PIT->CHANNEL[1].LDVAL = 79;
    //enable pit interrupt
    NVIC_EnableIRQ(22);
    //enable interrupts and channel zero only
    PIT->CHANNEL[0].TCTRL = 0x3;
}

void init_timer(void) {
	//enable clock gating for tpm0
	SIM->SCGC6 |= (1 << 24);
	//use OSCER clock
	SIM->SOPT2 |= (1 << 24);
	//one tick is 1 us roughly
	//should be 10 us is 79, 1 is 7-1
	TPM0->MOD = 7;
	NVIC_EnableIRQ(17); //tpm 0 interrupt
	// Reset, Enable Interrupt, Prescaler = 3 and Start Timer
	TPM0->SC = (1 << 7) | (1 << 6) | (1 << 3) | (3 << 0);
}
