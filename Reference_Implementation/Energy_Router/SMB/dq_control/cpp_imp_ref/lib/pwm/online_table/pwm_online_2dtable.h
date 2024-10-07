

#ifndef PWM_ONLINE_2H_
#define PWM_ONLINE_2H_
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
#include "pwm_on_off.h"
#include "pwm_online_1dtable.h"

#ifndef PI 
#define PI 3.14159262
#endif

#define M1_PONTS    11
#define M1_UPBOUND  1.0
#define M1_LOWBOUND 0.2

void generate_2d_table(float m[TAB_SIZE], float p[TAB_SIZE], float phase_shift, 
                       float & table_on[M1_PONTS][MAX_PWM_PNTS], 
                       float & table_off[M1_PONTS][MAX_PWM_PNTS],
                       ModulationParam md  = ModulationParam(),
                       Config_spwm_solver config = Config_spwm_solver()
                       )

{
    float d_m        =  (M1_UPBOUND - M1_LOWBOUND)/(M1_PONTS-1);
    for(int j = 0; j<M1_PONTS; j++)
    {
        m[0] = d_m * j + M1_LOWBOUND;
        generate_1d_table(m, p, phase_shift, table_on[j, :], table_off[j, :], md, config)
    }

}

#endif

// // #define M_UPBOUND       [1.0,  0.1,  0.3] // modulation up bound 
// // #define M_LOWBOUND      [0.2,  0.0,  0.0] // modulation low bound
// #define M_NUM_POINTS    [11,    2,   5] // num of intervals+1

// // #define PHASE_UPBOUND   [0.,    0.5,  0.5] // modulation phase shift upbound 
// // #define PHASE_LOWBOUND  [0.,   -0.5, -0.5] // modulation phase shift lowbound 
// // #define PHASE_NUM_POINTS[1,     11,   11]  // 

// // #define MAX_INTV_PNTS      11
// #define MAX_PWM_PNTS       17    //
// #define EPS 0.000000001