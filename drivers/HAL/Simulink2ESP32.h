/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Simulink2ESP32.h
 *
 * Code generated for Simulink model 'Simulink2ESP32'.
 *
 * Model version                  : 1.3
 * Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
 * C/C++ source code generated on : Mon Sep 22 04:23:42 2025
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. ROM efficiency
 * Validation result: Passed (8), Warnings (2), Error (0)
 */

#ifndef Simulink2ESP32_h_
#define Simulink2ESP32_h_
#ifndef Simulink2ESP32_COMMON_INCLUDES_
#define Simulink2ESP32_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* Simulink2ESP32_COMMON_INCLUDES_ */

#include "Simulink2ESP32_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T e_prev;                       /* '<Root>/MATLAB Function' */
  real_T u_prev;                       /* '<Root>/MATLAB Function' */
} DW_Simulink2ESP32_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T Veldereferencia;              /* '<Root>/Vel. de referencia' */
  real_T Velmedida;                    /* '<Root>/Vel. medida' */
} ExtU_Simulink2ESP32_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T Control;                      /* '<Root>/Control' */
} ExtY_Simulink2ESP32_T;

/* Real-time Model Data Structure */
struct tag_RTM_Simulink2ESP32_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_Simulink2ESP32_T Simulink2ESP32_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_Simulink2ESP32_T Simulink2ESP32_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_Simulink2ESP32_T Simulink2ESP32_Y;

/* Model entry point functions */
extern void Simulink2ESP32_initialize(void);
extern void Simulink2ESP32_step(void);
extern void Simulink2ESP32_terminate(void);

/* Real-time Model object */
extern RT_MODEL_Simulink2ESP32_T *const Simulink2ESP32_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Simulink2ESP32'
 * '<S1>'   : 'Simulink2ESP32/MATLAB Function'
 */
#endif                                 /* Simulink2ESP32_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
