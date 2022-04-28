#include "gvars.h"
#include "settings.h"

int iter_num = 0;

uint8_t my_id;
uint8_t node_id[N_NODES];
volatile int node_count = 0;

float gain[N_NODES];
Eigen::Vector3f dc[N_NODES];

volatile float external_illumination;

volatile bool calibrate_now = false;
volatile int cal_ack_count = 0;
volatile bool calibrate_all_done = false;

volatile int cons_ack_count = 0;

cons node(RHO, MAX_ITER);

states state = discovery;

bool reference_changed = false;

bool reference_lower_bound_changed = false;

float duty_cycle_consensus = 0.0;

float reference = 0.0;

float reference_backup = 0.0;

bool occupancy = false;

bool anti_windup_enabled = true;

bool ff_control_enabled = true;

bool fb_control_enabled = true;

bool stream_l_enabled = false;

bool stream_d_enabled = false;

float measured_illuminance = 0;
float measured_prev = 0;
float measured_prev_prev = 0;

float duty_cycle = 0;
float prev_duty_cycle = 0;

float duty_cycle_backup = 0.0;


float energy = 0.0;

float visibility_error_sum = 0.0;

float flicker_sum = 0.0;

int n_samples = 0;


float unoccupied_reference = UNOCCUPIED_REFERENCE;

float occupied_reference = OCCUPIED_REFERENCE;


float reference_lower_bound = UNOCCUPIED_REFERENCE;

float cost = COST;

unsigned long t_start = 0;
unsigned long t_startSim = 0;
bool startup = true;

float ff_duty_cycle = 0;
float fb_duty_cycle = 0;

pi cont(K_P, K_I, TIMESTEP); //E SE DEMORAR MAIS QUE 1 TIMESTEP??????
sim simu{ADC_REF, LDR_R};

float ldr_b;
float ldr_m;