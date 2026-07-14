# Repository Guidelines

## Project Structure & Module Organization

This repository contains firmware for a three-controller targeting car. `firmware/mspm0-vehicle/` is the MSPM0G3507 vehicle controller; `firmware/mspm0-ccd/` samples the 128-pixel line sensor; and `firmware/stm32h750-gimbal/` controls the STM32H750 gimbal. Reference documents, analyses, and the PDF-index utility live in `docs/`.

Keep application logic in existing source folders such as `Core/Src`, `User`, and `keil/motor`. Treat `build/`, `Objects/`, `.axf`, and `.hex` as generated artifacts; they are ignored by Git.

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

The MSPM0 projects require `mspm0_sdk@2.04.00.06`; preserve `.syscfg` pin assignments unless hardware changes. The H750 project can also open in `firmware/stm32h750-gimbal/MDK-ARM/TMC2209.uvprojx` and targets STM32CubeH7 1.12.1.

## Coding Style & Naming Conventions

Follow surrounding C style: four-space indentation in H750 code and tabs in legacy MSPM0 files. Do not reformat unrelated files. Use `snake_case` for new functions and variables, `UPPER_CASE` for macros, and preserve SysConfig/CubeMX peripheral identifiers. Document units for calibration values.

## Testing Guidelines

There is no host-side test framework. Verify changes in stages: CCD serial output, motor direction and encoder sign, stationary gimbal aiming, then full vehicle motion. Record board, firmware revision, supply voltage, and calibration inputs. Test the laser only against a controlled target with appropriate eye safety.

## Commit & Pull Request Guidelines

Use concise conventional commits, for example `fix: clamp gimbal scan range`. Keep each commit focused. PRs should state affected controller(s), hardware assumptions, build result, bench-test evidence, and changed pin, UART, or calibration values. Include photos or serial logs for behavior changes.
