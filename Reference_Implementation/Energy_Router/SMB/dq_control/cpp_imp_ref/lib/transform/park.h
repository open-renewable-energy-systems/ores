
#ifndef APRKE_H_
#define APRKE_H_

#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <vector>
#include <array>
#include <cstdlib>
#include <iostream>
#include <assert.h>     /* assert */
#include "filter.h"

# define PI 3.141592653589 


// ref: https://blog.csdn.net/qq_38847810/article/details/120607813
// transform [alpha, beta] to [d, q]] through matrix 
// siw(wt), -cos(wt)
// cos(wt), sin(wt)

/// @brief park transformation
struct Park_1phase
{
    void static transform(float alpha, float beta, float &d, float &q, float & m,
                   float wt)
    {
        d =  sin(wt) * alpha - cos(wt) * beta;
        q =  cos(wt) * alpha + sin(wt) * beta;
        m = sqrt(q*q + d*d);
    }
};

#define FILTER_TYPE_LPS 0
#define FILTER_TYPE_NOTCH 1

struct Park_1phase_w_handy_filter
{
     Config_low_pass_filter config;
     Low_pass_filter * lps_d;
     Low_pass_filter * lps_q;
     IIRFilter * iir_filter_d;
     IIRFilter * iir_filter_q;
     int type = 0;

     ///// type - LPS_TYPE, IIR_TYPE
     ////  hz - cutoff freq for lps, notch freqc for notch converter
     ///  ts - sampling time
     Park_1phase_w_handy_filter(int type, float hz, float ts): 
     type(type)
     {
        if(type == FILTER_TYPE_LPS)
        {
          config.setT(ts);
          config.setTao(1.0/(2.0*2*PI*hz));
          * lps_d =   Low_pass_filter(config);
          * lps_q =   Low_pass_filter(config);
        }
        else if(type == FILTER_TYPE_NOTCH)
        {
           if(hz = 150) {
             * iir_filter_d = Notch_Fc50Hz_Fs1k_BW40Hz_3x;
             * iir_filter_q = Notch_Fc50Hz_Fs1k_BW40Hz_3x;
           }
           else if(hz = 250) {
              * iir_filter_d = Notch_Fc50Hz_Fs1k_BW80Hz_5x;
              * iir_filter_q = Notch_Fc50Hz_Fs1k_BW80Hz_5x;
           }
           else {
              * iir_filter_d = Notch_Fc50Hz_Fs1k_BW20Hz_2x;
              * iir_filter_q = Notch_Fc50Hz_Fs1k_BW20Hz_2x; 
            }
        }

        else {};
     };

     void transform(float alpha, float beta, float &d, float &q, float & m,
                   float wt)
        {
            Park_1phase::transform(alpha, beta, d, q, m, wt);
            if(type==FILTER_TYPE_LPS)
            {
              d = lps_d->update(d);
              q = lps_q->update(q);
              m = sqrt(d*d+q*q);
            }
            else
            {
              iir_filter_d->filter(d);
              iir_filter_q->filter(q);
              m  = sqrt(d * d + q*q);
            }
        };
};
  
#endif


    // Config_low_pass_filter config;
    // Low_pass_filter lpf_d;  // low pass filter for d
    // Low_pass_filter lpf_q;  // low pass filter for q
    // Park_1phase(Config_low_pass_filter & config1, 
    //             Config_low_pass_filter & config2 
    // )
    //   : lpf_d(config1)
    //   , lpf_q(config2)
    //   {}; 
    