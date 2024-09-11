#ifndef IIRFILTER_H_
#define IIRFILTER_H_

#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <array>
#include <cstdlib>
// #include <assert.h>     /* assert */

/// IIR Filter y(n) = (B*X - A*Y)/a0 B = [b0, b1, ... bn]; A=[a1, a2, ... an]
/// reference: meta.ai, matlab fdatool, wiki 
struct IIRFilter
{
   float *x;
   float *y;
   float *a;
   float *b;
   int  curr_index = 0;
   int  order = 1;

   IIRFilter(const int order):order(order)
   {
      x = (float*)calloc((order+1), sizeof(float));// default 0
      y = (float*)calloc((order+1), sizeof(float)); // default 0
   }; // create 


   IIRFilter(const int order, float *a, float *b)
   :order(order)
   , a(a)
   , b(b)
   {
      x = (float*)calloc((order+1), sizeof(float));// default 0
      y = (float*)calloc((order+1), sizeof(float)); // default 0
   }; // 2nd case create with a, b as input
   
   float filter(float & input)
   {
     return this->filter(input, a, b);
   };
   
   /// @brief  perform filter based on external coeff a_, b_
   /// @param input: value
   /// @param a_: coeff for y
   /// @param b_: coeff for x
   /// @return filtered value
   float filter(float & input, float *a_, float *b_)
   {
       x[curr_index] = input;
       int temp_index = 1;
       int output = 0;
       for(int i = 0; i<order+1; i++)
       {
         temp_index = (curr_index-i)%(order+1);
         output += b_[i]*x[temp_index];
       }
       
       for(int i = 1; i<order+1; i++)
       {
         temp_index = (curr_index-i)%(order+1);
         output    -= a_[i]*y[temp_index];
       }
       output = output/a[0];
       y[curr_index] = output;
       curr_index = (curr_index+1)%(order+1);
       return output;
   }; 

   void reset()
   {
     curr_index = 0;
     for(int i = 0; i < order+1; i++)
     {
        x[i] = 0.0;
        y[i] = 0.0;
     }
   }

   struct IIRFilter operator=(IIRFilter & filter)
   {
       this->a     = filter.a;
       this->b     = filter.b;
       this->order = filter.order;
       this->x     = filter.x;
       this->y     = filter.y;
       return *this;
   };

   ~IIRFilter()
   {
      free(x);
      free(y);
   };
};

#endif