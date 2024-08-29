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

struct Clarke_transform_1phase
{
   float    out1        = 0.0;
   float    out2        = 0.0;
   float    out3        = 0.0;
   float    a           = 1.0; 
   float init_alpha(float t_sample, float grid_hz) 
   {
       float tc = 1.0/(2.0*PI*grid_hz);
       a = exp(-tc*t_sample);
   } // y  = a * y + (1-a)*x

   void transforms(float curr_val, float & alpha, float & beta)
   {
      out1 = out1*a + (1.0-a)*curr_val; // first filter
      out2 = out2*a + (1.0-a)*out1;     // 2nd filter
      out3 = 2.0 * out2;                // adjust the value to match the mag
      alpha = curr_val;
      beta = out3;
   }
};

struct Clarke_transform_1phase_opti
{
   std::vector<float> data;
   std::vector<float> angle;
   int   max_cap = 1;
   bool  init_flag = false;
   float m1, m2 = 0.0; // measurement value
   float p1, p2 = 0.0; // phase
   float EPS = 0.000001f;
   float mag_est = 1.0;
   Clarke_transform_1phase_opti(int num_histr_data):max_cap(num_histr_data)
   {};

   void transforms(float curr_val,float curr_angle,
                   float & alpha, float & beta,
                   float & return_angle,
                   bool & data_valid, 
                   float min_mag = 0.01, 
                   float max_mag = 100.0, 
                   float max_ang_delt = PI/2.0,
                   float min_ang_delt = PI/8.0,
                   float mag_filter_tc = 0.001, 
                   float dt            = 0.00005)
   {
      
      if(!angle.empty() && curr_angle < angle.back()) curr_angle += 2*PI; // ensure the new angle is better than the last one
      data.push_back(curr_val);
      angle.push_back(curr_angle);
      
      // oversize, over max delta angle, erase the first one;
      if(data.size()>max_cap || (angle.back() - angle.front() ) > max_ang_delt)
       { data.erase(data.begin()); angle.erase(angle.begin());
         while(angle.front()<0.0)
            {
               for(auto & ang:angle)
               {
                  ang = ang + 2*PI;
               }
            }
            while(angle.front()>2.0*PI)
            {
               for(auto & ang:angle)
               {
                  ang = ang - 2*PI;
               }
            } 
       }
       
       if(data.empty() || angle.back()-angle.front() < min_ang_delt/100.0 || data.size()<3) data_valid = false;
       else data_valid = true;
       if(!data_valid) return;
      
      // A * [sin(a) + sin(b)] = 2Asin[(a+b)/2] * cos([(b-a)/2.0])
      // A * [sin(b) - sin(a)] = 2Acos[(a+b)/2] * sin([(b-a)/2.0])
      m2 = data.back(); // measurement data point 2
      p2 = angle.back(); // angle (phase) of measurement 2
      m1 = data.front(); // measurement of data point 1
      p2 = angle.back(); // angle (phase) of measurement 1

      while(p2<p1) p2 += 2.0*PI; // ensure p2>p1

      float temp1  = cos((p2-p1)/2.0);
      float temp2  = sin((p2-p1)/2.0);

      if(abs(temp1)<EPS) temp1 = EPS;
      if(abs(temp2)<EPS) temp2 = EPS;

      float Asin_half    = (m2+m1) / temp1 / 2.0;
      float Acos_half    = (m2-m1) / temp2 / 2.0;

      float new_mag_est = sqrt((Asin_half*Asin_half+Acos_half*Acos_half));
      if(new_mag_est<  min_mag) new_mag_est = min_mag;
      if(new_mag_est> max_mag) new_mag_est = max_mag;

      float a = exp(-dt/mag_filter_tc);
      mag_est = a * mag_est + (1.0-a)*new_mag_est;

      float alpha = Asin_half / new_mag_est;
      float beta = Asin_half / new_mag_est;
      return_angle = (p2+p1)/2.0;

      while(return_angle > 2.0*PI) return_angle -= 2*PI;
      while(return_angle < 0.0)    return_angle += 2*PI;
      
   }
};

struct Clarke_transform_3phase
{
    void transform(float a, float b, float c,float & alpha, float & beta, float k = 0.66666667)
   {
      alpha = ( a - 0.5*b - 0.5*c) * k;
      beta  = ( 0.866025*b - 0.866025*c) *k;
   } 
};