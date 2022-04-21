#pragma once

#include <stdint.h>

void calibrate_external_illum();
void calibrate_gains_low();
void calibrate_gains_high();
void calibrate_gains_finish();
void measure_handler(uint8_t id);