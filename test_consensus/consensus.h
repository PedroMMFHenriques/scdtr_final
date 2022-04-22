#ifndef CONSENSUS_H
#define CONSENSUS_H

#include <Arduino.h>
#include <ArduinoEigenDense.h>

using Eigen::Vector3f;

class cons {
  public:
    int idx;
    float rho, max_iter;

    float L, o, cost; 
    Vector3f K; //ki1, ki2, ki3

    Vector3f d, d_av, y, c;
    float norm, sqr_diff;

    bool check_feasibility(Vector3f _d);
    float evaluate_cost(Vector3f _d);

    Vector3f my_d_best;
    float my_cost_best;
    
    cons(float rho = 1, float max_iter = 5, int idx = 0);
    void iter_cons(float L, float o, Vector3f c, Vector3f K); 
};

#endif
