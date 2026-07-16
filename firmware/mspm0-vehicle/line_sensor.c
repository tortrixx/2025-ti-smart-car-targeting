#include "line_sensor.h"

#include <stddef.h>

#include "ti_msp_dl_config.h"

#define LINE_SENSOR_TIMEOUT (1000000U)

static bool wait_until_idle(void)
{
    uint32_t timeout = LINE_SENSOR_TIMEOUT;

    while (timeout > 0U) {
        if ((DL_I2C_getControllerStatus(I2C_1_INST) &
                DL_I2C_CONTROLLER_STATUS_IDLE) != 0U) {
            return true;
        }
        timeout--;
    }

    return false;
}

static bool wait_until_bus_free(void)
{
    uint32_t timeout = LINE_SENSOR_TIMEOUT;

    while (timeout > 0U) {
        if ((DL_I2C_getControllerStatus(I2C_1_INST) &
                DL_I2C_CONTROLLER_STATUS_BUSY_BUS) == 0U) {
            return true;
        }
        timeout--;
    }

    return false;
}

bool line_sensor_read(uint8_t *raw_value)
{
    uint8_t register_address = LINE_SENSOR_DATA_REGISTER;

    if (raw_value == NULL) {
        return false;
    }

    DL_I2C_flushControllerTXFIFO(I2C_1_INST);
    DL_I2C_flushControllerRXFIFO(I2C_1_INST);
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, &register_address, 1U);

    if (!wait_until_idle()) {
        return false;
    }

    DL_I2C_startControllerTransfer(I2C_1_INST, LINE_SENSOR_I2C_ADDRESS,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1U);
    if (!wait_until_bus_free() || !wait_until_idle()) {
        return false;
    }

    DL_I2C_startControllerTransfer(I2C_1_INST, LINE_SENSOR_I2C_ADDRESS,
        DL_I2C_CONTROLLER_DIRECTION_RX, 1U);
    if (!wait_until_bus_free() || !wait_until_idle()) {
        return false;
    }

    *raw_value = DL_I2C_receiveControllerData(I2C_1_INST);
    return true;
}
