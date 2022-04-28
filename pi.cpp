#include "pi.h"

pi::pi(float _kp, float _ki, float _time_step){
  ui_before = 0;
  kp = _kp; ki = _ki; time_step = _time_step;
}

//calculate feedback component of u
float pi::calc_pwm(float e, bool anti_windup, float ff_PWM){
  float up = kp * e;
  float ui = ui_before + ki * time_step * e;
  float fb_PWM = up + ui;

  if(anti_windup){
    if(fb_PWM + ff_PWM > 1) ui = 1 - ff_PWM - up;
    else if(fb_PWM + ff_PWM < 0) ui = 0 - ff_PWM - up;
    fb_PWM = up + ui;
  }

  ui_before = ui;
  return fb_PWM;
}

float pi::calc_pwm(float r, float y, bool anti_windup, float ff_PWM){
  return calc_pwm(r-y, anti_windup, ff_PWM);
}
