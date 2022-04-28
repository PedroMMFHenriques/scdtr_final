#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <Arduino.h>
class sim {
  private:
    float m, b;
    float Vcc, Rdivider;
    
    float ti;
    float xf;
    float vi, vf;
    float s_gain;
    float tau;
    
    float LUX_to_LDR(float X);
    float X_to_staticV(float X);
    float X_to_tau(float X, bool increasing);
    
  public:  
    sim(float Vcc = 3.3, float Rdivider = 10000);
    void sim_init(float m, float b);
    void dim_change(float s_gain, float vi, float xf, float ti = 0);
    float get_vt(float t);
};

#endif
