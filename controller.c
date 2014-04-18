#include "controller.h"

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