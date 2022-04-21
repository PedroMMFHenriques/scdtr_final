#include "gvars.h"
#include "settings.h"

uint8_t my_id;
uint8_t node_id[N_NODES];
volatile int node_count = 0;

float gain[N_NODES];

volatile float external_illumination;

volatile bool calibrate_now = false;
volatile int cal_ack_count = 0;
volatile bool calibrate_all_done = false;

states state = discovery;
