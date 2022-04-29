#include "msg_functions.h"

#include <Arduino.h>

#include "gvars.h"

#include "comms.h"
#include "utils.h"
#include "protocol.h"
#include "settings.h"
#include "consensus.h"


void msg_d(uint8_t sender_id, float val){
    duty_cycle_backup = val;
    i2c_cmd_send(sender_id, R_CMD_d);
}

void msg_gd(uint8_t sender_id){
    i2c_cmd_float_send(sender_id, R_CMD_gd, duty_cycle);
}

void msg_r(uint8_t sender_id, float val){
    reference_backup = val;
    i2c_cmd_send(sender_id, R_CMD_r);
}

void msg_gr(uint8_t sender_id) {
    i2c_cmd_float_send(sender_id, R_CMD_gr, reference);
}

void msg_gl(uint8_t sender_id){
    i2c_cmd_float_send(sender_id, R_CMD_gl, measured_illuminance);
}

void msg_o(uint8_t sender_id, uint8_t val) {
    if(occupancy != (val == 1)){
        occupancy = (val == 1);

        if(occupancy) reference_lower_bound = OCCUPIED_REFERENCE;
        else reference_lower_bound = UNOCCUPIED_REFERENCE;
        reference_lower_bound_changed = true;
    }
    
    i2c_cmd_send(sender_id, R_CMD_o);
}

void msg_go(uint8_t sender_id) {
    uint8_t occ_byte = occupancy ? 1 : 0;
    Serial.printf("msg go = %d\n", occ_byte);
    i2c_cmd_byte_send(sender_id, R_CMD_go, occ_byte);
}

void msg_a(uint8_t sender_id, uint8_t val) {
    anti_windup_enabled = (val == 1);
    i2c_cmd_send(sender_id, R_CMD_a);
}

void msg_ga(uint8_t sender_id) {
    uint8_t aw_byte = anti_windup_enabled ? 1 : 0;
    i2c_cmd_byte_send(sender_id, R_CMD_ga, aw_byte);
}

void msg_w(uint8_t sender_id, uint8_t val) {
    ff_control_enabled = (val == 1);
    i2c_cmd_send(sender_id, R_CMD_w);
}

void msg_gw(uint8_t sender_id) {
    uint8_t ff_byte = ff_control_enabled ? 1 : 0;
    i2c_cmd_byte_send(sender_id, R_CMD_gw, ff_byte);
}

void msg_b(uint8_t sender_id, uint8_t val) {
	fb_control_enabled = (val == 1);
	i2c_cmd_send(sender_id, R_CMD_b);
}

void msg_gb(uint8_t sender_id) {
    uint8_t fb_byte = fb_control_enabled ? 1 : 0;
    i2c_cmd_byte_send(sender_id, R_CMD_gb, fb_byte);
}

void msg_gx(uint8_t sender_id) {
    i2c_cmd_float_send(sender_id, R_CMD_gx, external_illumination);
}

void msg_gp(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_gp, duty_cycle);
}

void msg_gt(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_gt, (float) (micros() - t_start) / 1000000.0); 
}

void msg_sl(uint8_t sender_id) {
    if (stream_l_enabled) {
        stream_l_enabled = false;
    } else {
        stream_l_enabled = true;
    }
    i2c_cmd_send(sender_id, R_CMD_sl);
}

void msg_sd(uint8_t sender_id) {
	if (stream_d_enabled) {
        stream_d_enabled = false;
    } else {
        stream_d_enabled = true;
    }
    i2c_cmd_send(sender_id, R_CMD_sd);
}

void msg_Bl(uint8_t sender_id) {

}

void msg_Bd(uint8_t sender_id) {

}

void msg_ge(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_ge, energy);
}

void msg_gv(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_gp, visibility_error_sum / (float) n_samples);
}

void msg_gf(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_gf, flicker_sum / (float) n_samples);
}



/* PARTE II */

void msg_gO(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_gO, occupied_reference);
}

void msg_gU(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_gU, unoccupied_reference);
}

void msg_gL(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_gL, reference_lower_bound);
}

void msg_gc(uint8_t sender_id) {
	i2c_cmd_float_send(sender_id, R_CMD_gc, cost);
}

void msg_O(uint8_t sender_id, float val) {
	occupied_reference = val;
    if(occupancy) reference_lower_bound_changed = true;
        
    i2c_cmd_send(sender_id, R_CMD_O);
}

void msg_U(uint8_t sender_id, float val) {
	unoccupied_reference = val;
    if(!occupancy) reference_lower_bound_changed = true;

    i2c_cmd_send(sender_id, R_CMD_U);
}

void msg_c(uint8_t sender_id, float val) {
    if(cost != val){
        cost = val;
        node.new_cost(cost);
        reference_lower_bound_changed = true;
    }

    i2c_cmd_send(sender_id, R_CMD_c);
}

void msg_R() {
    reset_enabled = true;
    i2c_cmd_send(BROADCAST, R_CMD_R);
}


//** REPLIES

// PARTE I

void r_msg_d(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" d %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_gd(uint8_t sender_id, float val) {
    Serial.printf("d %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_r(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" r %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_gr(uint8_t sender_id, float val){
    Serial.printf("r %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_gl(uint8_t sender_id, float val){
    Serial.printf("l %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_o(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" o %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_go(uint8_t sender_id, uint8_t val){
    Serial.printf("r_msg go = %d\n", val);
    Serial.printf("o %d %d\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_a(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" a %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_ga(uint8_t sender_id, uint8_t val){
    Serial.printf("a %d %d\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_w(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" w %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_gw(uint8_t sender_id, uint8_t val){
    Serial.printf("w %d %d\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_b(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" b %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_gb(uint8_t sender_id, uint8_t val){
    Serial.printf("b %d %d\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_gx(uint8_t sender_id, float val){
    Serial.printf("x %d %f\n", index_of(sender_id, node_id, node_count), val);
}
    
void r_msg_gp(uint8_t sender_id, float val){
    Serial.printf("p %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_gt(uint8_t sender_id, float val){
    Serial.printf("t %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_sl(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" sl %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_sd(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" sd %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_Bl(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" Bl %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_Bd(uint8_t sender_id)  {
    Serial.printf("ack");
    Serial.printf(" Bd %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_ge(uint8_t sender_id, float val){
    Serial.printf("e %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_gv(uint8_t sender_id, float val){
    Serial.printf("v %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_gf(uint8_t sender_id, float val){
    Serial.printf("f %d %f\n", index_of(sender_id, node_id, node_count), val);
}

/* PARTE II */

void r_msg_gO(uint8_t sender_id, float val){
    Serial.printf("O %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_gU(uint8_t sender_id, float val){
    Serial.printf("U %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_gL(uint8_t sender_id, float val){
    Serial.printf("L %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_gc(uint8_t sender_id, float val){
    Serial.printf("c %d %f\n", index_of(sender_id, node_id, node_count), val);
}

void r_msg_O(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" O %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_U(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" U %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_c(uint8_t sender_id) {
    Serial.printf("ack");
    Serial.printf(" c %d\n", index_of(sender_id, node_id, node_count));
}

void r_msg_R() {
    Serial.printf("ack");
    Serial.printf(" R\n");
    r_ack_count++;
}

void msg_stream_l(uint8_t sender_id, float val){
    Serial.printf("s l %d %f lx %lu ms", sender_id, val, millis());
}

void msg_stream_d(uint8_t sender_id, float val){
    Serial.printf("s d %d %f %% %lu ms", sender_id, val*100, millis());
}