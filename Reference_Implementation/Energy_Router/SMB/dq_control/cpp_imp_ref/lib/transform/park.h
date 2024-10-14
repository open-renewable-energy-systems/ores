
#ifndef APRKE_H_
#define APRKE_H_

#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <vector>
//#include <array>
#include <cstdlib>
#include <iostream>
#include <assert.h>     /* assert */
//#include "filter.h"
#include <stdexcept>
#include <exception>

# define PI 3.141592653589f

#include "../../test/cfg_main.h"

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
        m = sqrt(q * q + d * d);
        #ifdef COMPILE_MCU_CPP
        #else
        std::cout << "inside" << d << ", " << q << ", " << m << std::endl;
        #endif
    }
};

#define FILTER_TYPE_LPS 0
#define FILTER_TYPE_IIR 1

struct Park_1phase_w_handy_filter
{
    Low_pass_filter  lps_d;
    Low_pass_filter  lps_q;
    IIRFilter        iir_filter_d;
    IIRFilter        iir_filter_q;
    int              type = 0;

    Park_1phase_w_handy_filter(float T, float hz = 50.0f, uint8_t mode = 1, float k = 1.0f)// use lps filter, T -sampling time, hz - cutoff freq
        : lps_d(T, 1.0f / (2.0f * PI * hz), k, mode)
        , lps_q(T, 1.0f / (2.0f * PI * hz), k, mode)
        , iir_filter_d(2) // default to 2nd order
        , iir_filter_q(2) //default to 2nd order
        , type(FILTER_TYPE_LPS)
    {};


    Park_1phase_w_handy_filter(int order, float ts, float *a, float *b)// IIR filter init - method 1
        : lps_d()
        , lps_q()
        , iir_filter_d(order, a, b)
        , iir_filter_q(order, a, b)
        , type(FILTER_TYPE_IIR)
    {};


    Park_1phase_w_handy_filter(IIRFilter d_filter, IIRFilter q_filter) // IIR filter init - method2
        : lps_d()
        , lps_q()
        , iir_filter_d(d_filter)
        , iir_filter_q(q_filter)
        , type(FILTER_TYPE_IIR)
    {};

    void transform(float alpha, float beta, float &d, float &q, float & m,
                   float wt)
    {
        Park_1phase::transform(alpha, beta, d, q, m, wt);
        if(type == FILTER_TYPE_LPS)
        {
            d = lps_d.update(d);
            q = lps_q.update(q);
            m = sqrt(d * d + q * q);
        }
        else
        {
            iir_filter_d.filter(d);
            iir_filter_q.filter(q);
            m  = sqrt(d * d + q * q);
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


