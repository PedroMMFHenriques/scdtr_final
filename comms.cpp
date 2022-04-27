#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>

#include "comms.h"
#include "gvars.h"
#include "settings.h"
#include "utils.h"
#include "calibrations.h"
#include "protocol.h"
#include "msg_queue.h"

#include "msg_functions.h"


void msg_handler_parser(uint8_t *msg, int len);

void wakeup_msg_handler(uint8_t id);
void measure_msg_handler(uint8_t id);
void calibrate_msg_handler();
void cal_done_msg_handler();
void cal_ack_msg_handler();



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

void i2c_cmd_float_send(uint8_t id, uint8_t cmd, float val) {
    uint8_t msg[MSG_SIZE];
    int msg_size;
    uint8_t *val_byte_array = (uint8_t*) (&val);

    msg_size = msg_formatter(msg, cmd, val_byte_array, 4);
    i2c_send(id, msg, msg_size);
}

void i2c_cmd_byte_send(uint8_t id, uint8_t cmd, uint8_t val) {
    uint8_t msg[MSG_SIZE];
    int msg_size;

    msg_size = msg_formatter(msg, cmd, &val, 1);
    i2c_send(id, msg, msg_size);
}

void i2c_cmd_send(uint8_t id, uint8_t cmd) {
    uint8_t msg[MSG_SIZE];
    int msg_size;

    msg_size = msg_formatter(msg, cmd, NULL, 0);
    i2c_send(id, msg, msg_size);
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

float byte_array_to_float(uint8_t *byte_array) {
    float val;
    uint8_t *val_byte_array = (uint8_t *) (&val);
    memcpy(val_byte_array, byte_array, 4);
    return val;
}

void msg_handler_parser(uint8_t *msg, int len) {
    float val;

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

    case CMD_d:
        val = byte_array_to_float(&msg[3]);
        msg_d(msg[0], val);
        break;
    case CMD_gd:
        msg_gd(msg[0]);
        break;
    case CMD_r:
        val = byte_array_to_float(&msg[3]);
        msg_r(msg[0], val);
        break;
    case CMD_gr:
        msg_gr(msg[0]);
        break;
    case CMD_gl:
        msg_gl(msg[0]);
        break;
    case CMD_o:
        msg_o(msg[0], msg[3]);
        break;
    case CMD_go:
        msg_go(msg[0]);
        break;
    case CMD_a:
        msg_a(msg[0], msg[3]);
        break;
    case CMD_ga:
        msg_ga(msg[0]);
        break;
    case CMD_w:
        msg_w(msg[0], msg[3]);
        break;
    case CMD_gw:
        msg_gw(msg[0]);
        break;
    case CMD_b:
        msg_b(msg[0], msg[3]);
        break;
    case CMD_gb:
        msg_gb(msg[0]);
        break;
    case CMD_gx:
        msg_gx(msg[0]);
        break;
    case CMD_gp:
        msg_gp(msg[0]);
        break;
    case CMD_gt:
        msg_gt(msg[0]);
        break;
    case CMD_sl:
        msg_sl(msg[0]);
        break;
    case CMD_sd:
        msg_sd(msg[0]);
        break;
    case CMD_Bl:
        msg_Bl(msg[0]);
        break;
    case CMD_Bd:
        msg_Bd(msg[0]);
        break;
    case CMD_ge:
        msg_ge(msg[0]);
        break;
    case CMD_gv:
        msg_gv(msg[0]);
        break;
    case CMD_gf:
        msg_gf(msg[0]);
        break;
    case CMD_gO:
        msg_gO(msg[0]);
        break;
    case CMD_gU:
        msg_gU(msg[0]);
        break;
    case CMD_gL:
        msg_gL(msg[0]);
        break;
    case CMD_gc:
        msg_gc(msg[0]);
        break;  
    case CMD_O:
        val = byte_array_to_float(&msg[3]);
        msg_O(msg[0], val);
        break;
    case CMD_U:
        val = byte_array_to_float(&msg[3]);
        msg_U(msg[0], val);
        break;
    case CMD_c:
        val = byte_array_to_float(&msg[3]);
        msg_c(msg[0], val);
        break;
    case CMD_R:
        msg_R();
        break;  
    case R_CMD_d:
        r_msg_d(msg[0]);
        break;
    case R_CMD_gd:
        val = byte_array_to_float(&msg[3]);
        r_msg_gd(msg[0], val);
        break;
    case R_CMD_r:
        r_msg_r(msg[0]);
        break;
    case R_CMD_gr:
        val = byte_array_to_float(&msg[3]);
        r_msg_gr(msg[0], val);
        break;
    case R_CMD_gl:
        val = byte_array_to_float(&msg[3]);
        r_msg_gl(msg[0], val);
        break;
    case R_CMD_o:
        r_msg_o(msg[0]);
        break;
    case R_CMD_go:
        Serial.printf("msg[3]] = %d\n", msg[3]);
        r_msg_go(msg[0], msg[3]);
        break;
    case R_CMD_a:
        r_msg_a(msg[0]);
        break;
    case R_CMD_ga:
        r_msg_ga(msg[0], msg[3]);
        break;
    case R_CMD_w:
        r_msg_w(msg[0]);
        break;
    case R_CMD_gw:
        r_msg_gw(msg[0], msg[3]);
        break;
    case R_CMD_b:
        r_msg_b(msg[0]);
        break;
    case R_CMD_gb:
        r_msg_gb(msg[0], msg[3]);
        break;
    case R_CMD_gx:
        val = byte_array_to_float(&msg[3]);
        r_msg_gx(msg[0], val);
        break;
    case R_CMD_gp:
        val = byte_array_to_float(&msg[3]);
        r_msg_gp(msg[0], val);
        break;
    case R_CMD_gt:
        val = byte_array_to_float(&msg[3]);
        r_msg_gt(msg[0], val);
        break;
    case R_CMD_sl:
        r_msg_sl(msg[0]);
        break;
    case R_CMD_sd:
        r_msg_sd(msg[0]);
        break;
    case R_CMD_Bl:
        r_msg_Bl(msg[0]);
        break;
    case R_CMD_Bd:
        r_msg_Bd(msg[0]);
        break;
    case R_CMD_ge:
        val = byte_array_to_float(&msg[3]);
        r_msg_ge(msg[0], val);
        break;
    case R_CMD_gv:
        val = byte_array_to_float(&msg[3]);
        r_msg_gv(msg[0], val);
        break;
    case R_CMD_gf:
        val = byte_array_to_float(&msg[3]);
        r_msg_gf(msg[0], val);
        break;
    case R_CMD_gO:
        val = byte_array_to_float(&msg[3]);
        r_msg_gO(msg[0], val);
        break;
    case R_CMD_gU:
        val = byte_array_to_float(&msg[3]);
        r_msg_gU(msg[0], val);
        break;
    case R_CMD_gL:
        val = byte_array_to_float(&msg[3]);
        r_msg_gL(msg[0], val);
        break;
    case R_CMD_gc:
        val = byte_array_to_float(&msg[3]);
        r_msg_gc(msg[0], val);
        break;
    case R_CMD_O:
        r_msg_O(msg[0]);
        break;
    case R_CMD_U:
        r_msg_U(msg[0]);
        break;
    case R_CMD_c:
        r_msg_c(msg[0]);
        break;
    case R_CMD_R:
        r_msg_R();
        break;

    default:
        break;
    }
}

