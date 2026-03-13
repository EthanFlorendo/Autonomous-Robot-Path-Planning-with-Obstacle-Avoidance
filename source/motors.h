#ifndef MOTORS_H
#define MOTORS_H

//change values for servo
#define left 7200
#define right 2600
#define center 5000

//drive motors (TPM2 PWM + direction GPIO) and start switch (PTC3)
void init_pwmIO(void);
//servo on PTA12 (TPM1 CH0, 50 Hz)
void init_servo(void);

//four functions to simplify movement
void motors_forward(void);
void motors_stop(void);
void motors_right(void);
void motors_left(void);

#endif
