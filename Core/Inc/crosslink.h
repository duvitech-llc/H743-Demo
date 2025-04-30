/*
 * crosslink.h
 *
 *  Created on: Aug 6, 2024
 *      Author: gvigelet
 */

#ifndef INC_CROSSLINK_H_
#define INC_CROSSLINK_H_
#include "main.h"
#include <stdbool.h>

void fpga_configure();

int program_bitstream(void);

#endif /* INC_CROSSLINK_H_ */
