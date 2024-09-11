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
   
   Clarke_transform_1phase clk(config1.T, 50); // 基于滤波的 Clakke 变换
   Clarke_transform_1phase_complex clk2(2);    // 基于三角函数变换的Clarke变换
   Park_1phase_w_handy_filter prk(config2.T); // 带低通滤波的park变换
   // 带IIR滤波器的park变换
   // Park_1phase_w_handy_filter prk(Notch_Fc50Hz_Fs1k_BW40Hz_3x, Notch_Fc50Hz_Fs1k_BW40Hz_3x); //float T, float hz = 50.0, uint8_t mode = 1, float k = 1.0
   
   std::ofstream file("./data.txt");
   for(int i = 0; i<len; i++)
   {
      float alpha, beta = 0.0;
      float return_angle = 0.0; // 如果使用三角函数变换，需要返回一个新的相位
      bool filter = false;
      float d, q, m = 0.0; // m -幅值
      float oneStep_angle = abs(config1.T * omega);
      
      /////////alpha-beta transformation
      clk2.transform(current[i], wt[i], alpha, beta,  return_angle, oneStep_angle);
      // clk.transform(current[i], alpha, beta);
      //////////////

      Alpha.push_back(alpha);
      Beta.push_back(beta);

      ////////////alpah-beta to dq transformation
      // Park_1phase::transform(alpha, beta, d, q, m, return_angle); // direct func call , no filter
      prk.transform(alpha, beta, d, q, m, return_angle); // with handy filter
      ///////////

      file << t[i]*1000 <<", " << sin(return_angle) << ", " << cos(return_angle) 
                        <<", " << alpha << ", " << beta << ", " << sqrt(alpha*alpha + beta*beta)
                        <<", " << d     << ", " << q    << "," << m << ", "  << std::endl;
   };
   
   file.close();

   std::cout << "very nice" << std::endl;

    return 0;
};