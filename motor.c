/*
 * Portions based on: OrangutanMotors.cpp 
 * Written by Ben Schmidel, May 15, 2008.
 * Last modified: September 29, 2008
 * Copyright (c) 2008-2012 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */
/*
 * Portions based on: PololuWheelEncoders.cpp
 * Copyright (c) 2009-2012 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */
#include <pololu/orangutan.h>
#include "motor.h"

// encoder
#define ENC_A_PIN			IO_A0
#define ENC_B_PIN			IO_A1
#define NUM_WHEEL_POSITIONS 128

volatile uint16_t G_enc_count;
volatile uint8_t G_wheel_position;
volatile uint8_t G_last_wheel_position;

// motor
#define PWM_PORTPIN	IO_D6
#define	DIR_PORTPIN	IO_C6
#define MAX_TORQUE	0xFF

volatile uint8_t G_motor_torque;
volatile uint8_t G_motor_dir;

// extern
extern volatile uint8_t G_logging_flag;
extern volatile uint8_t G_release_logging;


void init_motor() {
	// Configure for non-inverted fast PWM output on motor PWM pins:   
    //  Normal port operation, OC2x disconnected (changes later when a non-zero speed is set)
    //  Timer2 counts up from 0 to 255 and then overflows directly to 0.
    TCCR2A = 0x03;
  
    // use the system clock/8 (=2.5 MHz) as the timer clock,
	// which will produce a PWM frequency of 10 kHz
    TCCR2B = 0x02;

	// use the system clock (=20 MHz) as the timer clock,
	// which will produce a PWM frequency of 78 kHz.  The Baby Orangutan B,
	// Orangutan SVP, and 3Pi can support PWM frequencies this high.  The
	// Orangutan LV-168 cannot support frequencies above 10 kHz.
    //TCCR2B = 0x01;

    // Initialize the PWM to lowest duty cycle possible (almost braking).
    OCR2B = 0;
	
	set_digital_output(DIR_PORTPIN, 0);

	// Set the PWM pins to be low outputs.  They have to be low, otherwise
	// speed-zero (when OC2x is disconnected) would not work.
	set_digital_output(PWM_PORTPIN, 0);
}

/*
 * Pin Change interrupts for wheel encoders
 * We have our connection such that the encoders are connected to 
 * Pin A0 and Pin A1
 *
 * PA0 --> PCINT0
 * PA1 --> PCINT1
 *
 * The handler for PCI0 triggers on PCINT7..0, so it will run for either encoder
 *
 * PCMSK0 register controls which pins contribute.
 * 
 */
void init_encoder() {
	
	// set pin change interrupts for PCINT7..0
	PCICR |= (1 << PCIE0);
	
	// mask this to only pins PA0 and PA1
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);
	
}

void set_speed(int8_t speed) {
	
	init_motor();

	if (speed < 0) {
		speed = -speed;	// make speed a positive quantity
		G_motor_dir = 1;	// preserve the direction
	}
	
	if (speed > MAX_TORQUE) {
		speed = MAX_TORQUE;
	}
	
	OCR2B = G_motor_torque = speed;

	if (G_motor_torque == 0) {
		// Achieve a 0% duty cycle on the PWM pin by driving it low,
		// disconnecting it from Timer2
		TCCR2A &= ~(1<<COM2B1); // turn off
	}
	else {
		// Achieve a variable duty cycle on the PWM pin using Timer2.
		TCCR2A |= 1<<COM2B1; // turn on

		if (G_motor_dir)
		{
			set_digital_output(DIR_PORTPIN, HIGH);
		}
		else
		{
			set_digital_output(DIR_PORTPIN, LOW);
		}
	}
}

int8_t get_speed() {
	return G_motor_torque * G_motor_dir;
}

uint8_t get_position() {
	return G_wheel_position;
}

ISR(PCINT0_vect) {

	// update the encoder count
	G_enc_count++;
	// update the wheel position
	(G_motor_dir == 0) ? G_wheel_position++ : G_wheel_position--;
	
	// roll over back to zero if needed
	G_wheel_position %= NUM_WHEEL_POSITIONS;
	
	if (G_logging_flag) {
		G_release_logging = 1;
	}
}
