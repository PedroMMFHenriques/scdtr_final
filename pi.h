#ifndef PI_H
#define PI_H

#include <Arduino.h>
class pi {
  private:
    float kp, ki;
    float ui_before;
    float time_step;
        
  public:  
    pi(float kp = 1, float ki = 0, float time_step = 0.01);
    float calc_pwm(float e, bool anti_windup = true, float ff_PWM = 0);
    float calc_pwm(float r, float y, bool anti_windup = true, float ff_PWM = 0);
};

#endif
