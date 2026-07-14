# 2025 TI Smart Car Targeting

Firmware and reference material for reproducing a control-focused targeting car.

## Layout

- `firmware/mspm0-vehicle/` — MSPM0G3507 vehicle drive, encoders, and navigation.
- `firmware/mspm0-ccd/` — MSPM0G3507 128-pixel line-scan acquisition and solution.
- `firmware/stm32h750-gimbal/` — STM32H750 gimbal, stepper, laser, IMU, and vision link.
- `docs/` — reference PDFs, analyses, and the PDF-index tool.

## Getting Started

Start with the controller relevant to the hardware being brought up. The two MSPM0 projects use Keil; the gimbal supports CMake presets and a Keil project. See [AGENTS.md](AGENTS.md) for exact project paths, SDK versions, and bench-test order.

Do not commit build outputs or IDE session files. Hardware pinouts and calibration are board-specific; review the relevant `.syscfg` or `.ioc` file before flashing.
