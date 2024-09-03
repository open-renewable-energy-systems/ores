#ifndef CLARKE_H_
#define CLARKE_H_

#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <array>
#include <cstdlib>
#include <iostream>
// #include <assert.h> 
#include "../filter/low_pass_filter.h"
#include "../filter/handy_filter.h"
#include "../utilities/cqueue.h"

# define PI 3.141592653589 

/// @brief alpha-beta 变换，适合高频采用场景
   //  lpf1(config),
   //  lpf2(config)
struct Clarke_transform_1phase
{
    Config_low_pass_filter config;
    Low_pass_filter lpf1;
    Low_pass_filter lpf2;
    Clarke_transform_1phase(float ts, const float fc=50)
    {
      config.setTao(1.0/(2.0*PI*fc));
      config.setT(ts);
      lpf1 = Low_pass_filter(config);
      lpf2 = Low_pass_filter(config);
    };

    void transform(float curr_val, float & alpha, float & beta)
   {
      alpha =  curr_val; // sin(angle)
      beta  =  2.0*lpf2.update(lpf1.update(curr_val)); // sin(angle-pi/2) = -cos(angle)
   }
};

/// @brief alpha-beta变换，三角函数变换， 采样>=0.8kHz， 假定均匀采样
struct Clarke_transform_1phase_complex
{
   cqueue cque;
   int    dsize = 2; // data size
   Clarke_transform_1phase_complex(int dsize):
   dsize(dsize),
   cque(dsize){}; 

   bool transform( float curr_val,       // 当前测量值
                   float curr_angle,     // 当前测量相位,对于0相位, 假定已经经过延时补偿
                   float & alpha,        // 返回alpha值
                   float & beta,         // 返回beta值
                   float & return_angle, // 返回角度
                   const float   oneStep_angle // 均匀采样相位步长
                   )
   {
      float delt_angle = float(dsize-1) * oneStep_angle ;
      cque.push(curr_val);
      if(!cque.is_full) return false;
      // A * [sin(b) + sin(a)] =  2Acos[(a+b)/2] * cos([(b-a)/2.0])
      // A * [sin(b) - sin(a)] =  2Asin[(a+b)/2] * sin([(b-a)/2.0])
      alpha    =    ( cque.back() + cque.front() ) / cos(delt_angle/2.0) / 2.0; //  sin[(a+b)/2]
      beta     =   -( cque.back() - cque.front() ) / sin(delt_angle/2.0) / 2.0; //  -cos[(a+b)/2]
      return_angle   = curr_angle - delt_angle/2.0; // b - (b-a)/2 =(a+b)/2 
      return true;
   };

   ~Clarke_transform_1phase_complex(){}; //释放内存

   void resetDataBuffer()
   {
      cque.reset();
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

#endif