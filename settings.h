#pragma once

#define I2C0_SDA 4
#define I2C0_SCL 5
#define I2C1_SDA 6
#define I2C1_SCL 7

#define I2C_FREQ 100000

#define LED_PIN 15
#define LDR_PIN A0

#define LED_POWER 1

#define SERIAL_BAUD 115200

#define ADC_RES 12

#define ADC_REF 3.3

#define PWM_FREQ 60000

#define PWM_DC_RANGE 4096

#define N_NODES 3

#define N_MEASURES_CAL 10

#define LDR_R 10000.0

#define LDR_B_2D 6.36
#define LDR_M_2D -1.05

#define LDR_B_3D 6.25
#define LDR_M_3D -1.00

#define LDR_B_61 6.85
#define LDR_M_61 -1.00

#define DC_CAL_HIGH 0.9

#define DC_CAL_LOW 0.1

#define MSG_SIZE 32

#define MSG_QUEUE_SIZE 10

//in ms
#define TIMESTEP 10
#define CONSENSUS_TIMEOUT 10000

#define UNOCCUPIED_REFERENCE 15.0
#define OCCUPIED_REFERENCE 50.0

#define COST 1.0

//consensus parameters
#define MAX_ITER 20
#define RHO 0.07

#define SAMPLES_PER_MEASUREMENT 20



#define CBUFFER_SIZE 6000



#define TAU_ASC_K 0.044

#define TAU_ASC_EXP -0.323

#define TAU_DES_K 0.02

#define TAU_DES_EXP -0.442

// in us
#define DEADTIME 2000

#define K_P 0.000012

#define K_I 0.012