

#ifndef PWM_ONLINE_2D_2H_
#define PWM_ONLINE_2D_2H_
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

#define M3_PONTS     5
#define P3_PONTS     5
#define M3_UPBOUND   0.2
#define M3_LOWBOUND  0.0
#define P3_UPBOUND    0.5
#define P3_LOWBOUND  -0.5

void generate_3d_table(float m[TAB_SIZE], float p[TAB_SIZE], float phase_shift, 
                       float & table_on[M3_PONTS][P3_PONTS][MAX_PWM_PNTS], 
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