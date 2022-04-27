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


float reference = 0.0;

float reference_backup = 0.0;

bool occupancy = true;

bool anti_windup_enabled = true;

bool ff_control_enabled = true;

bool fb_control_enabled = true;

bool stream_l_enabled = false;

bool stream_d_enabled = false;

float measured_illuminance;

float duty_cycle;

float duty_cycle_backup = 0.0;


float energy = 0.0;

float visibility_error_sum = 0.0;

float flicker_sum = 0.0;

int n_samples = 0;



float unoccupied_reference = UNOCCUPIED_REFERENCE;

float occupied_reference = OCCUPIED_REFERENCE;


float reference_lower_bound = 0.0;

float cost = COST;