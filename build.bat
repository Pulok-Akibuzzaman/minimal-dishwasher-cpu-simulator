@echo off
setlocal
cd /d "%~dp0"

echo ===============================================================================
echo            AquaCore-8: 8-Bit Dishwasher CPU Architecture (C Engine)
echo            Department of CSE ^| East West University (EWU)
echo            Course: CSE360 Computer Architecture
echo ===============================================================================

set GCC_PATH=gcc
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    if exist "%USERPROFILE%\scoop\apps\gcc\current\bin\gcc.exe" (
        set "GCC_PATH=%USERPROFILE%\scoop\apps\gcc\current\bin\gcc.exe"
    ) else if exist "%USERPROFILE%\scoop\apps\tcc\0.9.27\tcc\tcc.exe" (
        set "GCC_PATH=%USERPROFILE%\scoop\apps\tcc\0.9.27\tcc\tcc.exe"
    )
)

echo [1/2] Compiling C source files using %GCC_PATH%...
"%GCC_PATH%" -O2 -Wall -I src src\aquacore8.c src\tests.c src\main.c -o aquacore8_sim.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] C compilation failed!
    exit /b 1
)
echo [OK] Compiled successfully: aquacore8_sim.exe

echo.
echo [2/2] Running Automated Verification Test Suite (Unit + Integration)...
echo -------------------------------------------------------------------------------
aquacore8_sim.exe --test
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Tests failed!
    exit /b 1
)

echo.
echo ===============================================================================
echo            ALL 37 TESTS PASSED! C IMPLEMENTATION VERIFIED 100%%
echo ===============================================================================
echo.
echo Quick commands:
echo   run.bat          - Launch interactive menu
echo   run.bat test     - Run verification test suite
echo   run.bat batch    - Run full dishwasher cycle trace
echo   run.bat cli      - Launch interactive console monitor
echo.

if "%1"=="--no-pause" goto end
pause
:end
