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
#include "filter.h"
#include "base_lib.h"

# define PI 3.141592653589 

/// @brief alpha-beta 变换，适合高频采用场景
struct Clarke_transform_1phase
{
    Config_low_pass_filter config;
    Low_pass_filter lpf1;
    Low_pass_filter lpf2;
    Clarke_transform_1phase(Config_low_pass_filter & config):
    lpf1(config),
    lpf2(config){};

   void transforms(float curr_val, float & alpha, float & beta)
   {
      alpha = curr_val;
      beta = 2.0*lpf2.update(lpf1.update(curr_val)); // apply LPF twice
   }
};

/// @brief alpha-beta变换，三角函数变换， 高低频均可， 假定均匀采样
struct Clarke_transform_1phase_complex
{
   Buffer data_buffer;
   int    dsize = 1; // data size
   Clarke_transform_1phase_complex(int dsize):
   data_buffer(dsize){}; 

 
   // A * [sin(b) + sin(a)] = 2Asin[(a+b)/2] * cos([(b-a)/2.0])
   // A * [sin(b) - sin(a)] = 2Acos[(a+b)/2] * sin([(b-a)/2.0])
   bool transform( float curr_val,       // 当前测量值
                   float curr_angle,     // 当前测量相位,对于0相位, 假定已经经过延时补偿
                   float & alpha,        // 返回alpha值
                   float & beta,         // 返回beta值
                   float & return_angle, // 返回角度
                   float   oneStep_angle // 均匀采样相位步长
                   )
   {
      float m2, m1 = 0.0;
      float delt_angle = float(dsize-1) * oneStep_angle ;
      data_buffer.push(curr_val);
      if(!data_buffer.is_full) return false;
      m2 = data_buffer.back();
      m1 = data_buffer.front();
      float temp1  = cos(delt_angle/2.0);
      float temp2  = sin(delt_angle/2.0);
      float alpha    = (m2+m1) / temp1 / 2.0;
      float beta     = (m2-m1) / temp2 / 2.0;
      return_angle   = curr_angle - delt_angle/2.0;
   }
   ~Clarke_transform_1phase_complex(){};
   void resetDataBuffer()
   {
      data_buffer.reset();
   };
};

struct Clarke_transform_3phase
{
    void transform(float a, float b, float c,float & alpha, float & beta, float k = 0.66666667)
   {
      alpha = ( a - 0.5*b - 0.5*c) * k;
      beta  = ( 0.866025*b - 0.866025*c) *k;
   } 
};