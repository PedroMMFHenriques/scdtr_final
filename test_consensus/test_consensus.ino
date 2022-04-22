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
//ATENÇÃO!!!! d de 0 a 100!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
float rho = 0.07;
int max_iter = 50;
/////////////////////////////////////////////


void recv_handler(int len) {

}

void req_handler() {

}

void setup() {
  Serial.begin(115200);

  while(!Serial);
  //I2C 0 is master, I2C 1 is slave
  Wire.setSDA(4); Wire.setSCL(5); 
  Wire1.setSDA(6); Wire1.setSCL(7); 
  
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

//EXPERIMENTAL CASES
float L1 = 15, o1 = 3, L2 = 8, o2 = 5, L3 = 25, o3 = 4;

//COST FUNCTION PARAMETERS
float c1 = 1, c2 = 1,  c3 = 1;

//SYSTEM CALIBRATION PARAMETERS
float k11 = 2, k12 = 0.5, k13 = 0.5, k21 = 0.5, k22 = 2, k23 = 0.5, k31 = 0.5, k32 = 0.5, k33 = 2;

Vector3f K1(k11, k12, k13); 
Vector3f K2(k21, k22, k23); 
Vector3f K3(k31, k32, k33); 

void loop() {
  //wake up
  cons node1(rho, max_iter, 0);
  cons node2(rho, max_iter, 1);
  cons node3(rho, max_iter, 2);
  //calibrate

  node1.K = K1; 
  node1.norm = pow(node1.K.norm(),2);
  node1.sqr_diff = node1.norm - pow(node1.K(node1.idx),2);
  node1.c(node1.idx) = c1;
  node1.o = o1;
  node1.L = L1;

  node2.K = K2; 
  node2.norm = pow(node2.K.norm(),2);
  node2.sqr_diff = node2.norm - pow(node2.K(node2.idx),2);
  node2.c(node2.idx) = c2;
  node2.o = o2;
  node2.L = L2;

  node3.K = K3; 
  node3.norm = pow(node3.K.norm(),2);
  node3.sqr_diff = node3.norm - pow(node3.K(node3.idx),2);
  node3.c(node3.idx) = c3;
  node3.o = o3;
  node3.L = L3;

  for(int i = 1; i < max_iter; i++){
    //compute primal solutions
    node1.iter_cons(node1.L, node1.o, node1.c, node1.K);
    node1.d = node1.my_d_best;

    node2.iter_cons(node2.L, node2.o, node2.c, node2.K);
    node2.d = node2.my_d_best;

    node3.iter_cons(node3.L, node3.o, node3.c, node3.K);
    node3.d = node3.my_d_best;

    node1.d_av = (node1.d + node2.d + node3.d)/3;
    node2.d_av = (node1.d + node2.d + node3.d)/3;
    node3.d_av = (node1.d + node2.d + node3.d)/3;

    //COMPUTATION OF THE LAGRANGIAN UPDATES
    node1.y = node1.y + rho*(node1.d - node1.d_av);
    node2.y = node2.y + rho*(node2.d - node2.d_av);
    node3.y = node3.y + rho*(node3.d - node3.d_av);
  }

  float l1 = node1.K.transpose()*node1.d + node1.o, l2 = node2.K.transpose()*node2.d + node2.o, l3 = node3.K.transpose()*node3.d + node3.o;

  Serial.printf("Solutions: d = %f %f %f ; l = %f %f %f\n", node1.d_av[0], node1.d_av[1], node1.d_av[2], l1, l2, l3);
  delay(1000);

}
