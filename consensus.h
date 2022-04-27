#ifndef CONSENSUS_H
#define CONSENSUS_H

#include <Arduino.h>
#include <ArduinoEigenDense.h>

using Eigen::Vector3f;

class cons {
  private:
    int idx;
    float rho, max_iter;

    float L, o; 
    Vector3f K; //ki1, ki2, ki3

    Vector3f d, d_av, y, c;
    float n, sqr_diff;

    bool check_feasibility(Vector3f _d);
    float evaluate_cost(Vector3f _d);

    Vector3f my_d_best;
    float my_cost_best;
  
  public:
    cons(float rho = 1, float max_iter = 5);
    void init_cons(int idx, float o, float cost, Vector3f K);
    void iter_cons(); 
    void new_ref(float L);
    void update_cons(Vector3f _d_av);
    void get_sol_results();
};

void start_consensus();

void cons_start_msg_handler();
void cons_sol_msg_handler(uint8_t id, Vector3f d_node);


#endif
