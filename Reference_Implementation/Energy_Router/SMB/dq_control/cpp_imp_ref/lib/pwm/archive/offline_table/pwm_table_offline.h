#ifndef PWM_OFF_OFF_2H_
#define PWM_OFF_OFF_2H_
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

#ifndef PI 
#define PI 3.14159262
#endif

#define TAB_SIZE 3
#define ORDERS          [1,    3,      5] // 1st, 3rd, 5th order
#define M_UPBOUND       [1.0,  0.1,  0.3] // modulation up bound 
#define M_LOWBOUND      [0.2,  0.0,  0.0] // modulation low bound
#define M_NUM_POINTS    [11,    2,   5] // num of intervals+1

#define PHASE_UPBOUND   [0.,    0.5,  0.5] // modulation phase shift upbound 
#define PHASE_LOWBOUND  [0.,   -0.5, -0.5] // modulation phase shift lowbound 
#define PHASE_NUM_POINTS  [1,     11,   11]  // 

#define MAX_INTV_PNTS      11
#define MAX_PWM_PNTS       17    //
#define EPS 0.000000001

struct PWM_Offline_TableStruct
{
    float m_index[TAB_SIZE][MAX_INTV_PNTS]; // e.g. 0.2, 0.3 .. 1.0
    float p_index[TAB_SIZE][MAX_INTV_PNTS]; // -0.3, -0.2, .... 0.3
    float pwm_index[MAX_PWM_PNTS];

    float map_on[M_NUM_POINTS[0]][M_NUM_POINTS[1]][M_NUM_POINTS[2]][M_NUM_POINTS[0]][M_NUM_POINTS[1]][M_NUM_POINTS[2]][MAX_PWM_PNTS];
    float map_off[M_NUM_POINTS[0]][M_NUM_POINTS[1]][M_NUM_POINTS[2]][M_NUM_POINTS[0]][M_NUM_POINTS[1]][M_NUM_POINTS[2]][MAX_PWM_PNTS];

    int   m_index_at_t[TAB_SIZE];
    int   p_indx_at_t[TAB_SIZE];
    // int   pwm_ind_at_t

    float m_residual[TAB_SIZE];
    float p_residual[TAB_SIZE];
    float dm[TAB_SIZE];
    float dp[TAB_SIZE];

    vod PWM_Offline_TableStruct(int phase_shift) // init
    {
        for(int i = 0; i<TAB_SIZE; i++)
        {
            m_index[i][0] = M_LOWBOUND[i];
            p_index[i][0] = PHASE_LOWBOUND[i];
            float delta_m = 0.0;
            float delta_p = 0.0;

            if(M_NUM_POINTS[i]-1>=1)
                delta_m = (M_UPBOUND[i] - M_LOWBOUND[i])/(M_NUM_INTV[i]-1);

            if(PHASE_NUM_POINTS[i]-1>=1)
                delta_p = (M_UPBOUND[i] - M_LOWBOUND[i])/(PHASE_NUM_POINTS[i]-1);

            dm[i] = delta_m;
            dp[i] = delta_p;

            for(int j = 1; j<M_NUM_INTV[i]; j++)
            {
                if(M_NUM_POINTS[i]-1>=1)
                {
                    m_index[i][j] = m_index[i][0] + detla_m*j;
                }

                if(PHASE_NUM_POINTS[i]-1>=1)
                {
                    p_index[i][j] = p_index[i][0] + detla_p*j;
                }
            }
        }
        
        float d_phase = 2.0*PI/(MAX_PWM_PNTS-1);
        for(int i = 0; i<MAX_PWM_PNTS; i++)
        {
            pwm_index[i] = pwm_index[i] + d_phase*i + phase_shift;
        }

    }

    void lookup(float & m[TAB_SIZE], float & p[TAB_SIZE], float & on_phase, float & off_phase, )
    {
          for(int i = 0; i<TAB_SIZE; i++)
          {
            if(m[i] > M_UPBOUND[i]) m[i] = M_UPBOUND[i];
            if(m[i] < M_LOWBOUND[i]) m[i] = M_LOWBOUND[i];
            if(p[i] > PHASE_UPBOUND[i])   p[i] = PHASE_UPBOUND[i];
            if(p[i] < PHASE_LOWBOUND[i])  p[i] = PHASE_LOWBOUND[i];
            
            m_index_at_t[i] = floor( (m[i]-M_LOWBOUND[i])/(dm[i]+eps) );
            p_indx_at_t[i] = floor( (p[i]-PHASE_LOWBOUND[i])/(dm[i]+eps) );

            m_residual[i] = m[i] - (dm[i]*m_index_at_t[i] + M_LOWBOUND[i]);
            p_residual[i] = p[i] - (dp[i]*p_indx_at_t[i] + PHASE_LOWBOUND[i]);

          }
          on_phase  = map_on[p_indx_at_t[0]][m_index_at_t[1]][m_index_at_t[2]][p_indx_at_tx[0]][p_indx_at_tx[1]][p_indx_at_tx[2]];
          off_phase = map_off[p_indx_at_t[0]][m_index_at_t[1]][m_index_at_t[2]][p_indx_at_tx[0]][p_indx_at_tx[1]][p_indx_at_tx[2]];
          
          float index[6]    = {m_index_at_t[0], m_index_at_t[1], m_index_at_t[2], p_indx_at_t[0], p_indx_at_t[1], p_indx_at_t[2]};
          
          for(int i = 0; i<TAB_SIZE; i++ )
          {
            float index2[] = index;
            if(index2[2*i] +1  <= M_NUM_POINTS[i] -1) index2[2*i]++;
            on_phase += ( (map_on[index2[0]][index2[1]][index2[2]][index2[3]][index2[4]][index2[5]][index2[6]]) - on_phase)/(dm[i/2]+eps)    * m_residual[i];
            off_phase += ( (map_off[index2[0]][index2[1]][index2[2]][index2[3]][index2[4]][index2[5]][index2[6]]) - off_phase)/(dm[i/2]+eps) * m_residual[i];
          }

          
          for(int i = 0; i<TAB_SIZE; i++ )
          {
            float index2[] = index;
            if(index2[2*i+1] + 1 <= PHASE_NUM_POINTS[i]) index2[2*i+1]++;
            on_phase  += ( (map_on[index2[0]][index2[1]][index2[2]][index2[3]][index2[4]][index2[5]][index2[6]]) - on_phase)/(dm[i/2]+eps)    * p_residual[i];
            off_phase += ( (map_off[index2[0]][index2[1]][index2[2]][index2[3]][index2[4]][index2[5]][index2[6]]) - off_phase)/(dm[i/2]+eps)  * p_residual[i];
          }

    }
}


// struct PWM_Create_Table
// {
//   float fc    = 800.0;               // 调制频率
//   float fg    = 50.0;                // 电网频率，
//   PWM_high_dim_lookup table;

// //   PWM_Create_BaseTable(PWM_high_dim_lookup table):
// //   table(table);
 
// //   void create_table(PWM_high_dim_lookup & table)
// //   {

// //   }


// }

    // void create_table(const float fc, const float fg, float & phase_shift)
    // {
    //     for(int i1 = 0; i<M_NUM_POINTS[0]; i1++)
    //         for(int i2 = 0; i<M_NUM_POINTS[1]; i2++)
    //              for(int i3 = 0; i<M_NUM_POINTS[2]; i3++)
    //                 for(int i4 = 0; i<PHASE_NUM_POINTS[0]; i4++)
    //                     for(int i5 = 0; i<PHASE_NUM_POINTS[1]; i5++)
    //                         for(int i6 = 0; i<PHASE_NUM_POINTS[2]; i6++)
    //                         {
    //                              float m_rt[3] = {}
    //                              float p_rt[3] = {}
    //                              float order_rt[3]={1, 3, 5};
    //                              float ref_phase = 0;
    //                              map_on[i1][i2][i3][i4][i5][i6] = pwm_with_harmonic(m_rt, p_rt,order_rt, 3, 1 )
    //                         }

               
    //     }
    // }
