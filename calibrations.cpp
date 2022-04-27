#include "calibrations.h"
#include <Arduino.h>
#include "gvars.h"
#include "comms.h"
#include "settings.h"
#include "system.h"
#include "utils.h"
#include "protocol.h"


void calibrate_external_illum(){
    //turn off all LEDs
    analogWrite(LED_PIN, 0);

    delay(1000);

    //measure external illuminance
    int sum = 0;
    for (int i = 0; i < N_MEASURES_CAL; i++) {
        sum += analogRead(LDR_PIN);
    }

    external_illumination = adc_to_lux(sum) / N_MEASURES_CAL;

    delay(100);
}


/**
 * @brief this calibrates the sensors
 * 
 */
void calibrate_gains_low() {
    analogWrite(LED_PIN, dc_to_DAC(DC_CAL_LOW));
    delay(1000);
    
    // broadcast MEASURE ME
    i2c_cmd_byte_send(BROADCAST, MEASURE, my_id);
}

void calibrate_gains_high() {
    analogWrite(LED_PIN, dc_to_DAC(DC_CAL_HIGH));
    delay(1000);
    
    // broadcast MEASURE ME
    i2c_cmd_byte_send(BROADCAST, MEASURE, my_id);
}

void calibrate_gains_finish() {
    // uint8_t msg[MSG_SIZE];
    // int msg_size;

    analogWrite(LED_PIN, 0);

    // if node is not the last one
    if (node_id[N_NODES - 1] != my_id) {
        // message KAWIBRATE para node 'a frente;
        // msg_size = msg_formatter(msg, CALIBRATE, NULL, 0);
        // i2c_send(node_id[index_of(my_id, node_id, N_NODES) + 1], msg, msg_size);
        i2c_cmd_send(node_id[index_of(my_id, node_id, N_NODES) + 1], CALIBRATE);
    } else
    {
        // broadcast ALL DONE
        // msg_size = msg_formatter(msg, DONE_CALIBRATE, NULL, 0);
        // i2c_send(BROADCAST, msg, msg_size);
        i2c_cmd_send(BROADCAST, DONE_CALIBRATE);
    }
}


void measure_handler(uint8_t id) {
    static float low_point = 0;
    float high_point;
    int avg;
    int sum = 0;
    
    for (int i = 0; i < N_MEASURES_CAL; i++)
    {
        sum += analogRead(LDR_PIN);
    }

    if (!low_point)
    {
        avg = sum /  N_MEASURES_CAL;
        low_point = adc_to_lux(avg);
    }
    else
    { 
        avg = sum / N_MEASURES_CAL;
        high_point = adc_to_lux(avg);
        gain[index_of(id, node_id, N_NODES)] = (high_point - low_point) / (DC_CAL_HIGH - DC_CAL_LOW);
        low_point = 0;
    }
    
    // send ACK
    // uint8_t msg[MSG_SIZE];
    // int msg_size;   
    
    // msg_size = msg_formatter(msg, MEASURE_ACK, NULL, 0);
    // i2c_send(id, msg, msg_size);
    i2c_cmd_send(id, MEASURE_ACK);
}

void measure_msg_handler(uint8_t id) {
    
    measure_handler(id);
}

void calibrate_msg_handler() {
    
    calibrate_now = true;
}

void cal_done_msg_handler() {
    
    calibrate_all_done = true;
}

void cal_ack_msg_handler() {
    
    Serial.printf("recebi ack!!\n");
    cal_ack_count++;
}