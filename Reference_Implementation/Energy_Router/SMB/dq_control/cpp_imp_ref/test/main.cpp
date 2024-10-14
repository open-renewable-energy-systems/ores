#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <map>
#include <vector>
//#include <array>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <assert.h>     /* assert */
#include <fstream>
#include <queue>
#include "../lib/utilities/cqueue.h"
#include "../lib/transform/clarke.h"
#include "../lib/transform/park.h"
#include "../lib/filter/low_pass_filter.h"
// # define PI 3.141592653589

#include "cfg_main.h"








#ifdef COMPILE_MCU_CPP
typedef void *      (*MALLOC_FUNC)(size_t size);
typedef void        (*FREE_FUNC)(void *p);
typedef void *      (*REALLOC_FUNC)(void* p, size_t want);
typedef void *      (*CALLOC_FUNC)(size_t nmemb, size_t size);
typedef int         (*PRINT_FUNC)(const char* format, ...);

typedef void        (*CPP_IMP_REF_FUNC)(void);
typedef void        (*CPP_SET_MALLOC_FUN)(MALLOC_FUNC malloc_fun, FREE_FUNC free_fun, REALLOC_FUNC realloc_fun, CALLOC_FUNC calloc_fun, PRINT_FUNC print_fun);

MALLOC_FUNC     fun_os_malloc       = NULL;
FREE_FUNC       fun_os_free         = NULL;
REALLOC_FUNC    fun_os_realloc      = NULL;
CALLOC_FUNC     fun_os_calloc       = NULL;
PRINT_FUNC      fun_os_print        = NULL;

#pragma import(__use_no_heap_region)  //声明不使用C库的堆
extern "C" void *malloc(size_t size)
{
    return fun_os_malloc(size);
}
extern "C" void free(void* p)
{
    fun_os_free(p);
}
extern "C" void *realloc(void* p, size_t want)
{
    return fun_os_realloc(p, want);
}
extern "C" void *calloc(size_t nmemb, size_t size)
{
    return fun_os_calloc(nmemb, size);
}

void cpp_set_malloc_fun(MALLOC_FUNC malloc_fun, FREE_FUNC free_fun, REALLOC_FUNC realloc_fun, CALLOC_FUNC calloc_fun, PRINT_FUNC print_fun) __attribute__((section(".ARM.__at_0x08085344")));
void cpp_set_malloc_fun(MALLOC_FUNC malloc_fun, FREE_FUNC free_fun, REALLOC_FUNC realloc_fun, CALLOC_FUNC calloc_fun, PRINT_FUNC print_fun)
{
    fun_os_malloc       = malloc_fun;
    fun_os_free         = free_fun;
    fun_os_realloc      = realloc_fun;
    fun_os_calloc       = calloc_fun;
    fun_os_print        = print_fun;
}
#else
#endif











#ifdef COMPILE_MCU_CPP
void cpp_imp_ref(void) __attribute__((section(".ARM.__at_0x08084344")));
#else
#endif
void cpp_imp_ref(void)
{
    #ifdef COMPILE_MCU_CPP
    if(fun_os_print == NULL) return;
    fun_os_print("Start cpp_imp_ref print\r\n");
    #else
    #endif

    std::vector<float> t;
    std::vector<float> wt;
    std::vector<float> current;
    std::vector<float> Alpha;
    std::vector<float> Beta;

// prepare config for low pass filter
    Config_low_pass_filter config1;
    config1.mode = 0; // 0: 脉冲法 1: 欧拉法  2: 双线性变化  https://blog.csdn.net/weixin_45024950/article/details/126734025
    config1.tao  = 1.0f / (2.0f * PI * 50.0f); // tao = RC
    config1.k    = 1.0f;
    config1.T    = 1.0f / 1000.0f;
////////////////////////////////////////

// prepare config for low pass filter
    Config_low_pass_filter config2;
    config2.mode = 1; // 0: 脉冲法 1: 欧拉法  2: 双线性变化  https://blog.csdn.net/weixin_45024950/article/details/126734025
    config2.tao  = 1.0f / (2.0f * PI * 5.0f); // tao = RC
    config2.k    = 1.0f;
    config2.T    = config1.T;
////////////////////////////////////////

    float dt = config1.T;
    float tmin = 0.0f;
    float tmax = 1.0f;
    int len = round((tmax - tmin) / dt) + 1;
    float omega = 2.0f * PI * 50.0f;

    #ifdef COMPILE_MCU_CPP
    fun_os_print("len = %d\r\n", len);
    #else
    #endif

    for(int i = 0; i < len; i++)
    {
        t.push_back(float(i)*dt + tmin);
        wt.push_back(t.back() * omega);
        float I = sin(wt.back()) + 0.0f * sin(5.0f * wt.back());
        current.push_back(I);
    }

    Clarke_transform_1phase clk(config1.T, 50); // 基于滤波的 Clakke 变换
    Clarke_transform_1phase_complex clk2(2);    // 基于三角函数变换的Clarke变换
    Park_1phase_w_handy_filter prk(config2.T); // 带低通滤波的park变换
    // 带IIR滤波器的park变换
    // Park_1phase_w_handy_filter prk(Notch_Fc50Hz_Fs1k_BW40Hz_3x, Notch_Fc50Hz_Fs1k_BW40Hz_3x); //float T, float hz = 50.0, uint8_t mode = 1, float k = 1.0

    #ifdef COMPILE_MCU_CPP
    #else
    std::ofstream file("./data.txt");
    #endif
    for(int i = 0; i < len; i++)
    {
        float alpha, beta = 0.0;
        float return_angle = 0.0; // 如果使用三角函数变换，需要返回一个新的相位
//        bool filter = false;
        float d, q, m = 0.0; // m -幅值
        float oneStep_angle = abs(config1.T * omega);

        /////////alpha-beta transformation
        clk2.transform(current[i], wt[i], alpha, beta,  return_angle, oneStep_angle);
        // clk.transform(current[i], alpha, beta);
        //////////////

        Alpha.push_back(alpha);
        Beta.push_back(beta);

        ////////////alpah-beta to dq transformation
        Park_1phase::transform(alpha, beta, d, q, m, return_angle); // direct func call , no filter
        //  void static transform(float alpha, float beta, float &d, float &q, float & m,
        //           float wt)
        // prk.transform(alpha, beta, d, q, m, return_angle); // with handy filter
        ///////////

        #ifdef COMPILE_MCU_CPP
        fun_os_print("%u, %g, %g, %g, %g, %g, %g, %g, %g,\r\n", i, sin(return_angle), cos(return_angle), alpha, beta, sqrt(alpha * alpha + beta * beta), d, q, m);
        #else
        std::cout << "outside" << d << ", " << q << ", " << m << std::endl;
        file << t[i] * 1000 << ", " << sin(return_angle) << ", " << cos(return_angle)
             << ", " << alpha << ", " << beta << ", " << sqrt(alpha * alpha + beta * beta)
             << ", " << d     << ", " << q     << "," << m << ", "  << std::endl;
        #endif
    }

    #ifdef COMPILE_MCU_CPP
    fun_os_print("very nice\r\n");
    fun_os_print("End cpp_imp_ref print\r\n");
    #else
    file.close();
    std::cout << "very nice" << std::endl;
    #endif
}








int main()
{
    cpp_imp_ref();
    return 0;
}
