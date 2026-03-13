#ifndef ULTRASONIC_H
#define ULTRASONIC_H

//latest echo time (us) measured by the ultrasonic sensor
extern volatile unsigned int distance;

//trigger output (PTD2) and echo input interrupt (PTA13)
void init_US_Sensor(void);
//PIT ch0 = 60 ms ping period, ch1 = 10 us trigger pulse
void init_pit(void);
//TPM0 overflow roughly every microsecond for time_now
void init_timer(void);

#endif
