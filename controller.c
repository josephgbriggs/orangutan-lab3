#include "controller.h"
#include <pololu/orangutan.h>

/*
 * Use Proportial and Derivative closed-loop control to acheive the desired position
 * 
 */
int16_t calculate_pd(control_state_s *state, int16_t target_p, int16_t current_p) {
	/*
     * T = Kp(E) + Kd(dE/dt)
     *   = Kp(Sr-Sm) + Kd( d(Sr-Sm)/dt )
     *   where
     *		T = Output signal (torque)
     *		E = Error
     *		Sr = Reference position (target position)
     *		Sm = Current position (computed based on finite differences)
     *		Kp = Proportional gain
     *		Kd = Derivative gain
     * 
     * To determine the derivative:
	 *	E[t=1] = Sr - Sm[1]
	 *  E[t=0] = Sr - Sm[0]
	 *	==>
	 *  dE/dt = 
	 * 		(Sr - Sm[1]) - (Sr - Sm[0]) / (t1 - t0)
	 *		= Sr - Sr - Sm[1] + Sm[0] / (t1 - t0)
	 *		= - Sm(1) + Sm(0) / (t1 - t0)
	 *		= ΔSm / Δt
     */
	int16_t delta_p = target_p - current_p;
	int16_t proportional = state->Kp * delta_p;
	int16_t derivate = state->Kd * delta_p;
	
	return proportional + derivate;
}

/*
 * Use Proportial and Derivative closed-loop control to acheive the desired setting
 * 
 * T = Kp(Pr - Pm) - Kd*Vm 
 *   where
 *		T = Output signal
 *		Pr = Desired position (target)
 *		Pm = Current position
 *		Vm = Current velocity (change in position per time computed based on finite differences)
 *		Kp = Proportional gain
 *		Kd = Derivative gain
 */
int16_t calculate_pd_w_rate(control_state_s *state, int16_t target_p, int16_t current_p, int16_t current_rate) {
	
	int16_t delta_p = target_p - current_p;
	int16_t proportional = state->Kp * delta_p;
	int16_t derivate = state->Kd * current_rate;
	
	return proportional + derivate;
}

/* 
 * Clock using 16-bit timer (Timer/Counter 3)
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
void init_clock(uint16_t hz) {
	
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
			break;
		case 50:
			top_cnt = 1563;
			break;
		case 100:
			top_cnt = 781;
			break;
		case 1000:
		default:
			top_cnt = 78;
			break;
	}
	
	OCR3A = top_cnt;
	
	// turn on the interrupt in the mask
	TIMSK3 |= 1 << OCIE3A;
}

ISR(TIMER3_COMPA_vect) {
	
}