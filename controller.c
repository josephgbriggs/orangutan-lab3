#include <pololu/orangutan.h>
#include "controller.h"
#include "motor.h"
#include "serial_interface.h" //TODO remove - temporary for debugging
//#include <pololu/orangutan.h>

#define MS_IN_SEC 1000

// Note: static variables are local to this file
// Time
static volatile uint16_t time_step_ms;	// elapsed time between controller ISRs
static volatile uint32_t time_delta_ms;	// an intra-event timer
// Target values
static volatile uint8_t ref_target = 0;
// interim values
static volatile uint8_t mz_position = 0;
static volatile uint8_t last_mz_position = 0;
static volatile int16_t mz_velocity = 0;
static volatile int16_t last_mz_velocity = 0;

// Global Gains
volatile uint16_t G_Kp = 1; // Proportial gain
volatile uint16_t G_Kd = 1; // Derivative gain

/*
 * Fill the buffer with current variables
 * Return the length of what was filled
 */
int buffer_controller_values(char* buffer) {
	int l = sprintf( buffer, \
		"Kp:%u Kd:%u Vm:%d Pr:%d Pm:%d T:%d\r\n", \
		G_Kp, G_Kd, mz_velocity, ref_target, mz_position, G_mot_sig_torque);
	return l;
}

/*
 * Use Proportial and Derivative closed-loop control to acheive the desired position
 * 
 */
int16_t calculate_pd(uint16_t Kp, uint16_t Kd, int16_t ref, uint16_t mz, uint16_t last_mz, uint32_t time_delta) {
	/*
     * T = Kp(E) + Kd(dE/dt)
     *   = Kp(Sr-Sm) + Kd( d(Sr-Sm)/dt )
     *   where
     *		T = Output signal (torque)
     *		E = Error
     *		Pr = Reference position (target position)
     *		Pm = Current position (computed based on finite differences)
     *		Kp = Proportional gain
     *		Kd = Derivative gain
     * 
     * To determine the derivative:
	 *	E[t=1] = Pr - Pm[1]
	 *  E[t=0] = Pr - Pm[0]
	 *	==>
	 *  dE/dt = 
	 * 		(Pr - Pm[1]) - (Pr - Pm[0]) / (t1 - t0)
	 *		= Pr - Pr - Pm[1] + Pm[0] / (t1 - t0)
	 *		= - Pm(1) + Pm(0) / (t1 - t0)
	 *		= ΔPm / Δt
     */
	int16_t proportional = Kp * (ref - mz);
	int16_t derivate = (Kd * (last_mz - mz) * 1000) / time_delta;
	
	return proportional + derivate;
}


void set_speed(int16_t speed) {
	ref_target = speed;
	while (mz_velocity != speed) {
		set_torque(
			calculate_pd(G_Kp, G_Kd, speed, mz_velocity, last_mz_velocity, time_delta_ms));
	}
}

void set_position(uint8_t position) {
	ref_target = position;
	while (get_position() != position) {
		set_torque(
			calculate_pd(G_Kp, G_Kd, position, mz_position, last_mz_position, time_delta_ms));
	}
}

/* 
 * Sampling rate clock using 16-bit timer (Timer/Counter 3)
 *
 * (20MHz) * (1/PRESCALE) * (1/TOP) = (1 ISR / N ms) * 1000 = F Hz
 * 
 *  PRESCALER	   Hz	   
 *  -----------	--------- 
 *  clkT3S/256	78,125 Hz
 *  
 *  F(Hz) N(ms) TOP
 *  ----- ---- ----
 *  1000    1    78
 *   100   10   781
 *    50   20  1563
 *    10  100  7813
 */
void init_controller_rate(uint16_t hz) {
	
	uint16_t top_cnt;
	
	// set the clock tick prescaler bits to use clkio/256
	TCCR3B |= (1 << CS32);
	
	// Compare Output pins disconnected (OC3A/B not used)
	// Leave default settings in TCCR3A
	
	// Set waveform generation to CTC 'mode 4', OCR3A to define the TOP
	TCCR3B |= (1 << WGM32);

	// set the top in OCR3A
	switch (hz) {
		case 10:
			top_cnt = 7831;
			time_step_ms = 100;
			break;
		case 50:
			top_cnt = 1563;
			time_step_ms = 20;
			break;
		case 100:
			top_cnt = 781;
			time_step_ms = 10;
			break;
		case 1000: 	// this is also the default value
		default:
			top_cnt = 78;
			time_step_ms = 1;
			break;
	}
	
	OCR3A = top_cnt;
	
	// turn on the interrupt in the mask
	TIMSK3 |= 1 << OCIE3A;
}

ISR(TIMER3_COMPA_vect) {
	
	// Used to print to serial comm window
	char tempBuffer[32];
	int length = 0;
	
	// update the elapsed time
	time_delta_ms += time_step_ms;
	
	// only run if there is a motor encoder event to process
	if (G_encoder_event) {
		
		// preserve the last values before storing new ones
		last_mz_velocity = mz_velocity;
		last_mz_position = mz_position;
		
		// update values
		mz_position = G_enc_position;
		mz_velocity =  (MS_IN_SEC * (last_mz_position - mz_position)) / time_delta_ms;
		
		length = sprintf(tempBuffer, "last_mz_position: %u  ", last_mz_position);
		print_usb(tempBuffer, length);
		length = sprintf(tempBuffer, "mz_position: %u  ", mz_position);
		print_usb(tempBuffer, length);
		length = sprintf(tempBuffer, "time_delta_ms: %u  ", time_delta_ms);
		print_usb(tempBuffer, length);
		length = sprintf(tempBuffer, "mz_velocity: %d\r\n", mz_velocity);
		print_usb(tempBuffer, length);
		
		time_delta_ms = 0; // reset the intra-event timer
		G_encoder_event = 0; // reset the flag
	}
	
}
