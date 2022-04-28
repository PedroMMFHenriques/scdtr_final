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

    //meter numa função????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????'
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
            
            float g1 = 0.5, g2 = 0.5, g3 = 0.5; //DIVIDIR GANHOS POR 100???????? ou mudar DC_CAL_HIGH 0.9 para 90????
            if (my_id == node_id[0]) {
                g1 = 2;
            } else if(my_id == node_id[1]) {
                g2 = 2;
            } else {
                g3 = 2;
            }

            int idx = index_of(my_id, node_id, N_NODES);
            Eigen::Vector3f K(g1, g2, g3);
            node.init_cons(idx, 1.0, cost, K);

            state = normal;
            Serial.printf("Initialization done!\n");
            Serial.printf("My ID: %X\n", my_id);
            Serial.printf("IDs: %X %X %X \n", node_id[0], node_id[1], node_id[2]);
            Serial.printf("External Ilumination: %f\n", external_illumination);
            Serial.printf("Gains: %f %f %f\n\n\n", gain[0], gain[1], gain[2]);

            if(!startup) cancel_repeating_timer(&timer);
            startup = false;

            t_start = micros(); //elapsed timer
            add_repeating_timer_ms(- TIMESTEP, timer_ISR, NULL, &timer);
        }
        break;


    case normal:
        cli();

        // Serial.printf("External Ilumination: %f\n", external_illumination);
        // Serial.printf("Gains: %f %f %f\n\n\n", gain[0], gain[1], gain[2]);
        // delay(10000);

        if(reference_lower_bound_changed){
            reference_lower_bound_changed = false;
            start_consensus();
        }

        if(timer_fired){
            timer_fired = false;
            state = sampling;
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
        Serial.printf("Waiting consensus... (iter %d)\n", iter_num);   
        break;

    case consensus_wait:
        if (cons_ack_count == N_NODES) {
            Eigen::Vector3f d_av = (dc[0] + dc[1] + dc[2])/3;
            // Serial.printf("Received dc: \n");
            // Serial.printf("dc_0: %f %f %f \n", dc[0](0), dc[0](1), dc[0](2));
            // Serial.printf("dc_1: %f %f %f \n", dc[1](0), dc[1](1), dc[1](2));
            // Serial.printf("dc_2: %f %f %f \n", dc[2](0), dc[2](1), dc[2](2));
            // Serial.printf("d_av: %f %f %f \n", d_av(0), d_av(1), d_av(2));
            node.update_cons(d_av);

            state = consensus_calc;
        }
        break;
    
    case sampling:

        //measure illuminance
        float Yadc_sum = 0;
        //mean digital filter
        for(int i = 0; i < SAMPLES_PER_MEASUREMENT; i++){ 
            Yadc_sum += analogRead(ADC0_PIN);
        }

        measured_illuminance = adc_to_lux(Yadc_sum/SAMPLES_PER_MEASUREMENT);
        n_samples++;

        //turn on/off feedforward control
        if(ff_control_enabled) ff_duty_cycle = duty_cycle_consensus; 
        else ff_duty_cycle = 0;

        //turn on/off feedback control
        if(fb_control_enabled){ 
            if(reference_changed){ 
                reference_changed = false;
        
                float simLux = reference;
                //if(reference/static_gain >= 1) simLux = static_gain; COMO LIMITAR O MAXIMO??? O CONSENSUS JÁ FAZ ISSO?

                //calculate tau and readies the class
                simu.dim_change(gain[index_of(my_id, node_id, N_NODES)], adc_to_volt(measured_illuminance, simLux, 0);

                t_startSim = micros();
            }

            //having the tau, just needs to calculate voltage
            float fb_Rlux = volt_to_lux(simu.get_vt((micros() - t_startSim)/pow(10,6))); //new lux reference for the FB based on the simulator
            fb_duty_cycle = cont.calc_pwm(fb_Rlux, measured_illuminance, anti_windup_enabled, ff_duty_cycle/100) * 100;
        }
        else fb_duty_cycle = 0;

        //sum the components of feedforward and feedback
        duty_cycle = ff_duty_cycle + fb_duty_cycle;
        if(!ff_control_enabled && !fb_control_enabled) duty_cycle = duty_cycle_backup;
        
        if(duty_cycle > 100) duty_cycle = 100;
        else if(duty_cycle < 0) duty_cycle = 0;
        analogWrite(LED_PIN, dc_to_DAC(duty_cycle/100));


        break;

    default:
        break;
    }
    
}
