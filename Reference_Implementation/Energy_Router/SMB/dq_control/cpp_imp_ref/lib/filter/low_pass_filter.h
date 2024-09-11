#ifndef LOW_PASS_FILTER_H_
#define LOW_PASS_FILTER_H_

#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <map>
#include <vector>
#include <array>
#include <cstdlib>
#include <iostream>
#include <assert.h>     /* assert */
// reference: meta.ai & matlab fdatool
# define PI 3.141592653589 

/// @brief  low pass filter ,referent to https://en.wikipedia.org/wiki/Low-pass_filter
/// https://blog.csdn.net/weixin_45024950/article/details/126734025
/// Laplace notation: y(s) = k * x(s)/(1 + tao *s);

struct Config_low_pass_filter
{
    uint8_t mode = 1; // 0: 脉冲法 1: 欧拉法  2: 双线性变化  https://blog.csdn.net/weixin_45024950/article/details/126734025
    float tao    = 0.5/(PI*50.0); // tao = RC
    float k           = 1.0;
    float T           = 1.0;

     Config_low_pass_filter(){};
    /// @brief get cutoff freq based on time constant tao
    /// @return cutoff freq
    float getOmega_c()
    {
        return 1.0/tao;
    };
    
    /// @brief get coeff of filter = sample period * cutoff freq
    /// @return coeff of filter
    float getCoeff()
    {
       return T*getOmega_c();
    }

    /// @brief reset time constant tao based on cut of freq
    /// @param omega_c 
    void setTao(float omega_c)
    {
      tao = 1.0/omega_c;
    }

    void setT(float T_)
    {
        T=T_;
    } 

    void setK(float k_)
    {
        k = k_;
    }

    void setMode(uint8_t mode_)
    { 
      mode = mode_;

    }

};

struct Low_pass_filter
{
   Low_pass_filter(){}; // default constructor
   Low_pass_filter(float T, float tao = 1.0/(2*PI*50.0), float k = 1.0, uint8_t mode=1)
   {
     Config_low_pass_filter config;
     config.setT(T);
     config.setTao(tao);
     config.setK(k);
     config.setMode(mode);
     this->init(config.getCoeff(), config.getOmega_c(), config.mode);
     k = config.k;
   };

   Low_pass_filter(Config_low_pass_filter & config)
   {
      this->init(config.getCoeff(), config.getOmega_c(), config.mode);
      k = config.k;
   }
   
   void init(const float & coeff, const float & omega_c, const uint8_t & mode)
   {
      if(mode == 0) // 脉冲法
      {
        a0 = omega_c;
        a1 = 0.0;
        b1 = exp(-coeff);
      }
      else if(mode == 1) // 欧拉法
      {
        a0 = coeff/(1.0+coeff);
        a1 = 0.0;
        b1 = 1-a0;
      }
      else if(mode==2) // 双线性变化
      {
        a0 = coeff/(coeff+2.0);
        a1 = a0;
        b1 = -(coeff-2.0)/(coeff+2.0);
      }
   }
   float update(float xn)
   {
      float yn = k * digit_filter_3input(xn, xn_1, yn_1, a0, a1, b1); // float yn = k * ( a0 * xn + a1*x_n_1 + b1 * y_n_1); 
      xn_1 = xn;
      yn_1 = yn;
      return yn;
   }

   float digit_filter_3input(const float & est1, const float & est2, const float & est3,
                             const float & ratio1, const float & ratio2, const float & ratio3)
    {
         return  ratio1 * est1 + ratio2*est2 + ratio3 * est3;
    };

    struct Low_pass_filter operator=(const Low_pass_filter & filter)
   {
       this->k  = filter.k;
       this->a0 = filter.a0;
       this->a1 = filter.a1;
       this->b1 = filter.b1;
       this->xn_1 = xn_1;
       this->yn_1 = yn_1;
       return *this;
   }; // = operator （=运算符）

   float k = 1.0;
   float a0, a1, b1 = 0.0; // y(n) =  k * (a0*x(n) + a1*x(n-1) + b1*y(n-1))
   float xn_1, yn_1 = 0.0;// x_n_1 = x(n-1), y_n_1= y(n-1)
};
#endif

// int main()
// {
//    // prepare config for low pass filter
//    Config_low_pass_filter config1;
//    config1.mode = 0; // 0: 脉冲法 1: 欧拉法  2: 双线性变化  https://blog.csdn.net/weixin_45024950/article/details/126734025
//    config1.tao  = 1.0/(2.0*PI*50.0); // tao = RC
//    config1.k    = 1.0;
//    config1.T    = 1.0/1000.0;
// ////////////////////////////////////////
    //  Low_pass_filter lps(config1);
    // lps.update(1.0);
// }