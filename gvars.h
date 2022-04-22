#pragma once

#include <stdint.h>

#include "states.h"
#include "consensus.h"

extern int iter_num;
extern uint8_t my_id;
extern uint8_t node_id[];
extern volatile int node_count;

extern float gain[];
extern float dc[];

extern volatile float external_illumination;

extern volatile bool calibrate_now;
extern volatile int cal_ack_count;
extern volatile bool calibrate_all_done;

extern volatile bool cons_ack_count;

extern float cost[];
extern cons node(float rho, int max_iter);

extern states state;