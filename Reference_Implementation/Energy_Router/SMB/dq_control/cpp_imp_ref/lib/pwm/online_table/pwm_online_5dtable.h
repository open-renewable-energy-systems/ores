

#ifndef PWM_ONLINE_5D_2H_
#define PWM_ONLINE_5D_2H_
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
#include "pwm_online_2dtable.h"
#include "pwm_online_3dtable.h"
#include "pwm_online_4dtable.h"

#ifndef PI 
#define PI 3.14159262
#endif

#define M5_PONTS     5
#define M5_UPBOUND   0.2
#define M5_LOWBOUND  0.0

void generate_5d_table(float m[TAB_SIZE], float p[TAB_SIZE], float phase_shift, 
                       float & table_on[M5_PONTS][P3_PONTS][M3_PONTS][M1_PONTS][MAX_PWM_PNTS], 
                       float & table_off[M5_PONTS][P3_PONTS][M3_PONTS][M1_PONTS][MAX_PWM_PNTS],
                       ModulationParam md  = ModulationParam(),
                       Config_spwm_solver config = Config_spwm_solver()
                       )

{
    float d_m        =  (M5_UPBOUND - M5_LOWBOUND)/(M5_PONTS-1);
    for(int j = 0; j<M1_PONTS; j++)
    {
        m[2] = d_m * j + M5_LOWBOUND;
        generate_2d_table(m, p, phase_shift, table_on[j, :, :, :, :], table_off[j, :, :, :, :], md, config)
    }
}

#endif