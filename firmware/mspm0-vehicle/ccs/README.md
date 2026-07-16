# CCS project: vehicle controller

Two CCS projects are provided:

- `empty_LP_MSPM0G3507_nortos_ticlang.projectspec`: full vehicle controller.
- `line_sensor_test_LP_MSPM0G3507_nortos_ticlang.projectspec`: safe eight-channel line-sensor and OLED diagnostic. It does not configure or start the motor pins.

Import the required `.projectspec` with **File > Import Project(s)** in Code Composer Studio.

The project uses TI Arm Clang, the onboard XDS110 debugger, SysConfig, and the installed MSPM0 SDK. Source files are linked to this repository so edits made in CCS update the repository files.

Validated locally with MSPM0 SDK 2.10.00.04 and SysConfig 1.26.2. The original configuration was generated with MSPM0 SDK 2.04.00.06.

See [`docs/八路巡线模块接入与测试.md`](../../../docs/八路巡线模块接入与测试.md) before connecting the sensor or OLED. The document covers the PA15/PA16 I2C assignment and the J15, J17, and J18 jumper requirements.
