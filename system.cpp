#include "system.h"
#include "settings.h"
#include <math.h>
#include "gvars.h"

int dc_to_DAC(float dc) {
    return (int) (dc * (PWM_DC_RANGE - 1));
}

float adc_to_volt(int adc) {
    return adc / (float) (PWM_DC_RANGE - 1) * ADC_REF;
}

float volt_to_lux(float volt) {
    float R = LDR_R * ADC_REF / volt - LDR_R;
    return pow(10.0, (log10(R) - ldr_b) / ldr_m);
}

float adc_to_lux (int adc){
    return volt_to_lux(adc_to_volt(adc));
}

void reset_gvars(){
    iter_num = 0;

    node_id[N_NODES] = {0};
    node_count = 0;

    gain[N_NODES] = {0};
    
    dc[0] = Eigen::Vector3f(0,0,0);
    dc[1] = Eigen::Vector3f(0,0,0);
    dc[2] = Eigen::Vector3f(0,0,0);
    
    external_illumination = 0;

    calibrate_now = false;
    cal_ack_count = 0;
    calibrate_all_done = false;

    cons_ack_count = 0;

    r_ack_count = 0;

    reference_changed = false;

    reference_lower_bound_changed = false;

    duty_cycle_consensus = 0.0;

    reference = 0.0;

    reference_backup = 0.0;

    occupancy = false;

    anti_windup_enabled = true;

    ff_control_enabled = true;

    fb_control_enabled = true;

    stream_l_enabled = false;

    stream_d_enabled = false;

    reset_enabled = false;

    measured_illuminance = 0;
    measured_prev = 0;
    measured_prev_prev = 0;

    duty_cycle = 0;
    prev_duty_cycle = 0;

    duty_cycle_backup = 0.0;

    energy = 0.0;

    visibility_error_sum = 0.0;

    flicker_sum = 0.0;

    n_samples = 0;

    unoccupied_reference = UNOCCUPIED_REFERENCE;

    occupied_reference = OCCUPIED_REFERENCE;

    reference_lower_bound = UNOCCUPIED_REFERENCE;

    cost = COST;

    t_start = 0;
    t_startSim = 0;

    ff_duty_cycle = 0;
    fb_duty_cycle = 0;
}