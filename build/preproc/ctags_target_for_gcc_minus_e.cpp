# 1 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino"
# 2 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino" 2
# 3 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino" 2

# 5 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino" 2
# 6 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino" 2
# 7 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino" 2
# 8 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino" 2
# 9 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino" 2
# 10 "/home/nunu/Arduino/scdtr_final_pedro/scdtr_final_pedro.ino" 2

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
    Serial.begin(115200);

    analogReadResolution(12);
    analogWriteFreq(60000);
    analogWriteRange(4096);

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
    //cooler message parser 😎😎😎
    etl::vector<uint8_t, 32> msg;
    while(!msg_queue.empty()) {
        msg_queue.pop(msg);
        msg_handler_parser(msg.data(), msg.size());
    }

    switch (state) {
    case discovery:
        Serial.printf("Discovering...\n");
        node_discovery();
        if (node_count == 3) {
            node_discovery();
            sort3(node_id);
            state = waiting_callibration;
            calibrate_external_illum();
        }
        break;

    case waiting_callibration:
        Serial.printf("Wating callibration...\n");
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
        Serial.printf("Wating low point acks...\n");
        delay(1);
        if (cal_ack_count == 3) {
            cal_ack_count = 0;
            state = callibration_high;
            calibrate_gains_high();
        }
        break;

    case callibration_high:
        Serial.printf("Wating high point acks...\n");
        delay(1);
        if (cal_ack_count == 3) {
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
        break;

    default:
        break;
    }


    if(timer_fired){

        timer_fired = false;
    }
}
