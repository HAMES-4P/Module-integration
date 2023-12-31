/**********************************************************************************************************************
 * \file Cpu0_Main.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 * 
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of 
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and 
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all 
 * derivative works of the Software, unless such copies or derivative works are solely in the form of 
 * machine-executable object code generated by a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "GPIO.h"
#include "Driver_Stm.h"
#include "my_stdio.h"
#include "ERU_Interrupt.h"
#include "TOF.h"
#include "Buzzer.h"
#include "AppScheduling.h"
#include "motor.h"

IfxCpu_syncEvent g_cpuSyncEvent = 0;

static const boolean get_log = 1;
static const float target_w_ref = 218;
static const float PWM_speed = 72;
static float speed_decel = 1.0;
static unsigned char now_motor_value = 0;
static int direction = 1;
static int sw1_old = 0;
static int sw1 = 0;
static int distance = 0;
static boolean mode = 0;

int core0_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    /* Initialize */
    initPeripheralsAndERU();
    Driver_Stm_Init();
    Init_Mystdio();
    Init_GPIO();
    Init_Buzzer();
    Init_ToF();
    Init_DCMotors();

    setLED1(0);
    setLED2(0);
    setBeepCycle(0);

    while(1)
    {
        // pulling 방식을 사용한 SW1 작동
//        sw1 = getSW1();
//        if (sw1_old != sw1)
//        {
//            if ( sw1_old && !sw1)
//            {
//                mode = !mode;
//                setLED1(mode);
//            }
//            sw1_old = sw1;
//        }

        mode = INTERRUT_VAL;
        setLED1(mode);

        /* laser module 수신 */
        distance = getTofDistance();
        if ( 100 < distance && distance <= 200) {
            speed_decel = 0.5;
            setBeepCycle(130);  // Beep every 0.5s
        }
        else if ( 0 < distance && distance <= 100)
        {
            speed_decel = 0.0;
            setBeepCycle(130);  // Beep every 0.5s
        }
        else
        {
            speed_decel = 1.0;
            setBeepCycle(0);    // Beep off
        }

        // mode에 따라 PID 제어를 ON/OFF
        if ( !mode )
        {
            movChA_PWM(PWM_speed * speed_decel, direction);
            if ( get_log && !(getcntDelay() % 10000) )
            {
                my_printf("PWM: %.1f, Vw: %.1f\n", (PWM_speed * speed_decel), getWValue());
            }
        }
        else
        {
            now_motor_value = motor_pid(target_w_ref * speed_decel);
            movChA_PWM(now_motor_value, direction);
            if ( get_log && !(getcntDelay() % 10000) )
            {
                my_printf("Vw: %.2f\n", getWValue());
            }
        }
        // AppScheduling();
    }
    return (1);
}
