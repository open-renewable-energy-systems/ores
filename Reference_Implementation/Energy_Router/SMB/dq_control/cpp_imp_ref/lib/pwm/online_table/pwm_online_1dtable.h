

#ifndef PWM_ONLINE_1D_2H_
#define PWM_ONLINE_1D_2H_
#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <array>
#include <cstdlib>
#include <iostream>
#include <assert.h>     /* assert */
#include <fstream>
#include "../pwm_io/pwm_io.h"
#include "../../utilities/mod.h"
#include "../../config/Config_spwm_solver.h"
#include "pwm_on_off.h"

#ifndef PI 
#define PI 3.14159262
#endif

#define TAB_SIZE 3
#define ORDERS          [1,    3,      5] // 1st, 3rd, 5th order
#define MAX_PWM_PNTS    16 
#define EPS 0.000000001
#define PHASE_EPS 0.000000001


void generate_1d_table(float m[TAB_SIZE], float p[TAB_SIZE], float phase_shift, 
                       float & table_on[MAX_PWM_PNTS], 
                       float & table_off[MAX_PWM_PNTS],
                       ModulationParam md  = ModulationParam(),
                       Config_spwm_solver config = Config_spwm_solver()
                       )
y76
{
    float d_phase = 2.0*PI/MAX_PWM_PNTS;
    for(int i = 0; i<MAX_PWM_PNTS; i++)
    {
       float ref_phase = phase_shift + d_phase*i;
       table_on[i]     = pwm_with_harmonic(m, p ORDERS, TAB_SIZE, true,  ref_phase, md, config);
       table_off[i]    = pwm_with_harmonic(m, p ORDERS, TAB_SIZE, false, ref_phase, md, config);
    }
}

void combine_on_off(
                    float & table_on[MAX_PWM_PNTS], 
                    float & table_off[MAX_PWM_PNTS], 
                    float & time_table_time[2*MAX_PWM_PNTS+2],
                    float & time_table_on_off[2*MAX_PWM_PNTS+2],
                    float & comb_index [4*MAX_PWM_PNTS+2], 
                    int & table_on_off [4*MAX_PWM_PNTS+2],
                    int & table_ref_id [4*MAX_PWM_PNTS+2]
                    )
{

     float d_phase = 2.0*PI/MAX_PWM_PNTS;

    for(int i = 0; i <=MAX_PWM_PNTS; i++)
    {   
        
        talbe_on_off[4*i+1] = 0.0;
        talbe_on_off[4*i+2] = 1.0;
        talbe_on_off[4*i+3] = 1.0;
        talbe_on_off[4*i+4] = 0.0;

        comb_index[4*i+1] = table_on[i];
        comb_index[4*i+2] = table_on[i]  + PHASE_EPS;
        comb_index[4*i+3] = table_off[i];
        comb_index[4*i+4] = table_off[i] + PHASE_EPS;
        if(comb_index[4*i+1] < comb_index[4*i])
        {
             talbe_on_off[4*i+1] = 1.0;
             comb_index[4*i+1] = comb_index[4*i] + PHASE_EPS;
        } 
        time_table_time[2*i+1] = table_on[i];
        time_table_time[2*i+2] = table_off[i];
        time_table_on_off[2*i+1] = 1;
        time_table_on_off[2*i+2] = 0;
    }

    talbe_on_off[4*MAX_PWM_PNTS+1] = 0.0;
    talbe_on_off[0]                = 0.0; 
    time_table_time[2*MAX_PWM_PNTS+1] = 2*PI ;
    time_table_time[0]                = 0;
    time_table_on_off[0]               = 0;

}



#endif