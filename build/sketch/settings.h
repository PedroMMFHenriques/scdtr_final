#line 1 "/home/nunu/Arduino/scdtr_final_pedro/settings.h"
#pragma once

#define I2C0_SDA 4
#define I2C0_SCL 5
#define I2C1_SDA 6
#define I2C1_SCL 7

#define I2C_FREQ 100000

#define LED_PIN 15
#define LDR_PIN A0

#define SERIAL_BAUD 115200

#define ADC_RES 12

#define ADC_REF 3.3

#define PWM_FREQ 60000

#define PWM_DC_RANGE 4096

#define N_NODES 3

#define N_MEASURES_CAL 10

#define LDR_R 10000.0

#define LDR_B 6.15

#define LDR_M -1.0

#define DC_CAL_HIGH 0.9

#define DC_CAL_LOW 0.1

#define MSG_SIZE 32

#define MSG_QUEUE_SIZE 10

//in ms
#define TIMESTEP 10








#define CBUFFER_SIZE 6000



#define TAU_ASC_K 0.044

#define TAU_ASC_EXP -0.323

#define TAU_DES_K 0.02

#define TAU_DES_EXP -0.442

// in us
#define DEADTIME 2000

#define K_P 0.012

#define K_I 0.12