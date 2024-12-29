#ifndef DQ_TO_MODULATION_H
#define DQ_TO_MODULATION_H

#include <stdbool.h>

#define MAX_MOD_INDEX 1.0f

typedef struct {
    float vd;         // d-axis voltage component
    float vq;         // q-axis voltage component
    float vdc;        // DC bus voltage
} dq_voltage_t;

typedef struct {
    float index;           // Modulation index
    float phase_shift;     // Phase shift in radians
    float vd_adjust;       // Adjusted d-axis voltage component
    float vq_adjust;       // Adjusted q-axis voltage component
    bool valid;            // Indicates if the calculation was successful
} modulation_result_t;

modulation_result_t dq_to_modulation_calculate(dq_voltage_t dq_voltage);

#endif // DQ_TO_MODULATION_H
