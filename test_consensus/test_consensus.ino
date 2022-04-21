#include <Wire.h>
#include <hardware/flash.h> 

#include "consensus.h"

using namespace std;

int DAC_RANGE = 4095;
int analogReadRes = {12};
uint8_t my_full_id[8];
uint8_t my_id;
uint8_t node_id[3];
int node_count = 0;
int frequency = 100000;
int LED_PIN = 15;

char buff[100];

float gain[3];

//////////////////////////////////////////////
//ATENÇÃO!!!! d de 0 a 100
float rho = 1;
int max_iter = 5;
/////////////////////////////////////////////


void recv_handler(int len) {

}

void req_handler() {

}

void setup() {
  Serial.begin(115200);


  //I2C 0 is master, I2C 1 is slave
  Wire.setSDA(4); Wire.setSCL(5); 
  Wire1.setSDA(6);Wire1.setSCL(7); 
  
  Wire.setClock(frequency);
  Wire1.setClock(frequency);
  
  analogReadResolution(analogReadRes); //default is 12
  analogWriteFreq(60000); //60KHz, about max
  analogWriteRange(DAC_RANGE); //100% duty cycle
  
  flash_get_unique_id(my_full_id);
  my_id = my_full_id[6] & 0b01111111; //set 1st bit to 0

  Wire.begin();
  Wire1.begin(my_id);

  Wire1.onReceive(recv_handler);
  Wire1.onRequest(req_handler) ;

  delay(3000);
}

float c = 1;
float k11 = 2, k12 = 0.5, k13 = 0.5;
Vector3f K(k11, k12, k13); 

void loop() {

  //wake up
  int idx = 0; //A SUA POSIÇÃO NO VETOR DE NÓS ORDENADO
  cons node(rho, max_iter, idx);
  //calibrate

  float L = 150, o = 30;
  Serial.println(node.calc_cons(L, o, c, K));
  
  delay(2000);

}
