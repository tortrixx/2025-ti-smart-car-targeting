# CCS project: CCD controller

Import `adc12_single_conversion_LP_MSPM0G3507_nortos_ticlang.projectspec` with **File > Import Project(s)** in Code Composer Studio.

The project uses TI Arm Clang, the onboard XDS110 debugger, SysConfig, and the installed MSPM0 SDK. Source files are linked to this repository so edits made in CCS update the repository files.

Validated locally with MSPM0 SDK 2.10.00.04 and SysConfig 1.26.2. The existing CCD SysConfig selects the MSPM0G3507 LQFP-48 package; verify that this matches the actual CCD controller hardware before flashing a standard 64-pin LP-MSPM0G3507.
