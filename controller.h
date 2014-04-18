#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#include <inttypes.h>
// extern


typedef struct {
	uint16_t last_position;
	
	uint16_t Kp; // Proportial gain
	uint16_t Kd; // Derivative gain
	
} control_state_s;

/*
 * Use Proportial and Derivative closed-loop control to acheive the desired setting
 */
int16_t calculate_pd(control_state_s *state, int16_t target_p, int16_t current_p);

/*
 * Use Proportial and Derivative closed-loop control to acheive the desired setting
 */
int16_t calculate_pd_w_rate(control_state_s *state, int16_t target_p, int16_t current_p, int16_t current_rate);


#endif //__CONTROLLER_H
