#pragma once

#include <stdint.h>

void msg_d(uint8_t sender_id, float val);

void msg_gd(uint8_t sender_id);

void msg_r(uint8_t sender_id, float val);

void msg_gr(uint8_t sender_id);

void msg_gl(uint8_t sender_id);

void msg_o(uint8_t sender_id, uint8_t val);

void msg_go(uint8_t sender_id);

void msg_a(uint8_t sender_id, uint8_t val);

void msg_ga(uint8_t sender_id);

void msg_w(uint8_t sender_id, uint8_t val);

void msg_gw(uint8_t sender_id);

void msg_b(uint8_t sender_id, uint8_t val);

void msg_gb(uint8_t sender_id);

void msg_gx(uint8_t sender_id);

void msg_gp(uint8_t sender_id);

void msg_gt(uint8_t sender_id);

void msg_sl(uint8_t sender_id);

void msg_sd(uint8_t sender_id);

void msg_Bl(uint8_t sender_id);

void msg_Bd(uint8_t sender_id);

void msg_ge(uint8_t sender_id);

void msg_gv(uint8_t sender_id);

void msg_gf(uint8_t sender_id);

/* PARTE II */

void msg_gO(uint8_t sender_id);

void msg_gU(uint8_t sender_id);

void msg_gL(uint8_t sender_id);

void msg_gc(uint8_t sender_id);

void msg_O(uint8_t sender_id, float val);

void msg_U(uint8_t sender_id, float val);

void msg_c(uint8_t sender_id, float val);

void msg_R();


//** REPLIES 😎😎😎🤡🤡💀

// PARTE I

void r_msg_d(uint8_t sender_id);

void r_msg_gd(uint8_t sender_id, float val);

void r_msg_r(uint8_t sender_id);

void r_msg_gr(uint8_t sender_id, float val);

void r_msg_gl(uint8_t sender_id, float val);

void r_msg_o(uint8_t sender_id);

void r_msg_go(uint8_t sender_id, uint8_t val);

void r_msg_a(uint8_t sender_id);

void r_msg_ga(uint8_t sender_id, uint8_t val);

void r_msg_w(uint8_t sender_id);

void r_msg_gw(uint8_t sender_id, uint8_t val);

void r_msg_b(uint8_t sender_id);

void r_msg_gb(uint8_t sender_id, uint8_t val);

void r_msg_gx(uint8_t sender_id, float val);

void r_msg_gp(uint8_t sender_id, float val);

void r_msg_gt(uint8_t sender_id, float val);

void r_msg_sl(uint8_t sender_id);

void r_msg_sd(uint8_t sender_id);

void r_msg_Bl(uint8_t sender_id);

void r_msg_Bd(uint8_t sender_id);

void r_msg_ge(uint8_t sender_id, float val);

void r_msg_gv(uint8_t sender_id, float val);

void r_msg_gf(uint8_t sender_id, float val);

/* PARTE II */

void r_msg_gO(uint8_t sender_id, float val);

void r_msg_gU(uint8_t sender_id, float val);

void r_msg_gL(uint8_t sender_id, float val);

void r_msg_gc(uint8_t sender_id, float val);

void r_msg_O(uint8_t sender_id);

void r_msg_U(uint8_t sender_id);

void r_msg_c(uint8_t sender_id);

void r_msg_R();