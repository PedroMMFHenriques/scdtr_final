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
    uint8_t msg[MSG_SIZE];
    uint8_t data[1];

    data[0] = my_id;

    int data_size = 1;

    int msg_size = msg_formatter(msg, MEASURE, data, data_size);

    i2c_send(BROADCAST, msg, msg_size);
}

void calibrate_gains_high() {
    analogWrite(LED_PIN, dc_to_DAC(DC_CAL_HIGH));
    delay(1000);
    
    // broadcast MEASURE ME
    uint8_t msg[MSG_SIZE];
    uint8_t data[1];

    data[0] = my_id;

    int data_size = 1;

    int msg_size = msg_formatter(msg, MEASURE, data, data_size);

    i2c_send(BROADCAST, msg, msg_size);
}

void calibrate_gains_finish() {
    uint8_t msg[MSG_SIZE];
    int msg_size;

    analogWrite(LED_PIN, 0);

    // if node is not the last one
    if (node_id[N_NODES - 1] != my_id) {
        // message KAWIBRATE para node 'a frente;
        msg_size = msg_formatter(msg, CALIBRATE, NULL, 0);
        i2c_send(node_id[index_of(my_id, node_id, N_NODES) + 1], msg, msg_size);
    } else
    {
        // broadcast ALL DONE
        msg_size = msg_formatter(msg, DONE_CALIBRATE, NULL, 0);
        i2c_send(BROADCAST, msg, msg_size);
    }
}


void measure_handler(uint8_t id) {
    static float low_point = 0;
    float high_point;
    int sum = 0;
    
    for (int i = 0; i < N_MEASURES_CAL; i++)
    {
        sum += analogRead(LDR_PIN);
    }

    if (!low_point)
    {
        low_point = adc_to_lux(sum) / N_MEASURES_CAL;
    }
    else
    {
        high_point = adc_to_lux(sum) / N_MEASURES_CAL;
        gain[index_of(my_id, node_id, N_NODES)] = (high_point - low_point) / (DC_CAL_HIGH - DC_CAL_LOW);
        low_point = 0;
    }
    
    // send ACK
    uint8_t msg[MSG_SIZE];
    int msg_size;   
    
    msg_size = msg_formatter(msg, MEASURE_ACK, NULL, 0);
    i2c_send(id, msg, msg_size);
}
