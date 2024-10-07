

#ifndef PWM_ONLINE_4D_2H_
#define PWM_ONLINE_4D_2H_
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
#include "pwm_online_1dtable.h"
#include "pwm_online_2dtable.h"
#include "pwm_online_3dtable.h"

#ifndef PI 
#define PI 3.14159262
#endif

#define P3_PONTS     5
#define P3_UPBOUND    0.2
#define P3_LOWBOUND  -0.2

void generate_4d_table(float m[TAB_SIZE], float p[TAB_SIZE], float phase_shift, 
                       float & table_on[P3_PONTS][M3_PONTS][P3_PONTS][MAX_PWM_PNTS], 
                       float & table_off[P3_PONTS][M1_PONTS][MAX_PWM_PNTS],
                       ModulationParam md  = ModulationParam(),
                       Config_spwm_solver config = Config_spwm_solver()
                       )

{
    float d_m        =  (P3_UPBOUND - P3_LOWBOUND)/(P3_PONTS-1);
    for(int j = 0; j<P3_PONTS; j++)
    {
        p[1] = d_p * j + P3_LOWBOUND;
        generate_3d_table(m, p, phase_shift, table_on[j, :, :, :], table_off[j, :, :, :], md, config)
    }
}

#endif