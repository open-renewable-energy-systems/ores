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
#include <queue>

# define PI 3.141592653589 
# define NOMM 8       // number of max modules

struct PIGain
{
   float Pgain_11, Pgain_12, Pgain_21, Pgain_22 = 1.0;
   float Igain_11, Igain_12, Igain_21, Igain_22 = 1.0; 
};

struct dq_controller
{
   int id_err_integ = 0;
   int iq_err_integ = 0;
   void dq_PI(float d_ref, // d 轴目标值
              float q_ref, // q 轴目标值
              float d_est, // d 轴估计值
              float q_est, // q 轴估计值
              float ud_grid, // d 轴电网电压，即为电网电压最大值
              float uq_grid, // q 轴电网电压， 为0
              float ud_smb, // d轴smb期望到达电压
              float uq_smb, // q轴smb期望到达电压
              PIGain gains, // 控制器增益
              float dt,     // 时间周期
              float max_derrInteg,  // 最大d轴累计误差
              float max_qerrInteg) // 最大 q 累计误差
   {
       float d_err = d_est - d_ref;
       float q_err = q_est - q_ref;

       id_err_integ += d_err*dt;
       iq_err_integ += d_err*dt;
       if(id_err_integ >  max_derrInteg) id_err_integ = max_derrInteg;
       if(id_err_integ < -max_derrInteg) id_err_integ = - max_derrInteg;

       if(iq_err_integ >  max_qerrInteg) iq_err_integ = max_qerrInteg;
       if(iq_err_integ < -max_qerrInteg) iq_err_integ = - max_qerrInteg;

       float fd_q = gains.Pgain_11 * d_err + gains.Pgain_12*q_err; // feedback with P
       float fd_d = gains.Pgain_21 * d_err + gains.Pgain_22*q_err; // feedback with P
       fd_d       += gains.Igain_11 * id_err_integ + gains.Igain_12 * id_err_integ; // feedback with I 
       fd_q       += gains.Igain_21 * id_err_integ + gains.Igain_22 * iq_err_integ; // feedback with I
       ud_smb      = ud_grid - fd_d; // with feeedforward
       uq_smb      = uq_grid - fd_q; // with feedforward 
   }
};