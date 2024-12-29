#include "dq_controller_pid volt_feedback.h"

void DQControllerVoltFeedback_Init(DQControllerVoltFeedback_State* state, DQControllerVoltFeedback_Params* params) {
    // Cast the parameters to the base type
    DQController_Init((DQController_State*)state, (DQController_Params*)params);
}

void DQControllerVoltFeedback_Reset(DQControllerVoltFeedback_State* state) {
    DQController_Reset(state);
}

void DQControllerVoltFeedback_Update(DQControllerVoltFeedback_State* state, DQControllerVoltFeedback_Params* params) {
    // D-axis voltage control
    float error_d = -state->vd_meas + state->vd_ref;
    state->integral_d += error_d * params->Ts;
    
    // Anti-windup
    if (state->integral_d > params->integral_max) state->integral_d = params->integral_max;
    if (state->integral_d < params->integral_min) state->integral_d = params->integral_min;
    
    // Negative sign added because increase in error should decrease control output
    state->vd_fb = params->kp_d * error_d + params->ki_d * state->integral_d;
    state->vd_ff = state->vd_meas;
    
    state->vd_out = state->vd_fb + state->vd_ff;

    // Q-axis voltage control
    float error_q = -state->vq_meas + state->vq_ref;
    state->integral_q += error_q * params->Ts;
    
    // Anti-windup
    if (state->integral_q > params->integral_max) state->integral_q = params->integral_max;
    if (state->integral_q < params->integral_min) state->integral_q = params->integral_min;
    
    // Negative sign added because increase in error should decrease control output
    state->vq_fb = params->kp_q * error_q + params->ki_q * state->integral_q;
    state->vq_ff = state->vq_meas;
    state->vq_out = state->vq_fb + state->vq_ff;
}

void DQControllerVoltFeedback_SetReference(DQControllerVoltFeedback_State* state, float vd_ref, float vq_ref) {
    state->vd_ref = vd_ref;
    state->vq_ref = vq_ref;
}

void DQControllerVoltFeedback_UpdateMeasurements(DQControllerVoltFeedback_State* state, float vd_out, float vq_out) {
    state->vd_meas = vd_out;
    state->vq_meas = vq_out;
}   

void DQControllerVoltFeedback_SetIntegralTerms(DQControllerVoltFeedback_State* state, float integral_d, float integral_q, DQControllerVoltFeedback_Params* params) {
    // Clamp the provided values within limits
    if (integral_d > params->integral_max) {
        state->integral_d = params->integral_max;
    } else if (integral_d < params->integral_min) {
        state->integral_d = params->integral_min;
    } else {
        state->integral_d = integral_d;
    }

    if (integral_q > params->integral_max) {
        state->integral_q = params->integral_max;
    } else if (integral_q < params->integral_min) {
        state->integral_q = params->integral_min;
    } else {
        state->integral_q = integral_q;
    }
}

float DQControllerVoltFeedback_GetVoltageD(DQControllerVoltFeedback_State* state) {
    return state->vd_out;
}

float DQControllerVoltFeedback_GetVoltageQ(DQControllerVoltFeedback_State* state) {
    return state->vq_out;
}

float DQControllerVoltFeedback_GetVoltageD_FF(DQControllerVoltFeedback_State* state) {
    return state->vd_ff;
}

float DQControllerVoltFeedback_GetVoltageD_FB(DQControllerVoltFeedback_State* state) {
    return state->vd_fb;
}

float DQControllerVoltFeedback_GetVoltageQ_FF(DQControllerVoltFeedback_State* state) {
    return state->vq_ff;
}

float DQControllerVoltFeedback_GetVoltageQ_FB(DQControllerVoltFeedback_State* state) {
    return state->vq_fb;
}
