#include <stdint.h>

#include "consensus.h"
#include "gvars.h"
#include "protocol.h"
#include "comms.h"
#include "settings.h"

using Eigen::Vector3f;

cons::cons(float _rho, float _max_iter){
  rho = _rho; max_iter = _max_iter;
  d = Vector3f::Zero();
  d_av = Vector3f::Zero();
  y = Vector3f::Zero();
  c = Vector3f::Zero();
  n = 0; sqr_diff = 0;
}


void cons::init_cons(int _idx, float _o, float _cost, Vector3f _K){
  idx = _idx;
  o = _o;
  c(idx) = _cost;

  K = _K;
  n = pow(K.norm(),2);
  sqr_diff = n - pow(K(idx),2);

}


void cons::new_ref(float _L){
  node.L = _L;
}


void cons::iter_cons() {
  //iteration initialization
  Vector3f d_best(-1,-1,-1); 
  float cost_best = 1000000;
  bool sol_unconstrained = true, sol_boundary_linear = true, sol_boundary_0 = true, sol_boundary_100 = true, sol_linear_0 = true, sol_linear_100 = true; 
  
  Vector3f z = rho*d_av - y - c;

  //compute unconstrained minimum
  Vector3f d_u = (1/rho)*z;
  sol_unconstrained = check_feasibility(d_u);
  if(sol_unconstrained){
    //IF UNCONSTRAINED SOLUTION EXISTS, THEN IT IS OPTIMAL
    //NO NEED TO COMPUTE THE OTHER
    float cost_unconstrained = evaluate_cost(d_u);
    if(cost_unconstrained < cost_best){
      d_best = d_u;
      cost_best = cost_unconstrained;
    }
  }
  else{
    //compute minimum constrained to linear boundary   
    Vector3f d_bl = (1/rho)*z - K/n*(o - L + (1/rho)*z.transpose()*K);
    //check feasibility of minimum constrained to linear boundary
    sol_boundary_linear = check_feasibility(d_bl);
    //compute cost and if best store new optimum
    if(sol_boundary_linear){
      float cost_boundary_linear = evaluate_cost(d_bl);
      if(cost_boundary_linear < cost_best){
          d_best = d_bl;
          cost_best = cost_boundary_linear;
      }
    }

    //compute minimum constrained to 0 boundary
    Vector3f d_b0 = (1/rho)*z;
    d_b0(idx) = 0;
    //check feasibility of minimum constrained to 0 boundary
    sol_boundary_0 = check_feasibility(d_b0);
    //compute cost and if best store new optimum
    if(sol_boundary_0){ 
      float cost_boundary_0 = evaluate_cost(d_b0);
      if(cost_boundary_0 < cost_best){
          d_best = d_b0;
          cost_best = cost_boundary_0;
      }
    }

    //compute minimum constrained to 100 boundary
    Vector3f d_b1 = (1/rho)*z;
    d_b1(idx) = 100;
    //check feasibility of minimum constrained to 100 boundary
    sol_boundary_100 = check_feasibility(d_b1);
    //compute cost and if best store new optimum
    if(sol_boundary_100){
      float cost_boundary_100 = evaluate_cost(d_b1);
      if(cost_boundary_100 < cost_best){
          d_best = d_b1;
          cost_best = cost_boundary_100;
      }
    }

    //compute minimum constrained to linear and 0 boundary
    Vector3f d_l0 = (1/rho)*z - (1/sqr_diff)*K*(o - L) + (1/rho/sqr_diff)*K*(K(idx)*z(idx) - z.transpose()*K);
    d_l0(idx) = 0;
    //check feasibility of minimum constrained to linear and 0 boundary
    sol_linear_0 = check_feasibility(d_l0);
    //compute cost and if best store new optimum
    if(sol_linear_0){ 
      float cost_linear_0 = evaluate_cost(d_l0);
      if(cost_linear_0 < cost_best){
          d_best = d_l0;
          cost_best = cost_linear_0;
      }
    }

    //compute minimum constrained to linear and 100 boundary
    Vector3f d_l1 = (1/rho)*z - (1/sqr_diff)*K*(o - L + 100*K(idx)) + (1/rho/sqr_diff)*K*(K(idx)*z(idx) - z.transpose()*K);
    d_l1(idx) = 100;
    //check feasibility of minimum constrained to linear and 100 boundary
    sol_linear_100 = check_feasibility(d_l1);
    ///compute cost and if best store new optimum
    if(sol_linear_100){
      float cost_linear_100 = evaluate_cost(d_l1);
      if(cost_linear_100 < cost_best){
          d_best = d_l1;
          cost_best = cost_linear_100;
      }
    }
  }

  my_d_best = d_best;
  my_cost_best = cost_best;
    
  // broadcast SOLUTION
  uint8_t msg[MSG_SIZE];
  uint8_t data[1];

  data[0] = my_d_best;

  int data_size = 1;

  int msg_size = msg_formatter(msg, MEASURE, data, data_size);

  i2c_send(BROADCAST, msg, msg_size);
}


bool cons::check_feasibility(Vector3f _d){
  float tol = 0.001; //tolerance for rounding errors

  if (_d(idx) < 0 - tol) return false; 
  if (_d(idx) > 100 + tol) return false; 
  if (_d.transpose()*K < L - o - tol) return false; 
  return true;
}


float cons::evaluate_cost(Vector3f _d){
  float y1 = y.transpose()*_d;
  float y2 = y.transpose()*d_av;
  return c.transpose()*_d + y1 - y2 + rho/2*pow((_d - d_av).norm(),2); 
}


void cons::update_cons(Vector3f _d_av){
  d_av = _d_av;
  y += rho*(d - d_av);  
}

void cons::get_sol_results(){
  //return resultados node.d_av[node.idx]  node.K.transpose()*node.d + node.o
}