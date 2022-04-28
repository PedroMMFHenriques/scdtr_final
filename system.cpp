#include "system.h"
#include "settings.h"
#include <math.h>
#include "gvars.h"

int dc_to_DAC(float dc) {
    return (int) (dc * (PWM_DC_RANGE - 1));
}

float adc_to_volt(int adc) {
    return adc / (float) (PWM_DC_RANGE - 1) * ADC_REF;
}

float volt_to_lux(float volt) {
    float R = LDR_R * ADC_REF / volt - LDR_R;
    return pow(10.0, (log10(R) - ldr_b) / ldr_m);
}

float adc_to_lux (int adc){
    return volt_to_lux(adc_to_volt(adc));
}