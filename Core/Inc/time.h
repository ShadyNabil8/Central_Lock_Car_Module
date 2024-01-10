/*
 * time.h
 *
 *  Created on: Jan 9, 2024
 *      Author: shady
 */

#ifndef INC_TIME_H_
#define INC_TIME_H_

/*
 * I used this include to get the uint32_t typedef.
 */
#include "main.h"

extern volatile uint32_t uwTichk;

/**
 * @brief This function returns the number of milliseconds (thousandths of a second)
 * since the STM32 board began running the current program..
 */
uint32_t Millis();

#endif /* INC_TIME_H_ */
