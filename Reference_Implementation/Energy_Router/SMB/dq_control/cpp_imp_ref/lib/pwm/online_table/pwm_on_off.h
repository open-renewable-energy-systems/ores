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

#include "../pwm_io/pwm_io.h"
#include "../../utilities/mod.h"
#include "../../config/Config_spwm_solver.h"

inline float pwm_with_harmonic
                    (float * m,      // modulation coeff at different orders
                     float * phase,  // phase shift at different orders
                     float * order,  // orders
                     const int len,    // len of array of m, phase, order
                     bool    mode,    // 1 - pwm on, 0 pwn_off
                     float ref_phase, // ref phase 
                     const ModulationParam & mp, 
                     const Config_spwm_solver & config = Config_spwm_solver()) 
{
    float dx_last = 0.0;
    float dx      = 0.0;

    ref_phase = mod(ref_phase, 2*PI);
    float ref_phase_lk = mod(ref_phase, PI); // 保证在[0, pi]之间
    float sign = 1;
    if (mode) sign = -1.0
    else sign = 1.0;
    
    // 使用定点方法(fixed point iteration)迭代, 对比牛顿法，定点法在m/k小时，迭代速度快
    for(int i = 1; i < int(config.max_iter); i++) 
    {
       float k =  mp.get_positive_k();
       float theta = 0;
       for(int i = 0; i<len; i++)
       {
          theta = ref_phase_lk + dx;
          dx = sign * m[i]/k *sin(order[i] * theta + phase[i]);
       }
       if (abs(dx-dx_last) < config.err) break;
       dx_last = dx;
    }
    return ref_phase + dx;
};