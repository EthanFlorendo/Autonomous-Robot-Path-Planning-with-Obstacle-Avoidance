#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"

#include "delay.h"
#include "ultrasonic.h"
#include "motors.h"

//compare to measured distance
#define close 1000
#define far 2000

int main(void) {

	 BOARD_InitBootPins();
	 BOARD_InitBootClocks();
	 BOARD_InitDebugConsole();

	 //initialize everything
	init_pwmIO();
    init_servo();
    init_US_Sensor();
    init_timer();
    init_pit();


    //variable to record state
	int state = 0;

	//variables to record different distances
	int distF, distL, distR;


		//main loop
    	while (1) {

    	    switch (state) {

    	    case 0: //wait for button press
    	    	PRINTF("State0\r\n");
    	    	TPM1->CONTROLS[0].CnV = center;
    	        if (!(GPIOC->PDIR & (1 << 3))) {
    	            delay_ms(300);  // wait 2 seconds
    	            TPM1->CONTROLS[0].CnV = center;
    	            motors_forward();
    	            delay_ms(300);
    	            state = 1;

    	        }
    	        break;

    	    case 1: //go forward
    	    	PRINTF("State1\r\n");
    	    	motors_forward();
    	    	TPM1->CONTROLS[0].CnV = center;
    	    	delay_ms(10);
    	        //motors_forward();
    	    	distF = distance;

    	        if (distF < 700){
    	        	motors_stop();
    	        	delay_ms(500);
    	        	state = 2;

    	        }
    	        break;
    	    case 2: //stop and scan

    	    	PRINTF("State2\r\n");
    	    	motors_stop();
    	    	delay_ms(500);


    	    	//right scan
    	    	TPM1->CONTROLS[0].CnV = right;
    	    	delay_ms(300);
    	    	distR = distance;
    	    	delay_ms(300);

    	    	//left scan
    	    	TPM1->CONTROLS[0].CnV = left;
				delay_ms(300);
				distL = distance;
				delay_ms(300);



    	    	if (distR > close){

    	    		delay_ms(300);
    	    		state = 3;

    	    	}
    	    	else if(distL > close){
    	    		delay_ms(100);
    	    		state = 7;
    	    	}

    	    	//180, then hug wall
    	    	else if(distL > distR){
    	    		motors_left();
    	    		delay_ms(700);
    	    		state = 4;
    	    	}
    	    	else if (distR > distL){
    	    		motors_right();
    	    		delay_ms(700);
    	    		state = 8;
    	    	}




				break;

    	    case 3://turn right
    	    	PRINTF("State3\r\n");
    	    	motors_right();
    	    	delay_ms(200);
    	    	motors_stop();
    	    	delay_ms(150);

    	    	//front scan
				TPM1->CONTROLS[0].CnV = center;
				delay_ms(300);
				distF = distance;
				delay_ms(300);

				//if there is a wall somewhat close,
				if (distF < close){
					TPM1->CONTROLS[0].CnV = center;
					delay_ms(300);
					state = 1;
				}
				//no wall close, will look at left wall
				else {
					TPM1->CONTROLS[0].CnV = left;
					delay_ms(300);
					state = 4;
				}


    	    	break;

    	    case 4://hug left
    	    	PRINTF("State4\r\n");
    	    	TPM1->CONTROLS[0].CnV = left;
    	    	delay_ms(100);
    	    	motors_forward();

    	    	distL = distance;
    	    	//end of wall
    	    	if(distL > far){
    	    		motors_stop();
    	    		delay_ms(300);
    	    		state = 5;
    	    	}


    	    	break;

    	    case 5: //go around corner
    	    	PRINTF("State5\r\n");
    	    	motors_forward();
    	    	delay_ms(300);
				motors_left();
				delay_ms(200);
				motors_forward();
				delay_ms(300);
				motors_stop();
				state = 6;

    	    	break;

    	    case 6: //orient towards closest wall
    	    	//right scan
    	    	PRINTF("State6\r\n");
				TPM1->CONTROLS[0].CnV = center;
				delay_ms(300);
				distF = distance;
				delay_ms(300);

    	    	//right scan
				TPM1->CONTROLS[0].CnV = right;
				delay_ms(300);
				distR = distance;
				delay_ms(300);

				//left scan
				TPM1->CONTROLS[0].CnV = left;
				delay_ms(300);
				distL = distance;
				delay_ms(300);


				TPM1->CONTROLS[0].CnV = center;
				delay_ms(300);

				if(distF < distR && distF < distL){
					state = 1;
				}
				if(distR < distF && distR < distL){
					motors_right();
					delay_ms(200);
					state = 1;
				}
				if(distL < distR && distL < distF){
					motors_left();
					delay_ms(200);
					state = 1;
				}
    	    	break;


    	    case 7://turn left
    	    	PRINTF("State7\r\n");
				motors_left();
				delay_ms(300);
				motors_stop();
				delay_ms(300);

				//front scan
				TPM1->CONTROLS[0].CnV = center;
				delay_ms(300);
				distF = distance;
				delay_ms(300);

				//if there is a wall somewhat close,
				if (distF < close){
					state = 1;
				}
				//no wall close, will look at right wall
				else {
					TPM1->CONTROLS[0].CnV = right;
					delay_ms(300);
					state = 8;
				}


				break;

			case 8://hug right
				PRINTF("State8\r\n");
				TPM1->CONTROLS[0].CnV = right;
				delay_ms(100);
				motors_forward();

				//end of wall

				distR = distance;
				if(distR > far){
					motors_stop();
					delay_ms(300);
					state = 9;
				}





				break;

			case 9: //go around corner
				PRINTF("State9\r\n");
				motors_forward();
				delay_ms(300);
				motors_right();
				delay_ms(200);
				motors_forward();
				delay_ms(300);
				motors_stop();
				state = 6;

				break;





    	    }
    	}

    }




