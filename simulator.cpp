#include "simulator.h"
#include <math.h> 

sim::sim(float _Vcc, float _Rdivider){
  Vcc = _Vcc; Rdivider = _Rdivider;
}

void sim::sim_init(float _m, float _b){
  m = _m; b = _b;
}

//calculate tau
void sim::dim_change(float _s_gain, float _vi, float _xf, float _ti){
  s_gain = _s_gain; vi = _vi; xf = _xf; ti = _ti;
  
  vf = X_to_staticV(xf);

  bool increasing;
  if(vf > vi) increasing = true;
  else increasing = false;
  
  tau = X_to_tau(xf, increasing);

}

//calculate voltage
float sim::get_vt(float t){
  return vf - (vf - vi) * exp(-(t - ti)/tau);
}

inline float sim::LUX_to_LDR(float LUX){
  return pow(10, m*log10(LUX)+b);
}

float sim::X_to_staticV(float X){
  return Vcc * Rdivider /(sim::LUX_to_LDR(X) + Rdivider);
}

inline float sim::X_to_tau(float X, bool increasing){
  if(increasing) return -4*pow(10, -5)*pow(X,3)+ 0.0026* pow(X,2) - 0.0626 * X + 0.6796;
  else return -0.00004*pow(X,2) + 0.00086*X + 0.02384;
}
