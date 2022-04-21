#line 1 "/home/nunu/Arduino/scdtr_final/scdtr_final.ino"
#include <Arduino.h>
#include <hardware/flash.h>

#include "settings.h"
#include "comms.h"
#include "calibrations.h"
#include "gvars.h"

// using namespace std;
// ^this is evil. don't.

#line 12 "/home/nunu/Arduino/scdtr_final/scdtr_final.ino"
void setup();
#line 43 "/home/nunu/Arduino/scdtr_final/scdtr_final.ino"
void loop();
#line 12 "/home/nunu/Arduino/scdtr_final/scdtr_final.ino"
void setup() {
    Serial.begin(SERIAL_BAUD);

    analogReadResolution(ADC_RES);
    analogWriteFreq(PWM_FREQ);
    analogWriteRange(PWM_DC_RANGE);

    // 😮‍💨😮‍💨
    uint8_t my_full_id[8];
    flash_get_unique_id(my_full_id);
    my_id = my_full_id[6] & 0b01111111; // set MSB to 0

    // boku no id 😔😔
    initI2C(my_id);

    delay(3000); //make sure all devices are awake at this point

    // Discover your mom 🤭🤭
    node_discovery();

    for (;;) {
        Serial.println("discovery ok");
        delay(1000);
    }
    

    // Calibrate deez 🥜🥜 
    calibrate();

}

void loop() {
    Serial.printf("My ID: %X\n", my_id);

    Serial.printf("IDs: %X %X %X \n", node_id[0], node_id[1], node_id[2]);

    Serial.printf("External Ilumination: %f\n", external_illumination);

    Serial.printf("Gains: %f %f %f\n\n\n", gain[0], gain[1], gain[2]);

    delay(2000);
}

