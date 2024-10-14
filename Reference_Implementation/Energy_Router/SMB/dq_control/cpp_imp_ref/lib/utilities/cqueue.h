
#ifndef CQUEUE_H_
#define CQUEUE_H_

#include <stdio.h>
#include <stdint.h>
#include <cmath>
//#include <array>
#include <cstdlib>
#include <iostream>
#include <assert.h>     /* assert */

#include <stdlib.h>

/// @brief impl c type of queue function with fixed size; 
/// provide O(1) function: push, front, back
/// x x x x x x x (while data is full)
///   |            <- back() 
///     |          <- front()

/// x x x x x x x (while data is not full)
///   |            <- back() 
/// |              <- front()
struct cqueue 
{
    float * data;
    int curr_index = 0;
    int max_size   = 1;
    bool is_full   = false;
    float last_value = 0.0;

    cqueue(const int max_size):max_size(max_size)
    {
        data = (float*)malloc(max_size * sizeof(float));
    }
    void push(float & val)
    {
        data[curr_index] = val;
        last_value = val;
        curr_index++;
        curr_index = curr_index%max_size; 
        if(!is_full && curr_index == max_size -1) is_full= true;
    }

    float front()
    {
        if(!is_full && curr_index != 0) return data[0];
        else if(!is_full && curr_index ==0)  return 0.0;
        else return data[curr_index%max_size];
    }

    float back()
    { 
        return last_value;
    }

    void reset()
    {
        curr_index = 0;
        is_full = false;
    }
    ~cqueue()
    {
        free(data);
    }; // free memory
};

#endif


// cqueue Period data such as angle 缓冲周期数据
// struct Periodcqueue 
// {
//     int max_size = 1;
//     float T = 2*PI;
//     cqueue data;
//     Periodcqueue(const int & max_size, const float & T)
//     : max_size(max_size)
//     , data(max_size)
//     , T(T)
//     {};

//     void push(float val)
//     {
//         if(!data.is_full && data.curr_index==0) { data.push(val); return; }
//         while(data.back() - val < 0 ) val = val + T;
//         while(val - data.back() > T)  val = val - T;
//         data.push(val);
//     }

//     float front()
//     {
//        while(data.front() > T)
//        {
//          for(int i = 0; i<data.max_size; i++)
//           data.data[i] = data.data[i]-T;
//        }
//     }

//     float back()
//     { 
//         return data.back();
//     }

//     void reset()
//     {
//         data.reset();
//     }
//     ~Periodcqueue() {data.~cqueue();}; // free memory
// };

