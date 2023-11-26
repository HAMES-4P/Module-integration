/*
 * Driver_Stm.h
 *
 *  Created on: 2023. 11. 26.
 *      Author: 현대오토9
 */

#ifndef LIBRARIES_BSW_APPSCHEDULER_DRIVER_STM_H_
#define LIBRARIES_BSW_APPSCHEDULER_DRIVER_STM_H_

/***********************************************************************/
/*Include*/
/***********************************************************************/
// STM 헤더들
#include "Ifx_Types.h"
#include "IfxStm.h"
#include "IfxCpu_Irq.h"

/***********************************************************************/
/*Typedef*/
/***********************************************************************/
typedef struct
{
    uint8_t u8nuScheduling1msFlag;
    uint8_t u8nuScheduling10msFlag;
    uint8_t u8nuScheduling50msFlag;
    uint8_t u8nuScheduling100msFlag;
    uint8_t u8nuScheduling500msFlag;
    uint8_t u8nuScheduling1000msFlag;
    uint8_t u8nuScheduling10000msFlag;
}SchedulingFlag;

/***********************************************************************/
/*Define*/
/***********************************************************************/

/***********************************************************************/
/*External Variable*/
/***********************************************************************/
extern SchedulingFlag stSchedulingInfo;

/***********************************************************************/
/*Global Function Prototype*/
/***********************************************************************/
extern void Driver_Stm_Init(void);

#endif /* LIBRARIES_BSW_APPSCHEDULER_DRIVER_STM_H_ */
