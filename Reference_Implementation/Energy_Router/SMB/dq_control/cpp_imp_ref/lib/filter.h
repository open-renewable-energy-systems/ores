#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <map>
#include <vector>
#include <array>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <assert.h>     /* assert */
#include <fstream>
#include <queue>

# define PI 3.141592653589 

float digit_filter(float est1, float est2, float ratio)
{
   est1 = ratio * est1 + (1-ratio)*est2;
}

/// @brief  low pass filter ,referent to https://en.wikipedia.org/wiki/Low-pass_filter
/// https://blog.csdn.net/weixin_45024950/article/details/126734025
/// Laplace notation: y(s) = k * x(s)/(1 + tao *s);
struct Config_low_pass_filter
{
    uint8_t mode = 1; // 0: 脉冲法 1: 欧拉法  2: 双线性变化  https://blog.csdn.net/weixin_45024950/article/details/126734025
    float tao    = 0.5/(PI*50); // tao = RC
    float k           = 1.0;
    float T           = 1.0;
    float omega_cutoff = 0.0; // omega_c = 1/tao, cut off frequency

    void update_omega_cutoff()
    {
        omega_cutoff = 1.0/tao;
    };

    void update_tao()
    {
        tao = 1.0/omega_cutoff;
    };
};

struct Low_pass_filter
{
   Low_pass_filter(){};
   Low_pass_filter(Config_low_pass_filter & config) 
   {
      if(config.mode == 0) // 脉冲法
      {
        a0 = config.T*config.omega_cutoff;
        a1 = 0.0;
        b1 = exp(-config.T*config.omega_cutoff);
      }
      else if(config.mode == 1) // 欧拉法
      {
        a0 = config.T*config.omega_cutoff/(1.0+config.T*config.omega_cutoff);
        a1 = 0.0;
        b1 = 1.0/(1.0+config.T*config.omega_cutoff);
      }
      else if(config.mode==2) // 双线性变化
      {
        a0 = config.omega_cutoff*config.T/(config.omega_cutoff*config.T+2.0);
        a1 = config.omega_cutoff*config.T/(config.omega_cutoff*config.T+2.0);
        b1 = -(config.omega_cutoff*config.T-2.0)/(config.omega_cutoff*config.T+2.0);
      }
   };

   float update(float xn)
   {
      float yn = k * ( a0 * xn + a0*x_n_1 + b1 * y_n_1); 
      x_n_1 = xn;
      y_n_1 = yn;
      return yn;
   }
   float k = 1.0;
   float a0, a1, b1 = 0.0; // y(n) =  k * (a0*x(n) + a1*x(n-1) + b1*y(n-1))
   float x_n_1, y_n_1 = 0.0;// x_n_1 = x(n-1), y_n_1= y(n-1)
};
