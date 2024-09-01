
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

/// @brief 历史数据缓冲；push和front(),back()时间复杂度O（1）
struct Buffer 
{
    float* data;
    int curr_index = 0;
    int max_size = 0;
    bool is_full = false;
    Buffer(const int max_size):max_size(max_size)
    {
        data = (float*)malloc(max_size * sizeof(float));
    }
    void push(float val)
    {
        data[curr_index++] = val;
        curr_index = curr_index%max_size; 
        if(!is_full && curr_index == max_size -1) is_full= true;
    }

    float front()
    {
         return data[(curr_index-1)%max_size];
    }

    float back()
    {
        return data[curr_index];
    }

    void reset()
    {
        curr_index = 0;
        is_full = false;
    }
    ~Buffer(){ delete data};
};