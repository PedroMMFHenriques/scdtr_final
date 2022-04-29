#pragma once

#include <stdint.h>
#include <ArduinoEigenDense.h>

#include "states.h"

#include "consensus.h"
#include "pi.h"
#include "simulator.h"


extern int iter_num;
extern uint8_t my_id;
extern uint8_t node_id[];
extern volatile int node_count;

extern float gain[];
extern Eigen::Vector3f dc[];

extern volatile float external_illumination;

extern volatile bool calibrate_now;
extern volatile int cal_ack_count;
extern volatile bool calibrate_all_done;


extern volatile int cons_ack_count;

extern volatile int r_ack_count;

extern cons node;

extern states state;


extern bool reference_changed;

extern bool reference_lower_bound_changed;

extern float duty_cycle_consensus;


extern float reference;

extern float reference_backup;

extern bool occupancy;

extern bool anti_windup_enabled;

extern bool ff_control_enabled;

extern bool fb_control_enabled;

extern bool stream_l_enabled;

extern bool stream_d_enabled;

extern bool reset_enabled;

extern float measured_illuminance;
extern float measured_prev;
extern float measured_prev_prev;

extern float duty_cycle;
extern float prev_duty_cycle;

extern float duty_cycle_backup;


extern float energy;

extern float visibility_error_sum;

extern float flicker_sum;

extern int n_samples;


extern float unoccupied_reference;

extern float occupied_reference;


extern float reference_lower_bound;

extern float cost;

extern unsigned long t_start;
extern unsigned long t_startSim;

extern float ff_duty_cycle;
extern float fb_duty_cycle;
extern pi cont;
extern sim simu;

extern float ldr_m;
extern float ldr_b;

extern int printL_left;
extern int printD_left;
extern int buf_ptr;
