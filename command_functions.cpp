#include "command_functions.h"

#include <stdio.h>

#include <Arduino.h>

#include "gvars.h"

#include "comms.h"

#include "protocol.h"


void cmd_d(char *i_str, char *val_str){
    int i;
    float val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%f", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val < 0.0 || val > 1.0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_float_send(node_id[i], CMD_d, val);
    return;
}

void cmd_gd(char *i_str){
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gd);
    return;
}

void cmd_r(char *i_str, char *val_str){
    int i;
    float val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%f", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val < 0.0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_float_send(node_id[i], CMD_r, val);
    return;
}

void cmd_gr(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gr);
    return;
}

void cmd_gl(char *i_str){
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gl);
    return;
}

void cmd_o(char *i_str, char *val_str) {
    int i, val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%d", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val != 0 && val != 1) {
        Serial.println("err");
        return;
    }
    i2c_cmd_byte_send(node_id[i], CMD_o, val);
    return;
}

void cmd_go(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_go);
    return;
}

void cmd_a(char *i_str, char *val_str) {
    int i, val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%d", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val != 0 && val != 1) {
        Serial.println("err");
        return;
    }
    i2c_cmd_byte_send(node_id[i], CMD_a, val);
    return;
}

void cmd_ga(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_ga);
    return;
}

void cmd_w(char *i_str, char *val_str) {
    int i, val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%d", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val != 0 && val != 1) {
        Serial.println("err");
        return;
    }
    i2c_cmd_byte_send(node_id[i], CMD_w, val);
}

void cmd_gw(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gw);
    return;
}

void cmd_b(char *i_str, char *val_str) {
    int i, val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%d", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val != 0 && val != 1) {
        Serial.println("err");
        return;
    }
    i2c_cmd_byte_send(node_id[i], CMD_b, val);
}

void cmd_gb(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gb);
    return;
}

void cmd_gx(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gx);
    return;
}

void cmd_gp(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gp);
    return;
}

void cmd_gt(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gt);
    return;
}

void cmd_sl(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_sl);
    return;
}

void cmd_sd(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_sd);
    return;
}

void cmd_Bl(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_Bl);
    return;
}

void cmd_Bd(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_Bd);
    return;
}

void cmd_ge(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_ge);
    return;
}

void cmd_gv(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gv);
    return;
}

void cmd_gf(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gf);
    return;
}



/* PARTE II */

void cmd_gO(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gO);
    return;
}

void cmd_gU(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gU);
    return;
}

void cmd_gL(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gL);
    return;
}

void cmd_gc(char *i_str) {
    int i;
    if(sscanf(i_str, "%d", &i) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_send(node_id[i], CMD_gc);
    return;
}

void cmd_O(char *i_str, char *val_str) {
    int i;
    float val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%f", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val < 0.0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_float_send(node_id[i], CMD_O, val);
    return;
}

void cmd_U(char *i_str, char *val_str) {
    int i;
    float val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%f", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val < 0.0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_float_send(node_id[i], CMD_O, val);
    return;
}

void cmd_c(char *i_str, char *val_str) {
    int i;
    float val;
    if(sscanf(i_str, "%d", &i) != 1 || sscanf(val_str, "%f", &val) != 1) {
        Serial.println("err");
        return;
    }
    if(i > 2 || i < 0) {
        Serial.println("err");
        return;
    }
    if(val < 0.0) {
        Serial.println("err");
        return;
    }
    i2c_cmd_float_send(node_id[i], CMD_c, val);
    return;
}

void cmd_R() {
    i2c_cmd_send(BROADCAST, CMD_R);
    return;
}