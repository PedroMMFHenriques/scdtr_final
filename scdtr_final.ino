#include <Arduino.h>
#include <hardware/flash.h>

#include "settings.h"
#include "comms.h"
#include "calibrations.h"
#include "gvars.h"
#include "utils.h"
#include "msg_queue.h"
#include "cli.h"
#include "consensus.h"
#include "simulator.h"
#include "pi.h"
#include "system.h"
#include "protocol.h"

struct repeating_timer timer;
volatile uint32_t timer_time {0};
volatile bool timer_fired {false};


bool timer_ISR(struct repeating_timer *t) {
    if(!timer_fired) {
        timer_time = micros();
        timer_fired = true;
    }
    return true;
}

void setup() {
    Serial.begin(SERIAL_BAUD);

    analogReadResolution(ADC_RES);
    analogWriteFreq(PWM_FREQ);
    analogWriteRange(PWM_DC_RANGE);

    uint8_t my_full_id[8];
    flash_get_unique_id(my_full_id);
    my_id = my_full_id[6] & 0b01111111; // set MSB to 0

    initI2C(my_id);


    if (my_id == 0x2D) {
        ldr_b = LDR_B_2D;
        ldr_m = LDR_M_2D;

    } else if (my_id == 0x3D) {
        ldr_b = LDR_B_3D;
        ldr_m = LDR_M_3D;
    }
    else {
        ldr_b = LDR_B_61;
        ldr_m = LDR_M_61;
    }

    simu.sim_init(ldr_m, ldr_b);

    delay(3000); //make sure all devices are awake at this point
}

void loop() {
    float Yadc_sum;
    float adc_measurement;
    float flicker;
    uint32_t consensus_iter_start_time;
    float fb_Rlux;
    
    //cooler message parser
    etl::vector<uint8_t, MSG_SIZE> msg;
    while(!msg_queue.empty()) {
        msg_queue.pop(msg);
        msg_handler_parser(msg.data(), msg.size());
    }

    switch (state) {
    case discovery:
        
        Serial.printf("Discovering...\n");
        node_discovery();
        if (node_count == N_NODES) {
            node_discovery();
            sort3(node_id);

            state = waiting_callibration;
            calibrate_external_illum();
            Serial.printf("Waiting callibration...\n");
            delay(1);
        }
        break;

    case waiting_callibration:
        if (node_id[0] == my_id) {
            calibrate_now = true;
        }
        if (calibrate_now) {
            cal_ack_count = 0;
            state = callibration_low;
            calibrate_gains_low();
        }
        break;
        
    case callibration_low:
        Serial.printf("Waiting low point acks...\n");
        delay(1);
        if (cal_ack_count == N_NODES) {
            cal_ack_count = 0;
            state = callibration_high;
            calibrate_gains_high();
        }
        break;

    case callibration_high:
        Serial.printf("Waiting high point acks...\n");
        delay(1);
        if (cal_ack_count == N_NODES) {
            state = callibration_finish;
            calibrate_gains_finish();
            Serial.printf("Waiting all done...\n");
            delay(1);
        }
        break;

    case callibration_finish:
        if (calibrate_all_done) {
            if (my_id == node_id[0]) {
                reference_lower_bound_changed = true;
            }
    
            int idx = index_of(my_id, node_id, N_NODES);
            Eigen::Vector3f K(gain[0]/100.0, gain[1]/100.0, gain[2]/100.0);
            node.init_cons(idx, external_illumination, cost, K);

            state = normal;
            Serial.printf("Initialization done!\n");
            Serial.printf("My ID: %X\n", my_id);
            Serial.printf("IDs: %X %X %X \n", node_id[0], node_id[1], node_id[2]);
            Serial.printf("External Ilumination: %f\n", external_illumination);
            Serial.printf("Gains: %f %f %f\n\n\n", gain[0], gain[1], gain[2]);

            t_start = micros(); //elapsed timer
            add_repeating_timer_ms(- TIMESTEP, timer_ISR, NULL, &timer);
        }
        break;


    case normal:
        cli();


        if(reference_lower_bound_changed){
            reference_lower_bound_changed = false;
            start_consensus();
        }

        if(timer_fired){
            timer_fired = false;
            state = sampling;
        }

        if(reset_enabled){
            if(r_ack_count == N_NODES){
                while(!msg_queue.empty()) {
                    msg_queue.pop(msg);
                }
                cancel_repeating_timer(&timer);
                reset_gvars();
                state = discovery;
            } 
        }

        break;

    case consensus_calc:
        Serial.printf("Calculating consensus...\n");
        if(iter_num == MAX_ITER){
            Serial.printf("Consensus DONE...\n");
            
            //guardar resultados
            node.get_sol_results();

            reference_changed = true;

            Serial.printf("reference: %f\n", reference);
            Serial.printf("duty_cycle_consensus: %f\n", duty_cycle_consensus);

            state = normal;
            break;
        }

        cons_ack_count = 0;
        node.iter_cons();
        iter_num++;
        
        state = consensus_wait;
        consensus_iter_start_time =  millis();  
        Serial.printf("Waiting consensus... (iter %d)\n", iter_num);   
        break;

    case consensus_wait:
        if (cons_ack_count == N_NODES || millis() - consensus_iter_start_time > CONSENSUS_TIMEOUT) {
            Eigen::Vector3f d_av = (dc[0] + dc[1] + dc[2])/3;

            node.update_cons(d_av);

            state = consensus_calc;
        }
        break;
    
    case sampling:
        //measure illuminance
        Yadc_sum = 0;
        //mean digital filter
        for(int i = 0; i < SAMPLES_PER_MEASUREMENT; i++){ 
            Yadc_sum += analogRead(LDR_PIN);
        }

        adc_measurement = Yadc_sum/SAMPLES_PER_MEASUREMENT;
        measured_illuminance = adc_to_lux(adc_measurement);
        n_samples++;

        //turn on/off feedforward control
        if(ff_control_enabled) ff_duty_cycle = duty_cycle_consensus; 
        else ff_duty_cycle = 0;

        //turn on/off feedback control
        if(fb_control_enabled){ 
            if(reference_changed){  
                reference_changed = false;

                //calculate tau and readies the class
                simu.dim_change(gain[index_of(my_id, node_id, N_NODES)], adc_to_volt(adc_measurement), reference, 0);

                t_startSim = micros();
            }

            //having the tau, just needs to calculate voltage
            fb_Rlux = volt_to_lux(simu.get_vt((micros() - t_startSim)/pow(10,6))); //new lux reference for the FB based on the simulator
            fb_duty_cycle = cont.calc_pwm(fb_Rlux, measured_illuminance, anti_windup_enabled, ff_duty_cycle);
        }
        else fb_duty_cycle = 0;

        prev_duty_cycle = duty_cycle;

        //sum the components of feedforward and feedback
        duty_cycle = ff_duty_cycle + fb_duty_cycle;
        if(!ff_control_enabled && !fb_control_enabled) duty_cycle = duty_cycle_backup;
        
        if(duty_cycle > 1) duty_cycle = 1;
        else if(duty_cycle < 0) duty_cycle = 0;
        analogWrite(LED_PIN, dc_to_DAC(duty_cycle));


        if(stream_l_enabled){
            i2c_cmd_float_send(BROADCAST, STREAM_L, measured_illuminance);
        }
        if(stream_d_enabled){
            i2c_cmd_float_send(BROADCAST, STREAM_D, duty_cycle);
        }   
        
        /*
            //fill circular buffer
            if(cbuf.is_full()) cbuf.take();
            cbuf.put(my_data{prev_dc, Ylux});
        */

        //performance metrics
        energy += LED_POWER*prev_duty_cycle*TIMESTEP; 
        visibility_error_sum += max(0, reference - measured_illuminance); 
        
        flicker = 0;
        if(n_samples == 1) measured_prev = measured_illuminance;
        if(n_samples == 2){
            measured_prev_prev = measured_prev;
            measured_prev = measured_illuminance;
        }
        if(n_samples >= 3){
            if((measured_illuminance - measured_prev)*(measured_prev - measured_prev_prev) < 0){
                flicker = (abs(measured_illuminance - measured_prev) + abs(measured_prev - measured_prev_prev))/(2*TIMESTEP);
            }
            measured_prev_prev = measured_prev;
            measured_prev = measured_illuminance;      
        }
        flicker_sum += flicker;

        state = normal;
        break;

    default:
        break;
    }
    
}
