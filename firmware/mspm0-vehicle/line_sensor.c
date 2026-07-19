#include "line_sensor.h"

#include "ti_msp_dl_config.h"

/*
 * line_sensor.c - 8路GPIO数字传感器实现 (带校准验证)
 * line_sensor.c - 8-channel GPIO digital sensor with calibration validation
 *
 * 校准原理 / Calibration flow:
 *   1. 快速16次读取
 *   2. 统计每位为高的次数 (≥8次→判断为"白"/背景色)
 *   3. 校准时: raw XOR white_pattern → black=1
 *
 * 去抖动 / Debounce:
 *   连续3次读取 → (a&b)|(a&c)|(b&c) → 2/3多数投票
 */

/* 校准值: 哪些位为"白/背景"(高反射) / Calibration: which bits are "white" (high reflection) */
static uint8_t g_white_raw;

/* 校准状态: 是否成功完成 / Calibration status: whether init succeeded */
static bool g_cal_ok;

/* 单个GPIO输入读取 / Read single GPIO input */
static uint8_t read_input(GPIO_Regs *port, uint32_t pin)
{
    return (DL_GPIO_readPins(port, pin) != 0U) ? 1U : 0U;
}

/* 一次读取8路传感器 (未校准原始值) / Read all 8 channels once (raw, uncalibrated) */
static uint8_t read_raw_once(void)
{
    uint8_t value = 0U;

    /*
     * 位映射 / Bit mapping:
     *   value bit7 = X1 (PA15, 最左边)
     *   value bit6 = X2 (PA17)
     *   value bit5 = X3 (PA22)
     *   value bit4 = X4 (PA24)
     *   value bit3 = X5 (PA25)
     *   value bit2 = X6 (PA26)
     *   value bit1 = X7 (PA27)
     *   value bit0 = X8 (PB20, 最右边)
     */
    value |= (uint8_t)(read_input(GPIO_LINE_LINE_X1_PORT, GPIO_LINE_LINE_X1_PIN) << 7);
    value |= (uint8_t)(read_input(GPIO_LINE_LINE_X2_PORT, GPIO_LINE_LINE_X2_PIN) << 6);
    value |= (uint8_t)(read_input(GPIO_LINE_LINE_X3_PORT, GPIO_LINE_LINE_X3_PIN) << 5);
    value |= (uint8_t)(read_input(GPIO_LINE_LINE_X4_PORT, GPIO_LINE_LINE_X4_PIN) << 4);
    value |= (uint8_t)(read_input(GPIO_LINE_LINE_X5_PORT, GPIO_LINE_LINE_X5_PIN) << 3);
    value |= (uint8_t)(read_input(GPIO_LINE_LINE_X6_PORT, GPIO_LINE_LINE_X6_PIN) << 2);
    value |= (uint8_t)(read_input(GPIO_LINE_LINE_X7_PORT, GPIO_LINE_LINE_X7_PIN) << 1);
    value |= read_input(GPIO_LINE_LINE_X8_PORT, GPIO_LINE_LINE_X8_PIN);

    return value;
}

/*
 * 传感器GPIO配置 (使用SysConfig的默认配置, 不加下拉)
 * Use SysConfig default pin config — no extra pull resistors.
 *
 * 数字传感器输出为推挽模式, 加下拉可能导致信号被拉低。
 * The digital sensor outputs are push-pull; adding pull-downs can
 * drag the signal LOW and cause false readings.
 */
static void sensor_gpio_config(void)
{
    /*
     * 保持 SysConfig 原始配置: RESISTOR_NONE
     * 如果传感器确实需要下拉(例如开漏输出), 在 empty.syscfg 中修改
     * Keep SysConfig defaults (RESISTOR_NONE).
     * If pull-downs are truly needed (e.g. open-drain sensors),
     * configure them in empty.syscfg instead.
     */
    (void)0;  /* no-op: use SysConfig defaults */
}

/*
 * 上电校准 / Power-on calibration
 *
 * 前提条件: 小车停在白色背景上
 * Prerequisite: car sitting on white surface (ground)
 *
 * 16次采样 → 统计每位高电平次数
 * ≥8次为高 → 判断该位对应传感器为"白"(高反射)
 * 校准成功后, XOR消去白色背景, 只保留黑线
 *
 * 16 samples → count high level per bit
 * ≥8 counts → mark that bit as "white"(high reflection)
 * After calibration, XOR removes white background, keeping only black line.
 *
 * 验证 / Validation:
 *   - 不能全部为白 (128 = 0xFF) → 传感器可能未连接或全部悬空
 *   - 不能全部为黑 (0x00)   → 可能放在黑线上校准了
 *   - Must not be all-white (0xFF) → sensors possibly disconnected or all floating
 *   - Must not be all-black (0x00) → possibly calibrated over black line
 */
void LineSensor_Init(void)
{
    uint8_t bit_count[LINE_SENSOR_COUNT] = {0U};
    uint8_t sample;
    uint8_t i;
    uint8_t bit;

    /* 使用SysConfig默认GPIO配置 / Use SysConfig default GPIO config */
    sensor_gpio_config();

    for (i = 0U; i < 16U; i++) {
        sample = read_raw_once();
        for (bit = 0U; bit < LINE_SENSOR_COUNT; bit++) {
            if (((sample >> bit) & 0x01U) != 0U) {
                bit_count[bit]++;
            }
        }
        /* 短暂延时让传感器稳定 / Brief delay between samples for stability */
        {
            volatile uint16_t d;
            for (d = 0U; d < 200U; d++) {
                __asm volatile ("nop");
            }
        }
    }

    g_white_raw = 0U;
    for (bit = 0U; bit < LINE_SENSOR_COUNT; bit++) {
        if (bit_count[bit] >= 8U) {
            g_white_raw |= (uint8_t)(1U << bit);
        }
    }

    /*
     * 校准验证: 排除异常情况
     * Calibration validation: reject pathological cases
     *
     * 注意: 此传感器输出 1=白色(高反射), 0=黑色(吸收)
     * 因此全白(0xFF)在校准中是正常且预期的!
     * Note: this sensor outputs 1=white(reflect), 0=black(absorb)
     * So all-white (0xFF) is normal and expected during calibration!
     */
    g_cal_ok = true;

    /* 全黑: 所有位都是低电平 → 可能放在黑线上校准或传感器全部故障 */
    /* All-black: every bit LOW → calibrated on black line or all sensors dead */
    if (g_white_raw == 0x00U) {
        g_cal_ok = false;
    }
}

/*
 * 校准是否成功 / Whether calibration succeeded
 */
bool LineSensor_IsCalibrated(void)
{
    return g_cal_ok;
}

/*
 * 获取校准白值 (调试用) / Get white calibration pattern (for debugging)
 */
uint8_t LineSensor_GetWhitePattern(void)
{
    return g_white_raw;
}

/*
 * 原始值读取 (3次读取投票去抖动) / Raw read with 3-sample majority debounce
 *
 * (a&b)|(a&c)|(b&c) → 2/3的多数投票
 * Only bits that appear in ≥2 of 3 reads are kept
 *
 * 输出: X1=bit7 ... X8=bit0, 1=高电平(白色/背景)
 * Output: X1=bit7 ... X8=bit0, 1=HIGH(white/ground)
 */
bool LineSensor_ReadRaw(uint8_t *raw)
{
    uint8_t a;
    uint8_t b;
    uint8_t c;

    if (raw == 0) {
        return false;
    }

    a = read_raw_once();
    b = read_raw_once();
    c = read_raw_once();

    /* 2/3 majority vote */
    *raw = (uint8_t)((a & b) | (a & c) | (b & c));
    return true;
}

/*
 * 校准后读取 / Calibrated sensor read
 *
 * 校准后: line_raw = raw XOR white_pattern
 * → 1=黑(吸收光), 0=白(背景)
 *
 * Calibrated: line_raw = raw XOR white_pattern
 * → 1=black(absorbing), 0=white(ground)
 *
 * value[0]=X1(leftmost), value[7]=X8(rightmost)
 */
bool LineSensor_Read(LineSensor_Data *data)
{
    uint8_t i;
    uint8_t line_raw;

    if ((data == 0) || !LineSensor_ReadRaw(&data->raw)) {
        return false;
    }

    /* XOR校准: 消去"白"的固定模式 / XOR calibration: remove "white" pattern */
    line_raw = (uint8_t)(data->raw ^ g_white_raw);

    /* 解码到value数组 (value[0]=X1, value[7]=X8) */
    for (i = 0U; i < LINE_SENSOR_COUNT; i++) {
        data->value[i] = (uint8_t)((line_raw >> (7U - i)) & 0x01U);
    }

    return true;
}
