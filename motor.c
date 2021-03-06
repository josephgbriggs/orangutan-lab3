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
/******************************************************************************/
/* portions of used by permission:                                            */
/* http://www.nubotics.com/support/ww01/code/atmel/ww01_avrgcc_square.c.php   */
/*                                                                            */
/* WW-01 GNU C for Atmel ATMEGA16                                             */
/* Simple Odometry Example Program, v1.0 8/3/2004                             */
/*                                                                            */
/* Copyright 2004, Noetic Design, Inc.                                        */
/*                                                                            */
/* This demonstation program shows off the features                           */
/* of the WheelWatcher.                                                       */
/******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <pololu/digital.h>
#include <pololu/OrangutanTime.h>
#include "motor.h"
#include "serial_interface.h"// TODO this include is temporary for debugging
#include "controller.h" // TODO this include is temporary for debugging

#define MICROS_PER_SEC	1000000UL

// encoder
#define ENC_A_PIN		IO_A0
#define ENC_B_PIN		IO_A1
#define ENC_POS_PER_ROT	128
static volatile unsigned long us_at_last_event;
static volatile unsigned long us_delta;
static volatile uint8_t enc_a_val_last;
static volatile uint8_t enc_b_val_last;
volatile uint16_t 		G_enc_count;
volatile uint16_t		G_enc_count_last; // shouldn't have to keep track of last value
											// but I am getting hardware errors and can't count on 
											// one position change for every successful ISR firing 
											// due to encoder and timer hardware issues
volatile uint8_t 		G_enc_direction; // 0=forward 1=reverse
volatile uint8_t 		G_enc_position; // current position of the wheel
volatile uint8_t 		G_enc_position_last; 

volatile int16_t		G_enc_ang_vel;  // in units of positions/sec
volatile uint8_t		G_enc_error;

// motor
#define PWM_PORTPIN		IO_D6	// port pin for the PWM signal
#define	DIR_PORTPIN		IO_C6	// port pin for the direction signal
#define MAX_SPEED		214		// in positions/sec 
								// my motor can make approx. 10 rotations in 6 seconds
								// this is 1280 positions / 6 ~= 214 pps
volatile uint8_t 		G_mot_sig_torque;	// the amount currently applied
volatile uint8_t 		G_mot_sig_dir;		// the direction signal currently sent


// task flags
volatile uint8_t G_encoder_event; // can be used to release tasks


void init_motor() {
	// Set the mode
	//  Configure for non-inverted fast PWM output  
    //  Normal port operation, OC2x disconnected (changed later when a non-zero torque is set)
    //  Timer2 counts up from 0 to 255 and then overflows.
    TCCR2A = 0x03;
  
    // Set the clock
	//  prescale the system clock/8 (=2.5 MHz) as the timer clock,
	//  which will produce a PWM frequency of ~9.765 kHz
	//  ƒ = 20MHz / (8 * 256)
    TCCR2B = 0x02;
	
	// set up an interrupt
	//  fires at the top of the generated wave to keep track of time 
	// see ISR(TIMER2_OVF_vect )
	TIMSK2 |= (1 << TOIE2);

    // Initialize the PWM to lowest duty cycle possible (almost braking).
    OCR2B = 0;
	
	set_digital_output(DIR_PORTPIN, G_mot_sig_dir);

	// Set the PWM pins to be low outputs.  They have to be low, otherwise
	// torque-zero (when OC2x is disconnected) would not work.
	set_digital_output(PWM_PORTPIN, 0);
}

/*
 * Pin Change interrupts for wheel encoders
 * We have our connections such that the encoders are connected to 
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
	
	// turn on pin change interrupts for PCINT7..0
	PCICR |= (1 << PCIE0);
	
	// mask this interrupt to only fire on pins PA0 and PA1
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);	
}

/*
 * Set the torque for the motor. This is a positive or negative value
 * that is capped in the range [-MAX_TORQUE..MAX_TORQUE]
 */
void set_torque(int16_t torque) {
	
	init_motor();

	if (torque < 0) {
		torque = -torque;	// make torque a positive quantity
		G_mot_sig_dir = 1;	// preserve the direction
	}
	
	if (torque > MAX_TORQUE) {
		torque = MAX_TORQUE;
	}
	
	OCR2B = G_mot_sig_torque = torque;

	if (G_mot_sig_torque == 0) {
		// Achieve a 0% duty cycle on the PWM pin by driving it low,
		// disconnecting it from Timer2
		TCCR2A &= ~(1<<COM2B1); // turn off
	}
	else {
		TCCR2A |= 1<<COM2B1; // turn on timer again

		if (G_mot_sig_dir)
		{
			set_digital_output(DIR_PORTPIN, HIGH);
		}
		else
		{
			set_digital_output(DIR_PORTPIN, LOW);
		}
	}
}

int16_t get_torque() {
	cli();
	int16_t t = (G_mot_sig_dir == 0) ? G_mot_sig_torque : -G_mot_sig_torque;
	sei();
	return t;
}

uint8_t get_position() {
	cli();
	uint8_t pos = G_enc_position;
	sei();
	return pos;
}

/* 
 * Fire this ISR on every signal we receive from the encoder
 */
ISR(PCINT0_vect) {
	
	unsigned long us; // current elapsed microseconds
	
	// Used to print to serial comm window
	char tempBuffer[32];
	int length = 0;
	
	// get the current encoder readings
	unsigned char enc_a_val = is_digital_input_high(ENC_A_PIN);
	unsigned char enc_b_val = is_digital_input_high(ENC_B_PIN);

	// use the pattern to determine the axle spin direction
	char plus = enc_a_val ^ enc_b_val_last;
	char minus = enc_b_val ^ enc_a_val_last;

	// my encoder is giving unreliable readings, so this error triggers often
	// not quite what to do except replace my hardware
	if (enc_a_val != enc_a_val_last && enc_b_val != enc_b_val_last) {
		G_enc_error = 1;
	} else {
		// clear the error if we have recovered this time
		G_enc_error = 0;
	}
	
	// if there is an encoder error, use the motor signal direction instead
	// of the direction from the encoder
	if (G_enc_error && (G_mot_sig_dir != 0)) {
		plus = (char) -G_mot_sig_dir;
		minus = (char) G_mot_sig_dir;
	}
	if(plus) { 
		G_enc_count++;
		G_enc_position++;
		G_enc_direction = 0;
	}
	else if(minus) {	
		G_enc_count--;
		G_enc_position--;
		G_enc_direction = 1;
	}
	else {
		// something messed up and don't increment anything
		// TODO log something here or set an error flag
	}

	// roll over to position zero when the motor axel spins all the way around
	G_enc_position %= ENC_POS_PER_ROT;
	
	/* NOTE: I was going to use the code below that depends on ISR(TIMER2_OVF_vect), 
	 * but can't due to some references to the Pololu libraries which pull in code
	 * that defines that interrupt handler and causes a linker error when I define
	 * it. So, instead I am relying in the fact that those libraries set up timer 2
	 * and use the get_ticks() and ticks_to_microseconds() methods for my 
	 * calculations.
	 *
	// calculate the elapsed ticks since the last encoder event
	if (ticks_last_event < timer_ticks) {
		tick_delta = timer_ticks - ticks_last_event;
	}
	else {
		tick_delta = 0xFF - ticks_last_event + timer_ticks;
	}
	 * 
	 */
	int16_t p_delta = 0;
	us = ticks_to_microseconds(get_ticks()); // this uses timer 2 overlow set in pololu libraries

	// if there are encoder errors, don't do calculations, because microseconds 
	// also seem to be incorrect when encoder errors are present
	if ( ! G_enc_error) {
		// this calculates a running average to filter alignment noise
		// previous time delta weighted three times more than current time delta
		us_delta = (us_delta * 3 + (us - us_at_last_event)) >> 2;
	
	   /*
	    * Angular velocity ω is in positions per second (positive or negative)
	    *      positions   1000000µs   p position   
	    *  ω = --------- = --------- * ----------  = (1,000,000 * p) / m
	    *       second     1 second       m µs      
	    */
		p_delta = G_enc_count - G_enc_count_last;
		G_enc_ang_vel = MICROS_PER_SEC * p_delta / us_delta;
	}

	if (G_logging_flag) {
		
		if (G_enc_error) { 	
			print_usb("----- motor.c -->ERROR\r\n", 24); 
		}
		else { 				
			print_usb("----- motor.c -----\r\n", 21); 
		}
		length = sprintf(tempBuffer, "µs: %lu  µs-last: %lu  µs∆: %lu\r\n", us, us_at_last_event, us_delta);
		print_usb(tempBuffer, length);
		if (G_enc_error) { 	
			length = sprintf(tempBuffer, "cnt: %d  pos∆: N/C\r\n", G_enc_count); 
		}
		else {
			length = sprintf(tempBuffer, "cnt: %d  pos∆: %d\r\n", G_enc_count, p_delta); 	
		}
		print_usb(tempBuffer, length);
		length = sprintf(tempBuffer, "enA: %d  enB: %d  plus: %d  minus: %d\r\n", enc_a_val, enc_b_val, plus, minus);
		print_usb(tempBuffer, length);
		length = sprintf(tempBuffer, "edir: %d  pos: %d  err: %d\r\n", G_enc_direction, G_enc_position, G_enc_error);
		print_usb(tempBuffer, length);
		if (G_enc_error) { 	
			length = sprintf(tempBuffer, "mdir: %d  tor: %d  vel: N/C\r\n", G_mot_sig_dir, G_mot_sig_torque);
		}
		else {
			length = sprintf(tempBuffer, "mdir: %d  tor: %d  vel: %d\r\n", G_mot_sig_dir, G_mot_sig_torque, G_enc_ang_vel);
		}
		print_usb(tempBuffer, length);
	}
	
	// preserve the current values for next time
	enc_a_val_last = enc_a_val;
	enc_b_val_last = enc_b_val;
	us_at_last_event = us;
	G_enc_count_last = G_enc_count;
	
	// set the event flag for controller processing
	G_encoder_event = 1;
}

// This conflicts with the pololu libraries interrupt in OrangutanTime
// /*
//  * This interrupt keeps track of time for calculations. 
//  * It fires at the top of the fast PWM cycle every 102.4µs
//  */
// ISR(TIMER2_OVF_vect) {
// 	timer_ticks++;
// }


