#include <Arduino.h>
#include <hardware/flash.h>

#include "settings.h"
#include "comms.h"
#include "calibrations.h"
#include "gvars.h"
#include "utils.h"
#include "msg_queue.h"
#include "consensus.h"

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

    // 
    uint8_t my_full_id[8];
    flash_get_unique_id(my_full_id);
    my_id = my_full_id[6] & 0b01111111; // set MSB to 0

    // 
    initI2C(my_id);

    delay(3000); //make sure all devices are awake at this point

    //add_repeating_timer_ms(- TIMESTEP, timer_ISR, NULL, &timer);

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

            float idx = index_of(my_id, node_id, N_NODES);
            Eigen::Vector3f K(gain[0],gain[1],gain[2]);
            node.init_cons(idx, external_illumination, cost[idx], K);
            
            state = waiting_callibration;
            calibrate_external_illum();
        }
        break;

    case waiting_callibration:
        Serial.printf("Waiting callibration...\n");
        delay(1);
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
        }
        break;

    case callibration_finish:
        Serial.printf("Waiting all done...\n");
        delay(1);
        if (calibrate_all_done) {
            state = normal;
        }
        break;


    case normal:
        Serial.printf("My ID: %X\n", my_id);

        Serial.printf("IDs: %X %X %X \n", node_id[0], node_id[1], node_id[2]);

        Serial.printf("External Ilumination: %f\n", external_illumination);

        Serial.printf("Gains: %f %f %f\n\n\n", gain[0], gain[1], gain[2]);

        delay(1000);

        ////////////////////////////////////////
        // User muda a referência ou occupancy
        float L;
        switch (my_id)
        {
        case 45:
            L = 20;
            break;

        case 61:
            L = 10;
            break;

        case 97:
            L = 15;
            break;
        
        default:
            Serial.println("oopsie");
            break;
        }
        bool ref_change = true;
        ////////////////////////////////////////

        if(ref_change){
            ref_change = false;

            node.new_ref(L);
            iter_num = 0;
            state = consensus_calc;
        }

        break;


    case consensus_calc:
        Serial.printf("Calculating consensus...\n");
        if(iter_num == MAX_ITER){
            Serial.printf("Consensus DONE...\n");
            //guardar resultados node.d_av[node.idx]  node.K.transpose()*node.d + node.o
            [d_sol, l_sol] = node.get_results();
            
            state = normal;
            break;
        }

        //fazer send no fim do iter_cons???????????? ou + 1 função?
        node.iter_cons();
        iter_num++;
        
        cons_ack_count = 0;

        state = consensus_wait;        
        break;
    
    case consensus_wait:
        Serial.printf("Waiting consensus...\n");
        if (cons_ack_count == N_NODES) {
            Vector3f d_av(dc[0], dc[1], dc[2]);
            node.update_cons(d_av);

            state = consensus_calc;
        }
        break;


    default:
        break;
    }
    

    if(timer_fired){

        timer_fired = false;
    }
}
