/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Simulink2ESP32.c
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

#include "Simulink2ESP32.h"
#include "rtwtypes.h"

/* Block states (default storage) */
DW_Simulink2ESP32_T Simulink2ESP32_DW;

/* External inputs (root inport signals with default storage) */
ExtU_Simulink2ESP32_T Simulink2ESP32_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_Simulink2ESP32_T Simulink2ESP32_Y;

/* Real-time model */
static RT_MODEL_Simulink2ESP32_T Simulink2ESP32_M_;
RT_MODEL_Simulink2ESP32_T *const Simulink2ESP32_M = &Simulink2ESP32_M_;

/* Model step function */
void Simulink2ESP32_step(void)
{
  real_T e;

  /* MATLAB Function: '<Root>/MATLAB Function' incorporates:
   *  Inport: '<Root>/Vel. de referencia'
   *  Inport: '<Root>/Vel. medida'
   */
  e = Simulink2ESP32_U.Veldereferencia - Simulink2ESP32_U.Velmedida;
  Simulink2ESP32_DW.u_prev = ((e - Simulink2ESP32_DW.e_prev) * 0.5 +
    Simulink2ESP32_DW.u_prev) + 0.01 * e;
  if (Simulink2ESP32_DW.u_prev > 1.0) {
    Simulink2ESP32_DW.u_prev = 1.0;
  } else if (Simulink2ESP32_DW.u_prev < -1.0) {
    Simulink2ESP32_DW.u_prev = -1.0;
  }

  Simulink2ESP32_DW.e_prev = e;

  /* End of MATLAB Function: '<Root>/MATLAB Function' */

  /* Outport: '<Root>/Control' */
  Simulink2ESP32_Y.Control = Simulink2ESP32_DW.u_prev;
}

/* Model initialize function */
void Simulink2ESP32_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void Simulink2ESP32_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
