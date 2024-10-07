
#ifndef PWM2_IO_H_
#define PWM2_IO_H_

# define PI 3.141592653589 

//定义调制参数
struct ModulationParam
{
  float fc    = 800.0;               // 调制频率
  float fg    = 50.0;                // 电网频率，
  
  float get_modulation_k()
  {
    return fc/(fg*PI);
  }

} mp; // mp is global variable //定义一个全局变量，方便直接用

inline float get_modulation_k(float fc, float fg) // 采样频率，电网频率，
{
    return fc/(fg*PI);
}

#endif