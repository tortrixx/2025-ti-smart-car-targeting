#ifndef LINE_SENSOR_H
#define LINE_SENSOR_H

#include <stdbool.h>
#include <stdint.h>

#define LINE_SENSOR_I2C_ADDRESS (0x12U)
#define LINE_SENSOR_DATA_REGISTER (0x30U)

/* X1 is bit 7 and X8 is bit 0. Black is 0; white is 1. */
bool line_sensor_read(uint8_t *raw_value);

#endif
