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

# define PI 3.141592653589 
# define NOMM 8       // number of max modules

////////////////////////////////////
// 定义调制参数
struct ModulationParam
{
  float fc    = 800.0;               // 载波频率
  float fg    = 50.0;                // 电网频率，
//   float k     = fc/(fg*PI);         // 系数K,为了计算三角波跟正弦的交线
  float dphs  = 2.0*PI*fg/fc; // （固定）相位间隔

  float get_ratio(float m)    //返回值用于计算三角波跟正弦交线
  {
   return m*fg*PI/fc;  //return m/k;  // 
  }

} mp; // mp is global variable //定义一个全局变量，方便直接用
//////////////////////////////////////////////////

//////////////////////////////////////////
/// 定义SPWM交线求解器的配置参数
struct Config_spwm_solver
{
    uint8_t max_iter = 5; //最多迭代次数
    float   err      = 0.000000314; //如果前低于此误差，终止迭代
} config; // config is global variable // 定义全局变量
////////////////////////////////////////

///////////////////////////////////////
///定义一个电池的基本结构，用于计算动态调制系数
struct BattModule  
{
  float   volt       = 48.0;  // 电压
  float   SOC        = 0.8;   // SOC
  float   capacity  = 100.0; // Ah
  float   volt_norm  = 48.0; // 名义电压
  float   SOC_min    = 0.0;
  float   SOC_max    = 1.0;
  float   volt_min   = 40.0;
  float   volt_max   = 60.0;
  float   m          = 0.8; // 调制系数
  float   m_norm     = 0.8; //名义调制系数，暂且不用，以后再说
  uint8_t soft_ID    = 1;     // 可动态变化的ID
  uint8_t hard_ID    = 0;     // 物理ID
  bool    isAvailable = true;  // 是否可用
};

///封装所有模块的参数
struct BattModules
{
public:
  BattModules() //初始化
  {
     for (uint8_t i = 0; i<NOMM; i++) 
     {
         battModules[i]         = BattModule();
         battModules[i].soft_ID = i;
         battModules[i].hard_ID = i;
     }
  };

  int8_t nom = 8; // number of modules //动态的模块个数，6-8 个
  BattModule *battModules       = (BattModule*)malloc(sizeof(BattModule) * NOMM); //按照最大个数定义数组
  std::vector<BattModule>       aBattModules; // available modules //动态数据存储

   void SOC_balance_discharge(float volt_req) 
   {
     float sum = 0.0;
     float ratio = 0.0;

     for (uint8_t i = 0; i<nom; i++)
     {
         sum      += battModules[i].volt * ( battModules[i].SOC - battModules[i].SOC_min)*battModules[i].capacity; //要确保SOC>SOC_min
     }

     ratio = volt_req/sum;
     
     for (uint8_t i = 0; i<nom; i++)
     {
         battModules[i].m =  ratio * ( battModules[i].SOC - battModules[i].SOC_min)*battModules[i].capacity;
         battModules[i].volt_norm =  battModules[i].m * battModules[i].volt;
         battModules[i].m_norm =  battModules[i].m;
     }
   }


   void SOC_balance_charge(float volt_req) 
   {
     float sum = 0.0;
     float ratio = 0.0;

     for (uint8_t i = 0; i<nom; i++)
     {
         sum      += battModules[i].volt * ( battModules[i].SOC_max - battModules[i].SOC); //后面需要要加过0处理
     }

     ratio = volt_req/sum;
     
     for (uint8_t i = 0; i<nom; i++)
     {
         battModules[i].m         =  ratio * ( battModules[i].SOC_max - battModules[i].SOC);
         battModules[i].volt_norm =  battModules[i].m * battModules[i].volt;
         battModules[i].m_norm =  battModules[i].m;
     }
   }
   
   /// @brief 估计SOC的简单方法，以后需要跟新修正
   void rough_est_soc_basedOn_volt()
   {
    for (uint8_t i = 0; i<nom; i++)
     {
        battModules[i].SOC =  ( battModules[i].volt -  battModules[i].volt_min )/ ( battModules[i].volt_max -  battModules[i].volt_min);
     }
   }


// 估值电池恢复，可进行push操作，把电池纳入总体计算
  void push (const BattModule & module)
  {
    nom += 1;
    if (aBattModules.size()>0) 
    {
        uint8_t preSoftID = aBattModules.back().soft_ID;
        aBattModules.push_back(module);
        aBattModules.back().soft_ID = preSoftID+1;
    }
    else 
    {
        aBattModules.push_back(module);
        aBattModules.back().soft_ID = 0;
    }
  }

// 电池故障旁路，做删除操作
  void erase (uint8_t index)
  {
    aBattModules.erase(aBattModules.begin()+index);
    nom -= 1;
    for (uint8_t i = index; i<aBattModules.size(); i++) 
    {
       aBattModules[i].soft_ID = aBattModules[i].soft_ID - 1;
    }
  }
} bms; // bms is global variable // 定义一个电池系统的全局变量


// 求余
float mod(float num, float T) 
{
  while(num>T) num -= T;
  while(num<0) num += T;
  return num;
};

// 根据如下公式计算PWM开启 和关闭交线
// implement k*dx = M*sin(x_ref + dx); dx = M/k*sin(x_ref + dx)
inline float pwm_off(float m, float ref_phase, 
                     ModulationParam & mp, 
                     const Config_spwm_solver & config = Config_spwm_solver()) 
{
    float dx_last = 0.0;
    float dx      = 0.0;

    ref_phase = mod(ref_phase, 2*PI);
    float ref_phase_lk = mod(ref_phase, PI); // 保证在[0, pi]之间

    for(int i = 1; i < int(config.max_iter); i++) 
    {
       dx = mp.get_ratio(m) * sin(ref_phase_lk + dx);

       if (abs(dx-dx_last) < config.err) break;
       dx_last = dx;
    }
    // std::cout<<"pwm_off solver ref_phase:  " << ref_phase
    //          << ", solution" << ref_phase + dx
    //          << ", ratio: "  << mp.get_ratio(m)
    //          <<std::endl;

    return ref_phase + dx;
};

inline float pwm_on(float m, float ref_phase, 
                     ModulationParam & mp, 
                     const Config_spwm_solver & config = Config_spwm_solver()) 
{
    float dx_last = 0.0;
    float dx      = 0.0;
    ref_phase = mod(ref_phase, 2*PI);
    float ref_phase_lk = mod(ref_phase, PI);

    for(uint8_t i = 1; i < config.max_iter; i++) 
    {
       dx = - mp.get_ratio(m) * sin(ref_phase_lk + dx);
       if (abs(dx-dx_last) < config.err) break;
       dx_last = dx;
    }
    return ref_phase + dx;
};

// 单模块计数器表格
struct TimerTable 
{
   TimerTable(float phase_init)
   {
    // std::cout<< "==========" <<std::endl;
    this->initlization(phase_init); 
   }
   
    int           numM         = 51; // 查表调制系数的个数
    int           ratioFrq     = round(mp.fc/mp.fg); // 频率比
    float         m_init       = 0.5; 
    float         m_end        = 1.00; // 调制最大，最小值[m_init, m_end]
    float         delt_m       = 0.0001; //建表时的调制系数间隔
    float         phase_init_   = 0;      // 根据模块编号定的初始相位
    uint8_t       module_index = 0;     // 模块编号
    uint16_t      min_count    = 1;

   std::vector<         float      > m_index;

   std::vector< std::vector<float> > phase_merge;
   std::vector< std::vector<float> > delt_phase;
  //  std::vector< std::vector<bool>  > operation_value_;
  //  std::vector< std::vector<bool>  > delt_phase;
   
   std::vector<         float      > phase_merge_temp;
   std::vector<         float      > delt_phase_temp;

   std::vector<         float      > delt_ptable_at_current_m; // 查表相位间隔
   std::vector<         uint16_t   > delt_ctable_at_current_m; // 查表计数器间隔
   
   std::vector<         bool       > operation_value;          //中断改写值
   std::vector<         bool       > operation_direction;      //中断改写正向或负向导通
   std::vector<         float      > phase_merge_at_mt;
   std::vector<         uint16_t   > phase_merge_count_at_mt;


   // 初始化
   void initlization(float phase_init) 
   {
        // std::cout<< "initlization for phase_init: " << phase_init <<std::endl;
        phase_init_ = phase_init;
        delt_m       = (m_end-m_init)/float(numM-1);
        m_index.clear();

        for (int i = 0; i< numM; i++) 
        {
            m_index.push_back(m_init + delt_m*float(i));
            // std::cout<<" " << m_index.back();
        }

        // std::cout<< "*******size of m_index: " << m_index.size() <<std::endl;

      for (int i = 0; i< numM ; i++)
        {
            phase_merge_temp.clear();

            phase_merge_temp.push_back(0.0);

            delt_phase_temp.clear();
            
            // std::cout<<"------------printing table at " << i << "----------" <<std::endl;

            for (int j = 0; j<ratioFrq; j++) 
            {
              float ref_phase = phase_init + float(j)*mp.dphs;
              // if (j == ratioFrq-1)
               { 
                // std::cout <<  "******ref_phase " <<ref_phase<<std::endl;
               }
              float on        = pwm_on (m_index[i],  ref_phase, mp, config);
              float off       = pwm_off(m_index[i],  ref_phase, mp, config);

              delt_phase_temp.push_back(on - phase_merge_temp.back());
              phase_merge_temp.push_back(on);
    
              delt_phase_temp.push_back(off -  on);
              phase_merge_temp.push_back(off);
              
            }
            ////最后一个值为0，截止到(2*PI+epsilon);
            // std::cout  << "last off phase" <<  phase_merge_temp.back() <<std::endl;
            delt_phase_temp.push_back(2*PI  - phase_merge_temp.back());
            phase_merge_temp.push_back(2*PI);
            
            // std::cout<<"size of delt_phase table: "  << delt_phase_temp.size() <<std::endl;
            // std::cout<<"size of phase_merge table: " << phase_merge_temp.size() <<std::endl;
             phase_merge.push_back (phase_merge_temp);
             delt_phase.push_back (delt_phase_temp);
            //  std::cout  << "last phase "
            //             << delt_phase.back() <<std::endl;

        }    
    }

   void update_operation() 
   {
    
    
    operation_value.clear();
    operation_direction.clear();

    for (int j = 0; j<ratioFrq; j++) 
      {
        operation_value.push_back(false);
        operation_value.push_back(true);

        if (j < int(ratioFrq/2)) // 正半周期
        {
          operation_direction.push_back(true);
          operation_direction.push_back(true);
        }
        else  // 负半周期 
        {
          operation_direction.push_back(false);
          operation_direction.push_back(false);
        }

     }

      operation_value.push_back(false);
      operation_direction.push_back(false);
      std::cout<<std::endl;
  }

// 线性查表
     float linear_interp(float m, uint8_t ratioFrqIndex) 
     {
         assert(m <=m_end && m>=m_init);

         int lb_index  = floor( (m-m_init)/delt_m);
         int ub_index  = ceil( (m-m_init)/delt_m);

         float left_v  = float(lb_index)*delt_m + m_init;      
         float ratio   = (m - left_v)/delt_m;

         float t1      = delt_phase[lb_index][ratioFrqIndex];
         float t2      = delt_phase[ub_index][ratioFrqIndex];

         float dt      = (1.0-ratio)*t1 + ratio*t2;
         return dt;
     }

// 得到当前调制系数 m 情况下，计时器表格 
     void get_table_at_current_m(float m) 
     {
         for (int i = 0; i<int(ratioFrq); i++) 
         {
           delt_ptable_at_current_m.push_back(this->linear_interp(m, uint8_t(2*i)));
           delt_ptable_at_current_m.push_back(this->linear_interp(m, uint8_t(2*i+1)));
         }
        
        delt_ptable_at_current_m.push_back(this->linear_interp(m, uint8_t(2*ratioFrq)));

        this->update_operation();
         std::cout<<"Table update for given m is done"<<std::endl;
        
        this->get_phase_merge_current_m();

     }

////////////////////////////////////////////////////////
////////////记录phase的绝对值//////////////////////////////
     void get_phase_merge_current_m()
     {
       phase_merge_at_mt.clear();
       phase_merge_at_mt.push_back(delt_ptable_at_current_m[0]);

     for (uint8_t i = 1; i < uint8_t(delt_ptable_at_current_m.size()); i++)
       {
         phase_merge_at_mt.push_back(delt_ptable_at_current_m[i] + phase_merge_at_mt.back());
       }
     }
////////////////////////////////////////////////////////////////////////////////
     
// 直接计算，省去中间表格
     void get_table_at_current_m2(float m) 
     {
       
       delt_ptable_at_current_m.clear();
       phase_merge_temp.clear();
       phase_merge_temp.push_back(0.0);

       for (int j = 0; j<ratioFrq; j++) 
            {
              float ref_phase = phase_init_ + float(j)*mp.dphs;
 
              float on        = pwm_on (m,  ref_phase, mp, config);
              float off       = pwm_off(m,  ref_phase, mp, config);

              delt_ptable_at_current_m.push_back(on - phase_merge_temp.back());
              phase_merge_temp.push_back(on);

              delt_ptable_at_current_m.push_back(off -  on);
              phase_merge_temp.push_back(off);
              
            }
            ////最后一个值为0，截止到(2*PI+epsilon);
            // std::cout  << "last off phase" <<  phase_merge_temp.back() <<std::endl;
            delt_ptable_at_current_m.push_back(2*PI  - phase_merge_temp.back());
            phase_merge_temp.push_back(2*PI);
            
            this->update_operation();  

     }  
 

    // 转换相位差到时间
    // 需要在如下3个事件下操作：
    // a - 周期信号发生改变
    // b - 调制系数发生改变
    // c-  调制频率变化
        void conver_ptable_2_ctable(float p2c_ratio)
    {
        delt_ctable_at_current_m.clear();
        for (int i  = 0; i<delt_ptable_at_current_m.size(); i++)
        {
          uint16_t counts = static_cast<uint16_t> (round( delt_ptable_at_current_m[i] * p2c_ratio));

          // if (counts>1) counts--; //少计一个 （假定计数是从最高值计到0； 移植到嵌入式，适当调整
          delt_ctable_at_current_m.push_back(counts);
        //   this->handle_min_count(i);
        }

        float total_phase = 0;
            for (int j = 0; j<delt_ptable_at_current_m.size(); j++)
            {
              total_phase += delt_ptable_at_current_m[j];
            }
            std::cout<< "xxxxxxxxxxxxxTotal phase is " << total_phase <<std::endl;
            std::cout<< "error percentage compared to 2pi " << (total_phase - 2*PI)/(2*PI)*100.0 <<"%"<<std::endl;

                    // print the total number of counts 
            int total_count = 0;
            int count_2pi = p2c_ratio*2.0*PI;
            for (int j = 0; j<delt_ptable_at_current_m.size(); j++)
            {
              total_count += delt_ctable_at_current_m[j];
            }
            std::cout<< "xxxxxxxxxxxxxTotal count is " << total_count <<std::endl;
            std::cout<< "error counts compared to " << count_2pi
                                                    <<", "
                                                    << ( float (total_count)  - count_2pi) /(count_2pi)*100.0 <<"%"<<std::endl;

    }
   
   // 根据当前计时器的SPWM交线编号，计算计时数
    uint16_t get_current_cout(uint16_t current_index)
    {
        return delt_ctable_at_current_m[current_index];
    }

    void handle_min_count(uint16_t i)
    {
      if ( delt_ctable_at_current_m[i]   <  min_count && 
           operation_direction[i]       ==  operation_direction[i+1] &&
           i < ratioFrq ) 
      {
        operation_value[i] =  operation_value[i+1];
      }
    }

};

//对多模块 创建表格，每个模块的初始相位不同

struct TimerTables
{
   TimerTables() // 构建对象
    {
      this->initilization();
    }

  std::vector<TimerTable> timerTables; 
  // std::vector<>
  const float phase_init_adj = 0*mp.dphs/bms.nom/2;
  float ratio_phase_2_cout = pow(2.0,20.0)/(2*PI*mp.fg);
  
  // 初始化
  void initilization() 
    { 
 
      timerTables.clear();

      for (int i = 0; i< bms.nom; i++ )
      {
          float phase_init = 0.0;

        //   this->phase_shift_method2(i, phase_init, phase_init_adj);
          this->phase_shift_method1(i, phase_init);

        std::cout<<"before construction: "<<i
                << ", phase_init: "<< phase_init <<std::endl;

        std::cout<<"Table Creating at Module: " << i <<std::endl;

        TimerTable timeTable_temp(phase_init);

        std::cout<<"Table Creating at Module: "<< i << " DONE: " <<std::endl;


        timerTables.push_back(timeTable_temp);
    
        std::cout<<"after construction: "<<i
                << ", phase_init: "<< phase_init <<std::endl;
        std::cout<< " size of delta ctable " << timerTables.back().delt_ctable_at_current_m.size() << std::endl;
      }
    }

    void phase_shift_method1(int i, float & phase_init)
    {
       float i_float = float(i);

       if (i==0) i_float = float(bms.nom);

       phase_init = i_float*mp.dphs/bms.nom;
    }

    void phase_shift_method2(int i, float & phase_init, float phase_init_adjus)
    {
      phase_init = phase_init_adjus +  float(i)*mp.dphs/bms.nom;
    }

       // 对多模块跟新
    void update_table_at_m_t(float m, float p2c_ratio)
    {
    //  std::cout<<"table size: " << timerTables.size()<<std::endl;

     for (int i = 0; i< timerTables.size(); i++ )
      {
          ratio_phase_2_cout = p2c_ratio;
          std::cout<<"Creating 1D phase table given m at " << m <<std::endl;

          timerTables[i].get_table_at_current_m2(m); 

          //timerTables[i].get_table_at_current_m(m); 

          timerTables[i].conver_ptable_2_ctable(p2c_ratio); 

          std::cout<<"1D phase table created " << m <<std::endl;
          std::cout<<"timerTable at i: " << i <<std::endl;

          for (int j = 0; j < timerTables[i].delt_ctable_at_current_m.size(); j++) 
          {
            std::cout <<timerTables[i].delt_ctable_at_current_m[j]<< " " ;
          }
          std::cout<<std::endl<<std::endl; 

          std::cout<<"Operation value at i: " << i <<std::endl;

          for (int j = 0; j < timerTables[i].operation_value.size(); j++) 
          {
            std::cout <<timerTables[i].operation_value[j]<< " " ;
          }
          std::cout<<std::endl<<std::endl; 

          std::cout<<"Operation direction i: " << i <<std::endl;

          for (int j = 0; j < timerTables[i].operation_direction.size(); j++) 
          {
            std::cout <<timerTables[i].operation_direction[j]<< " " ;
          }
          std::cout<<std::endl<<std::endl; 

      }
    }

    void update_table_at_m_t(float m)
    {
      this->update_table_at_m_t(m, ratio_phase_2_cout);
    }
    // 在当前中断下，得到下个计数器的个数
    uint16_t get_counts_at_interruption(uint8_t module_index, uint16_t interruption_id)
    {
      //  if (interruption_id  < 5) 
      //  {
      //      std::cout<<"=================="<<std::endl;
      //      std::cout<< timerTables[module_index].delt_ptable_at_current_m[interruption_id] <<std::endl;
      //  }
      uint16_t count = timerTables[module_index].delt_ctable_at_current_m[interruption_id];

      std::cout << "count @ module "   << int(module_index) 
                << " & interruption id " << interruption_id
                << " : "                << count
                << std::endl;

       return count;
    }

};

//存储基本GPIO的操作信息，用于状态管理模拟
struct GPIOBase 
{
    bool      operation_value = false; // true- on, false:ff 
    bool      direction       = false; // true - positive, false: negative
    uint8_t   operation_id    = 0;     // gpi id
};

// 多个GPIO的信息，用于状态管理模拟
struct GPIOs
{
    GPIOs() {
        for (int i = 0; i< bms.nom; i++ )
                state.push_back(GPIOBase());
        }
    std::vector<GPIOBase> state;
};

// 模拟单个计时器基本参数
struct CouterBase
{ 
    uint16_t  over_flow_cout  = 1; // 计时最大溢出数
    uint8_t   operation_id    = 0;
    bool      operation_value = false;
    bool      direction       = false;
    uint8_t   interruption_index = 0;
   
   // 计时 -1 操作
    void  minus_one()
    {
       if (over_flow_cout>=1) 
          over_flow_cout--;
    }
};

// 处理外部中断（比如电压，电流反馈等，1000Hz+）
// a：跟新所有的表格（即新的调制系数m的表格）
// b. 读取当前相位（从总计时器读取），并根据相位偏差，计算在当前时刻 t的“目标相位”。 注意：需要从总计时器有个函数，能把计时“动态”转换成相位
// c. 根据 phase_merge_at_mt 来反向查找中断ID （这个值以前没有，现在需要加在timerTable 类中，添加这个变量，及跟新该变量的函数）
// d. 根据中断ID来确定本次中断的目标改写状态 （每个模块单处理）
// e. 根据中断ID + 1 的时刻和当前时刻 (phase_at_interrupt_IDplus1 - phase_at_current_t)，决定本次中断，需要改写计时器的个数 （每个模块单独处理）
// 注意:b步骤中的，读取相位， 对每个模块，最好分别读取（不提前读取，就是用时才读取），值可能有少许差别
// f. 计时器状态改写成功后，退出
struct ExternalInterruptHandle
{
  // m_new : 新的调制系数，主要是需要跟新电压
  // delta_phase_count: 新的相位变化，主要是根据反馈信号获得
  ExternalInterruptHandle(CouterBase * counter, TimerTables *timer_tables, float m_new, float delta_phase=0.0):
  counter(counter),
  timer_tables(timer_tables)
  {
    this->set_param(m_new, delta_phase);
  };

  void set_param(float m_new, int delta_phase =0.0)
  {
      m_new = m_new;
      delta_phase = delta_phase;
  } 

  void update_timer_tables()
  {
        // update all time tables 
        timer_tables->update_table_at_m_t(m_new, pow(2.0,20.0)/(2*PI*mp.fg)); // 看怎么合理设置这个值（跟随频率变化而变化）
  }

  // 根据counter ID， 跟新counter状态信息
  void update_counter(uint8_t counter_id)
  {
     // 得到当前的 phase + 一定的补偿
     // 根据当前的 phase 和 delta_phase_shift counter, 得到当前目标phase
     // 根据      timer_tables->timerTables[i].phase_merge_at_mt 查表得到ID
     // 得到ID即为目标中断ID，计算到下一个时刻的中断值，改写计时器。
     // 通过目标ID， 检索计时器的目标  IO 状态值
     // 产生中断，重新写改写计时器值
  }

  void get_current_phase()
  {
    // 得到当前的phase（主计时器？）
  }
  // void get_current_counter_value(uint8_t counter_id,  uint16_t & counter_current_value)
  // {
  //    /// 此处读STM32当前计时器数字，输入counter_id， 输出计时器数字
  //    // 
  // }
    CouterBase *counter;
    TimerTables *timer_tables;
    float m_new;
    float delta_phase;
 
}；


int main()
{
    uint64_t  count        = 0; // 全局计时器
    float     phase_max    = 2*PI; //仿真边界
    float     ratio_phase_2_cout = pow(2.0,20.0)/(2*PI*mp.fg);  // 1.0/(mp.fg * ts); // 1.0/(2*PI*mp.fg)*pow(2,22); counts/rad 
    uint32_t  count_max    = phase_max *ratio_phase_2_cout; // radius to counts

    GPIOs     gpios         = GPIOs();

    CouterBase counters[NOMM];

    TimerTables timerTables = TimerTables();
    
    float m = 1.00;

    timerTables.update_table_at_m_t(m, ratio_phase_2_cout);

    float phase_t0 = 0.0;  // 前计数时刻相位
    float phase_t1 = 0.0;  // 当前计数相位
    bool reset_flag = false;

    std::ofstream mmcPhaseShift;
    mmcPhaseShift.open("mmc.csv"); // Create file
   
    for (int i = 0; i<count_max; i ++) 
    {

      mmcPhaseShift << i << ", ";
      
      phase_t0 = float(i-1)/ratio_phase_2_cout;
      phase_t1 = float(i)/ratio_phase_2_cout;
      phase_t0          = mod(phase_t0, 2*PI);
      phase_t1          = mod(phase_t1, 2*PI);
      reset_flag        = (phase_t1 < phase_t0); // 模拟过0 重置

      for (int j=0; j<bms.nom; j++) 
      {
        //  counters[j].interruption_index =0 || 
        if(reset_flag || i==0) // 在0脉冲 重写所有的计时器，这里的0是电池相位0，不是电网
        {
           counters[j].over_flow_cout      = timerTables.get_counts_at_interruption(j, 0);
           counters[j].interruption_index  = 0;

           gpios.state[j].operation_value  = timerTables.timerTables[j].operation_value[0];
           gpios.state[j].operation_id     = j;
           gpios.state[j].direction        = timerTables.timerTables[j].operation_direction[0];
          
        //   counters[j].interruption_index++;

        //   counters[j].minus_one();
          //  std::cout << "system reset with flag: "<<reset_flag << std::endl;
          //  std::cout << "1st couter: " << counters[j].over_flow_cout
          //            << " at module: " << j << std::endl;
        }
        else if (counters[j].over_flow_cout == 0) // 模拟某个电池的计时器到时，而产生的中断
        {
            // 模拟改写计时数
            counters[j].interruption_index++;

            uint16_t intr_index = int16_t(counters[j].interruption_index);
            uint8_t u8_j        = uint8_t(j);

            counters[j].over_flow_cout = timerTables.get_counts_at_interruption(u8_j, intr_index);
            // 改写GPIO的值
            gpios.state[j].operation_value = timerTables.timerTables[j].operation_value[int(intr_index)];
            gpios.state[j].operation_id    = j;
            gpios.state[j].direction       = timerTables.timerTables[j].operation_direction[int(intr_index)];              
          
            // std::cout << "Module : " << j << " new counter loaded as: "<< counters[j].over_flow_cout
            //           << " at step " << i
            //           << " current pwm index is " << int(counters[j].interruption_index)
            //           << std::endl;
            // 跟新PWM的编号
        }
        else 
        {
            counters[j].minus_one();
        }

        if (gpios.state[j].direction)
           mmcPhaseShift  <<  gpios.state[j].operation_value  <<", "; 
        else 
           mmcPhaseShift <<  -gpios.state[j].operation_value  <<", "; 
      }
      mmcPhaseShift << std::endl;
    }
    
    mmcPhaseShift.close();
    std::cout<<"gpio output file saved==========="<<std::endl;
    return 0;
    
}