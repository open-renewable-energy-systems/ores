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
#include "../lib/utilities/cqueue.h"
#include "../lib/transform/clarke.h"
#include "../lib/transform/park.h"
#include "../lib/filter/low_pass_filter.h"
# define PI 3.141592653589 

int main()
{
   
   std::vector<float> t;
   std::vector<float> wt;
   std::vector<float> current;
   std::vector<float> Alpha; 
   std::vector<float> Beta;

// prepare config for low pass filter
   Config_low_pass_filter config1;
   config1.mode = 0; // 0: 脉冲法 1: 欧拉法  2: 双线性变化  https://blog.csdn.net/weixin_45024950/article/details/126734025
   config1.tao  = 1.0/(2.0*PI*50.0); // tao = RC
   config1.k    = 1.0;
   config1.T    = 1.0/1000.0;
////////////////////////////////////////

// prepare config for low pass filter
   Config_low_pass_filter config2;
   config2.mode = 1; // 0: 脉冲法 1: 欧拉法  2: 双线性变化  https://blog.csdn.net/weixin_45024950/article/details/126734025
   config2.tao  = 1.0/(2.0*PI*5.0); // tao = RC
   config2.k    = 1.0;
   config2.T    = config1.T;
////////////////////////////////////////

   float dt = config1.T;
   float tmin = 0.0;
   float tmax = 1.0;
   int len = round((tmax-tmin)/dt)+1;
   float omega = 2.0*PI*50.0;

   for(int i = 0; i<len; i++)
   {
      t.push_back(float(i)*dt + tmin);
      wt.push_back(t.back() * omega);
      float I = sin(wt.back()) + 0.0*sin(5.0*wt.back());
      current.push_back(I);
   }
   
   Clarke_transform_1phase clk(config1.T, 50);

   Clarke_transform_1phase_complex clk2(2);

   // Park_1phase prk(config2, config2);
   Park_1phase_w_handy_filter prk(0, 10, config2.T);

   std::ofstream file("./data.txt");

   for(int i = 0; i<len; i++)
   {
      float alpha, beta = 0.0;
      float return_angle = 0.0;
      bool filter = false;
      float d, q, m = 0.0;
      float oneStep_angle = abs(config1.T * omega);
      clk2.transform(current[i], wt[i], alpha, beta,  return_angle, oneStep_angle);
      // clk.transform(current[i], alpha, beta);
      Alpha.push_back(alpha);
      Beta.push_back(beta);
      // prk.transform(alpha, beta, d, q, m, wt[i], filter);
      prk.transform(alpha, beta, d, q, m, return_angle);
      file << t[i]*1000 <<", " << sin(return_angle) << ", " << cos(return_angle) 
                        <<", " << alpha << ", " << beta << ", " << sqrt(alpha*alpha + beta*beta)
                        <<", " << d     << ", " << q    << "," << m << ", "  << std::endl;
   };
   
   file.close();

   std::cout << "very nice" << std::endl;

    return 0;
};