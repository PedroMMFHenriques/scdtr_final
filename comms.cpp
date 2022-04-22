#include <Arduino.h>
#include <Wire.h>

#include "comms.h"
#include "gvars.h"
#include "settings.h"
#include "utils.h"
#include "calibrations.h"
#include "consensus.h"
#include "protocol.h"
#include "msg_queue.h"


void msg_handler_parser(uint8_t *msg, int len);

void wakeup_msg_handler(uint8_t id);
void measure_msg_handler(uint8_t id);
void calibrate_msg_handler();
void cal_done_msg_handler();
void cal_ack_msg_handler();
void cons_msg_handler();



void initI2C(uint8_t slave_id) {
    // I2C 0 is master, I2C 1 is slave
    Wire.setSDA(I2C0_SDA);
    Wire.setSCL(I2C0_SCL);
    Wire1.setSDA(I2C1_SDA);
    Wire1.setSCL(I2C1_SCL);

    Wire.setClock(I2C_FREQ);
    Wire1.setClock(I2C_FREQ);

    Wire.begin();
    Wire1.begin(slave_id);

    Wire1.onReceive(recv_handler);
    Wire1.onRequest(req_handler);

}

void recv_handler(int len) {
    etl::vector<uint8_t, MSG_SIZE> recv_buffer;

    for (int i = 0; i < len; i++)
    {
        recv_buffer.push_back(Wire1.read());
    }

    msg_queue.push_from_unlocked(recv_buffer);

}

void req_handler() {
    // please delete it!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1!!!111!!!!!1111!!!11!
    static int ctr = 765;
    char buff[7];
    sprintf(buff, "%06X", (ctr++) % 65535);
    Wire1.write(buff, 6);
}

void i2c_send(uint8_t id, uint8_t *msg, int len) {
    int write_err;
    //Serial.printf("VOU MANDAR %c PARA %X !!\n", msg[0], id);
    do
    {
        Wire.beginTransmission(id);
        Wire.write(msg, len);
        write_err = Wire.endTransmission();
    } while (write_err);
}

int msg_formatter(uint8_t *msg, uint8_t cmd, uint8_t *data, int data_size) {
    msg[0] = my_id;
    msg[1] = data_size + 3;
    msg[2] = cmd;
    if (data_size) {
        memcpy(&msg[3], data, data_size);
    }
    return msg[1]; //total message size
}

void node_discovery(void) {
    uint8_t msg[MSG_SIZE];
    uint8_t data[1];

    data[0] = my_id;

    int data_size = 1;

    int msg_size = msg_formatter(msg, WAKEUP, data, data_size);

    i2c_send(BROADCAST, msg, msg_size);

    delay(500);
}

void msg_handler_parser(uint8_t *msg, int len) {

    if (msg[1] != len) {
        Serial.printf("Spwish spwash, your message is trash!\n");
        return;
    }

    switch (msg[2]) {
    case WAKEUP:
        wakeup_msg_handler(msg[3]);
        break;
    case MEASURE:
        measure_msg_handler(msg[3]);
        break;
    case CALIBRATE:
        calibrate_msg_handler();
        break;
    case DONE_CALIBRATE:
        cal_done_msg_handler();
        break;
    case MEASURE_ACK:
        cal_ack_msg_handler();
        break;
    case CONSENSUS_SOLUTION:
        cons_msg_handler(msg[0], msg[3]);
        break;
    default:
        break;
    }
}


void wakeup_msg_handler(uint8_t id) {
    bool new_id = true;

    for(int i = 0; i < N_NODES; i++) {
        if (node_id[i] == id) {
            new_id = false;
            break;
        }
    }
    
    //if new id, add it
    if(new_id) node_id[node_count++] = id;
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
    cal_ack_count++;
}


void cons_msg_handler(uint8_t id, float dc) {
    cons_ack_count++;
    cons_handler()
}