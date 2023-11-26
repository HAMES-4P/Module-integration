/*
 * AppScheduling.c
 *
 *  Created on: 2023. 11. 26.
 *      Author: 현대오토9
 */
#include "AppScheduling.h"
#include "Driver_Stm.h"
#include "ERU_Interrupt.h"
#include "GPIO.h"
#include "my_stdio.h"
#include "Buzzer.h"
#include "Tof.h"

typedef struct
{
    uint32_t u32nuCnt1ms;
    uint32_t u32nuCnt10ms;
    uint32_t u32nuCnt100ms;
    uint32_t u32nuCnt500ms;
}TestCnt;

// Static Function Prototype
//static void AppTask1ms();
//static void AppTask10ms();
static void AppTask100ms();
static void AppTask500ms();

TestCnt stTestCnt;

void AppScheduling()
{
    if(stSchedulingInfo.u8nuScheduling100msFlag == 1u)
    {
        stSchedulingInfo.u8nuScheduling100msFlag = 0u;
        AppTask100ms();
    }

    if(stSchedulingInfo.u8nuScheduling500msFlag == 1u)
    {
        stSchedulingInfo.u8nuScheduling500msFlag = 0u;
        AppTask500ms();
    }
    return;
}


void AppTask100ms()
{
    setLED1(0);
    setLED2(1);
}

void AppTask500ms()
{
    setLED1(1);
    setLED2(0);
}



