#pragma once

#include <stdint.h>

void calibrate_external_illum();
void calibrate_gains_low();
void calibrate_gains_high();
void calibrate_gains_finish();
void measure_handler(uint8_t id);

void measure_msg_handler(uint8_t id);
void calibrate_msg_handler();
void cal_done_msg_handler();
void cal_ack_msg_handler();