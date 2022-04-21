#line 1 "/home/nunu/Arduino/scdtr_final_pedro/gvars.h"
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