/*
 * time.c
 *
 *  Created on: Jan 9, 2024
 *      Author: shady
 */

#include "time.h"

/**
 * @brief This function returns the number of milliseconds (thousandths of a second)
 * since the STM32 board began running the current program..
 */
uint32_t Millis() {
	return uwTick;
}
