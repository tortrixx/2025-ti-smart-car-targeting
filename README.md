# 2025 TI Smart Car Targeting

Firmware and reference material for reproducing a control-focused targeting car.

This branch also contains the standalone 2025 GPIO eight-channel line-following car. Its hardware connection, control algorithm, tuning procedure, CCS build, and DSLite flashing instructions are in [docs/2025巡线车固件与调参手册.md](docs/2025巡线车固件与调参手册.md).

## Layout

- `firmware/mspm0-vehicle/` — MSPM0G3507 vehicle controller; on this branch it is also the 2025 GPIO line-following-car main controller.
- `firmware/mspm0-ccd/` — MSPM0G3507 128-pixel line-scan acquisition and solution.
- `firmware/stm32h750-gimbal/` — STM32H750 gimbal, stepper, laser, IMU, and vision link.
- `docs/` — reference PDFs, analyses, and the PDF-index tool.

## Getting Started

Start with the controller relevant to the hardware being brought up. The two MSPM0 projects use Keil; the gimbal supports CMake presets and a Keil project. See [AGENTS.md](AGENTS.md) for exact project paths, SDK versions, and bench-test order.

Do not commit build outputs or IDE session files. Hardware pinouts and calibration are board-specific; review the relevant `.syscfg` or `.ioc` file before flashing.
