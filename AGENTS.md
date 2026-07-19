# Repository Guidelines

## Project Structure & Module Organization

This repository contains firmware for a three-controller targeting car and a standalone 2024 line-following car. `firmware/mspm0-vehicle/` serves dual purpose: it is the vehicle controller for the targeting car (CCD-based) and, on this branch (`codex/line-sensor-support`), the main controller for the 2024 competition line-following car (GPIO 8-way sensor + AT8236 motors).

### 2024 Line-Following Car (This Branch)

| File | Role |
|---|---|
| `firmware/mspm0-vehicle/empty.c` | Main: 10ms control loop, mode select (stop/sensor debug/track) |
| `firmware/mspm0-vehicle/line_follow.c` | State machine: WAIT_LINE → TRACK_LINE → BRIDGE → DONE, PD differential steering |
| `firmware/mspm0-vehicle/line_sensor.c` | 8-channel GPIO digital sensor: 16-sample calibration + 3-sample majority vote debounce |
| `firmware/mspm0-vehicle/keil/motor/motor.c` | Software PWM motor: 20-step cycle, 1ms update, ~50Hz PWM |
| `firmware/mspm0-vehicle/app_config.h` | **All tunable parameters**: speed, PID, bridge, turn limits, motor invert |
| `firmware/mspm0-vehicle/empty.syscfg` | SysConfig: GPIO inputs (8x sensor), TIMG PWM (2x motor) |
| `firmware/mspm0-vehicle/build/cli-build/` | CLI build output: .out, .hex, .map, generated ti_msp_dl_config |

The `firmware/stm32h750-gimbal/` and CCD code are not used in the line-following configuration.

Reference documents, analyses, and the PDF-index utility live in `docs/`.

Keep application logic in existing source folders. Treat `build/`, `Objects/`, `.axf`, and `.hex` as generated artifacts; they are ignored by Git.

## File Encoding

MSPM0 source files (`firmware/mspm0-vehicle/`, `firmware/mspm0-ccd/`) use **GBK (简体中文)** encoding. The STM32H750 gimbal uses UTF-8. Do not convert MSPM0 files to UTF-8 — garbled Chinese comments break readability. Open GBK files with the correct code page in your editor.

## Hardware Architecture

This is a three-controller targeting car communicating over UART:

| Controller | MCU | Project | Main File | Role |
|---|---|---|---|---|
| CCD | MSPM0G3507 | `firmware/mspm0-ccd/keil/...uvprojx` | `adc12_single_conversion.c` | Reads TSL1401 128-pixel line sensor via ADC, binarizes, sends 2-byte result on UART0 |
| Vehicle | MSPM0G3507 | `firmware/mspm0-vehicle/keil/...uvprojx` | `empty.c` | 1 kHz motor PID, encoder, IMU, forwards CCD data + encoder count to gimbal |
| Gimbal | STM32H750 | `firmware/stm32h750-gimbal/CMakePresets.json` | `Core/Src/main.c` | MaixCAM targeting, WIT IMU, 2-axis TMC2209 stepper tracking, laser control |

### Pin / Jumper Constraints

- **J17, J18** (MSPM0 LaunchPad): must be **removed** when using OLED on PA26/PA27 — these disconnect the onboard light sensor sharing those pins.
- **J15**: must remain **installed** — routes PA16 to J3.29 for line sensor I2C SDA.
- **I2C1** (PA15 SCL / PA16 SDA): 8-way line sensor module at address `0x12`, data register `0x30`. X1=bit7, X8=bit0, 0=black, 1=white.
- **I2C0** (PA27 SCL / PA26 SDA): SSD1306 128×64 OLED. Requires 3.3V (not 5V) to avoid pulling I2C lines above VCC.

## Build, Test, and Development Commands

Use the matching project for each controller:

```powershell
UV4.exe -b "firmware/mspm0-vehicle/keil/empty_LP_MSPM0G3507_nortos_keil.uvprojx"
UV4.exe -b "firmware/mspm0-ccd/keil/adc12_single_conversion_LP_MSPM0G3507_nortos_keil.uvprojx"
Push-Location firmware/stm32h750-gimbal
cmake --preset Debug
cmake --build --preset Debug
Pop-Location
```

MSPM0 Keil projects require `mspm0_sdk@2.04.00.06`. MSPM0 CCS projects (on this branch) need `mspm0_sdk@2.10.00.04` and TI Arm Clang. Preserve `.syscfg` pin assignments unless hardware changes.

CCS import paths:
- Vehicle: `firmware/mspm0-vehicle/ccs/empty_LP_MSPM0G3507_nortos_ticlang.projectspec`
- Line sensor test: `firmware/mspm0-vehicle/ccs/line_sensor_test_LP_MSPM0G3507_nortos_ticlang.projectspec`
- CCD: `firmware/mspm0-ccd/ccs/adc12_single_conversion_LP_MSPM0G3507_nortos_ticlang.projectspec`

### CLI Build & Flash (No CCS Required) — 2024 Line-Following Car

Tools installed at:
- **SysConfig**: `C:/ti/sysconfig_1.26.2/sysconfig_cli.bat`
- **Compiler**: `C:/ti/ti_cgt_arm_llvm_4.0.2.LTS/bin/tiarmclang.exe`
- **SDK**: `C:/ti/mspm0_sdk_2_10_00_04/`
- **Flasher**: `C:/ti/ccs2100/ccs/ccs_base/DebugServer/bin/DSLite.exe`
- **CCXML**: `C:/Users/sznnn/Desktop/car/targetConfigs/MSPM0G3507.ccxml`

```bash
# 1. Generate SysConfig (pin/peripheral config)
PROJ="firmware/mspm0-vehicle"
SDK="C:/ti/mspm0_sdk_2_10_00_04"
"C:/ti/sysconfig_1.26.2/sysconfig_cli.bat" \
  --product "$SDK/.metadata/product.json" \
  --device MSPM0G3507 --package "LQFP-64(PM)" \
  --compiler ticlang --output "$PROJ/build/cli-build" \
  "$PROJ/empty.syscfg"
cp "$PROJ/build/cli-build/ti_msp_dl_config."* "$PROJ/"

# 2. Build
TI_CLANG="C:/ti/ti_cgt_arm_llvm_4.0.2.LTS/bin/tiarmclang.exe"
OUT="firmware/mspm0-vehicle/build/cli-build"
CFLAGS="-mcpu=cortex-m0plus -mthumb -O2 -Wall"
INC="-I$PROJ -I$SDK/source -I$SDK/source/third_party/CMSIS/Core/Include -DDeviceFamily_MSPM0G350X"
for src in empty.c line_sensor.c line_follow.c "keil/motor/motor.c" ti_msp_dl_config.c; do
  "$TI_CLANG" $CFLAGS $INC -c "$PROJ/$src" -o "$OUT/$(basename $src .c).o"
done
# + startup file, + link (see previous build in transcript)

# 3. Flash (USB must be connected!)
DSLITE="C:/ti/ccs2100/ccs/ccs_base/DebugServer/bin/DSLite.exe"
"$DSLITE" flash \
  --config="$CCXML" \
  "$OUT/smart_car.out"
```

### 2024 Line-Following Car Pinout

**8-Channel Digital Grayscale Sensor** (GPIO direct read, no I2C):
| Sensor | Pin | GPIO Macro |
|--------|-----|------------|
| X1 (leftmost) | PA15 | `GPIO_LINE_LINE_X1_PORT/PIN` |
| X2 | PA17 | `GPIO_LINE_LINE_X2_PORT/PIN` |
| X3 | PA22 | `GPIO_LINE_LINE_X3_PORT/PIN` |
| X4 | PA24 | `GPIO_LINE_LINE_X4_PORT/PIN` |
| X5 | PA25 | `GPIO_LINE_LINE_X5_PORT/PIN` |
| X6 | PA26 | `GPIO_LINE_LINE_X6_PORT/PIN` |
| X7 | PA27 | `GPIO_LINE_LINE_X7_PORT/PIN` |
| X8 (rightmost) | PB20 | `GPIO_LINE_LINE_X8_PORT/PIN` |

Sensor type: Digital output with threshold comparator. **1=white (high reflection), 0=black (absorbing).** After XOR calibration: **1=black (line detected).**

**Calibration**: Car MUST be on white surface at power-up. `LineSensor_Init()` samples 16 times; bits that read HIGH ≥8 times are marked as "white" and XOR'd out during reads.

**AT8236 Dual Motor Driver** (Software PWM, ~50Hz):
| Motor | Forward | Reverse | GPIO Macros |
|-------|---------|---------|-------------|
| Left | PA12 (AIN1) | PA13 (AIN2) | `GPIO_PWM_LEFT_C0/C1_PIN` |
| Right | PA28 (BIN1) | PA31 (BIN2) | `GPIO_PWM_RIGHT_C0/C1_PIN` |

**Build Modes** (set in `empty.c`):
- `FORCE_MOTOR_STOP_TEST=1` — motors forced off (safety test)
- `SENSOR_RAW_DEBUG=1` — raw sensor read, observe `g_debug_line_raw` in debugger
- Both `0` (default) — normal line-following mode

The H750 project also opens in `firmware/stm32h750-gimbal/MDK-ARM/TMC2209.uvprojx` and targets STM32CubeH7 1.12.1 + `gcc-arm-none-eabi` for CMake builds.

## Coding Style & Naming Conventions

Follow surrounding C style: four-space indentation in H750 code and tabs in legacy MSPM0 files. Do not reformat unrelated files. Use `snake_case` for new functions and variables, `UPPER_CASE` for macros, and preserve SysConfig/CubeMX peripheral identifiers. Document units for calibration values.

**Generated files**: `ti_msp_dl_config.c/.h` and `.syscfg` are generated by TI SysConfig; `Core/Src/gpio.c`, `i2c.c`, `usart.c` etc. by STM32CubeMX. Do not hand-edit these — changes are lost on regeneration. Modify the `.syscfg` or `.ioc` instead, or put custom logic in application files.

**Build-file trap**: The MSPM0 vehicle Keil project compiles `keil/motor/motor.c`, NOT the root-level `firmware/mspm0-vehicle/motor.c` or `GW8.c`. The root `motor.c` even references STM32 HAL types and won't compile for MSPM0. Always verify which file a Keil source group actually references before editing.

## Inter-Controller Communication

All links run at **115200 baud, 8N1**, no hardware flow control, no checksums (except WIT IMU).

### CCD → Vehicle (UART0, 2 bytes/frame)
| Byte | Range | Meaning |
|---|---|---|
| 0 | 0–127 | Black-line center pixel (`NOW_black` on vehicle) |
| 1 | 128–255 | Longest black run length + 128 (`NOW_black_cnt` on vehicle, subtract 128) |

### Vehicle → Gimbal (UART0 → UART4, 10-byte frame)
```
CA AC | int32 encoder1 (LE) | uint8 steering_state | uint8 turn_count | 12 23
```

### MaixCAM → Gimbal (UART1, 24-byte frame)
```
AA 12 | int32 target_x | int32 target_y | int32 laser_x | int32 laser_y | int32 radius | 89 78
```
No length/checksum fields — verify frame delimiters and byte order first.

### WIT IMU → Gimbal (UART6, 11-byte frames)
WIT JY901/WT61 protocol: `0x55` + datatype + 8 data bytes + checksum. Parsed by `User/wit.c`.

### Gimbal → Stepper Drivers (UART2 X-axis, UART3 Y-axis)
Zhang Datou protocol (`User/Zhangdatou.c`). `User/TMC2209.c` exists but `tms2209_init()` is commented out — Zhang Datou is the active driver.

## Laser Safety

The gimbal controls a laser on **PB6** via `laser_enable()`. The laser is **OFF** at power-up and enabled only 500 ms after target acquisition. During bench testing of motor direction, encoders, serial links, or PID, keep the laser **physically disabled or aimed at a controlled backstop**. Never point at people or reflective surfaces.

## Line Sensor Module (8-way Infrared)

`firmware/mspm0-vehicle/line_sensor.c/.h` provides an I2C driver (address `0x12`, data register `0x30`) for a Yahboom 8-channel smart line sensor. `line_sensor_test.c` is a standalone OLED-based diagnostic. This is distinct from the legacy `GW8.c` (STM32 HAL, not in the MSPM0 build).

The module needs **hardware calibration** in its final mounting position:
1. Power on, wait 20+ seconds
2. Long-press KEY1 until red LED stays on
3. All probes on black line for 3s, short-press KEY1
4. All probes on white surface for 3s, short-press KEY1
5. Red LED off = success; slow blink = retry

Reference implementations and datasheets live in `八路巡线红外传感器/`.

## Reference Materials (docs/)

`docs/` contains a PDF knowledge base with 97 indexed PDFs (91 unique, 6 duplicates). Entry points:
- `docs/索引/PDF总索引.md` — master index
- `docs/索引/开发板.md` — dev board datasheets (20 PDFs)
- `docs/索引/芯片.md` — chip datasheets (27 PDFs)
- `docs/索引/模块.md` — module specs (36 PDFs)
- `docs/索引/比赛规则.md` — competition rules (57 PDFs)
- `docs/分析/资料审计.md` — collection audit with gaps identified

Rebuild indexes: `python docs/tools/build_pdf_index.py` (requires `pypdf`). CSV uses UTF-8 BOM.

Key firmware docs:
- `docs/固件中文阅读与调试指南.md` — three-controller debug walkthrough with UART frame formats
- `docs/八路巡线模块接入与测试.md` — line sensor wiring and CCS import guide

## Testing Guidelines

There is no host-side test framework. Verify changes in stages: CCD serial output, motor direction and encoder sign, stationary gimbal aiming, then full vehicle motion. Record board, firmware revision, supply voltage, and calibration inputs. Test the laser only against a controlled target with appropriate eye safety.

**Recommended bring-up order** (change ONE parameter group at a time):
1. Flash each board separately; record firmware rev, supply voltage, and changed parameters.
2. **CCD only**: verify continuous 2-byte UART output; center byte tracks black line position.
3. **Vehicle only**: lift drive wheels, verify `encoder1_num`/`encoder2_num` direction agrees with `motor_a`/`motor_b`; fix sign before tuning PID.
4. **Vehicle + Gimbal**: confirm gimbal UART4 `enconter`, `turn`, `turn_count` update with vehicle motion.
5. **Gimbal steppers only** (laser off): test axis directions at low speed, then zero point and limits.
6. **WIT IMU**: slowly rotate gimbal/vehicle, confirm `wit_data.yaw` is continuous across ±180°.
7. **MaixCAM**: verify UART1 frame and coordinates, then scan/track, then finally enable laser.

## Commit & Pull Request Guidelines

Use concise conventional commits, for example `fix: clamp gimbal scan range`. Keep each commit focused. PRs should state affected controller(s), hardware assumptions, build result, bench-test evidence, and changed pin, UART, or calibration values. Include photos or serial logs for behavior changes.
