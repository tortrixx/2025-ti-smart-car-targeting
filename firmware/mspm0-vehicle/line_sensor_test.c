#include "ti_msp_dl_config.h"

#include "board.h"
#include "drv_oled.h"
#include "line_sensor.h"

volatile uint8_t g_line_sensor_raw = 0xFFU;
volatile bool g_line_sensor_ok = false;

int main(void)
{
    uint8_t raw_value = 0xFFU;
    uint8_t sensor_index;

    SYSCFG_DL_init();
    oled_init();

    display_6_8_string(0, 0, "8-LINE SENSOR");
    display_6_8_string(0, 1, "0=BLK 1=WHT");
    display_6_8_string(0, 3, "X1 X2 X3 X4");
    display_6_8_string(0, 5, "X5 X6 X7 X8");

    while (1) {
        g_line_sensor_ok = line_sensor_read(&raw_value);
        if (g_line_sensor_ok) {
            g_line_sensor_raw = raw_value;
            display_6_8_string(0, 2, "I2C OK       ");

            for (sensor_index = 0U; sensor_index < 4U; sensor_index++) {
                display_6_8_number((unsigned char)(3U + sensor_index * 18U), 4,
                    (float)((raw_value >> (7U - sensor_index)) & 0x01U));
                display_6_8_number((unsigned char)(3U + sensor_index * 18U), 6,
                    (float)((raw_value >> (3U - sensor_index)) & 0x01U));
            }
        } else {
            display_6_8_string(0, 2, "I2C ERROR    ");
        }

        delay_ms(100);
    }
}
