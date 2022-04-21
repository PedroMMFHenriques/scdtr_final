#ifndef CONSENSUS_H
#define CONSENSUS_H

#include <Arduino.h>
#include <ArduinoEigenDense.h>

using Eigen::Vector3f;

class cons {
  private:
    int idx;
    float rho, max_iter;

    float L, o, cost; 
    Vector3f K; //ki1, ki2, ki3

    Vector3f d, d_av, y, c;
    float norm, sqr_diff;

    auto iter_cons();
    bool check_feasibility(Vector3f _d);
    float evaluate_cost(Vector3f _d);

  public:  
    cons(float rho = 1, float max_iter = 5, int idx = 0);
    auto calc_cons(float L, float o, float cost, Vector3f K); 
};

#endif
