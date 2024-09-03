
#ifndef HAND_FILTER_H
#define HAND_FILTER_H

#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <map>
#include <vector>
#include <array>
#include <cstdlib>
#include <iostream>
#include <assert.h>     /* assert */
# include "low_pass_filter.h"
# include "iir_filter.h"

# define PI 3.141592653589 

// 陷波滤波器（Notch Filter）
float NF_2ndorder_Fn100_Fs1k_BW20_b[3] =  { 0.96897915136010271, -1.5678412012906751, 0.96897915136010271};
float NF_2ndorder_Fn100_Fs1k_BW20_a[3] =  {1.0, -1.5678412012906751,  0.93795830272020542};

float NF_2ndorder_Fn150_Fs1k_BW40_b[3] =  {0.93960029184251637, -1.1045663891894697, 0.93960029184251637};
float NF_2ndorder_Fn150_Fs1k_BW40_a[3] =  {1.0, -1.1045663891894697,  0.87920058368503273};

float NF_2ndorder_Fn250_Fs1k_BW80_b[3] =  { 0.88444720318969217, -0.00000000000000010831354364010853,   0.88444720318969217};
float NF_2ndorder_Fn250_Fs1k_BW80_a[3] =  {1.0, -0.00000000000000010831354364010853,  0.76889440637938433};

// reference: meta.ai & matlab fdatool
struct HandyFitler_lps_50Hz_90Delay
{
   Config_low_pass_filter config;
   Low_pass_filter lps1;
   Low_pass_filter lps2;
   HandyFitler_lps_50Hz_90Delay(float fs) 
   {
       config.mode = 0; // 0: 脉冲法 1: 欧拉法  2: 双线性变化  https://blog.csdn.net/weixin_45024950/article/details/126734025
       config.tao  = 1.0/(2.0*PI*50.0); // tao = RC
       config.k    = 1.0;
       config.T    = 1.0/fs;
       lps1 = Low_pass_filter(config);
       lps2 = Low_pass_filter(config);
   };
   float update(float val)
   {
    return 2.0*lps2.update(lps1.update(val));
   }
} ;

///  create 90 degree delay for 50Hz sinusoid wave: input: sin(xxx), output: - cos(xxx)
HandyFitler_lps_50Hz_90Delay lps_50Hz_Fs1k_90Delay  = HandyFitler_lps_50Hz_90Delay(1000.0);
HandyFitler_lps_50Hz_90Delay lps_50Hz_Fs10k_90Delay = HandyFitler_lps_50Hz_90Delay(10000.0);
HandyFitler_lps_50Hz_90Delay lps_50Hz_Fs20k_90Delay = HandyFitler_lps_50Hz_90Delay(20000.0);
HandyFitler_lps_50Hz_90Delay lps_50Hz_Fs50k_90Delay = HandyFitler_lps_50Hz_90Delay(50000.0);

/// Notch filter to filter out 50x Hz signal in 50Hz sinusoid wave
IIRFilter Notch_Fc50Hz_Fs1k_BW20Hz_2x(2, NF_2ndorder_Fn100_Fs1k_BW20_a, NF_2ndorder_Fn100_Fs1k_BW20_b);
IIRFilter Notch_Fc50Hz_Fs1k_BW40Hz_3x(2, NF_2ndorder_Fn150_Fs1k_BW40_a, NF_2ndorder_Fn150_Fs1k_BW40_b);
IIRFilter Notch_Fc50Hz_Fs1k_BW80Hz_5x(2, NF_2ndorder_Fn250_Fs1k_BW80_a, NF_2ndorder_Fn250_Fs1k_BW80_b);
#endif