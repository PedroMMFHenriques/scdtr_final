#pragma once

#include <stdint.h>

#include "states.h"

extern uint8_t my_id;
extern uint8_t node_id[];
extern volatile int node_count;

extern float gain[];

extern volatile float external_illumination;

extern volatile bool calibrate_now;
extern volatile int cal_ack_count;
extern volatile bool calibrate_all_done;

extern states state;


extern float reference;

extern float reference_backup;

extern bool occupancy;

extern bool anti_windup_enabled;

extern bool ff_control_enabled;

extern bool fb_control_enabled;

extern bool stream_l_enabled;

extern bool stream_d_enabled;

extern float measured_illuminance;

extern float duty_cycle;

extern float duty_cycle_backup;



extern float energy;

extern float visibility_error_sum;

extern float flicker_sum;

extern int n_samples;


extern float unoccupied_reference;

extern float occupied_reference;


extern float reference_lower_bound;

extern float cost;