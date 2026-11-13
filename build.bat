@echo off
setlocal
cd /d "%~dp0"

echo ===============================================================================
echo            AquaCore-8: 8-Bit Dishwasher CPU Simulator (Minimal C)
echo            Department of CSE ^| East West University (EWU)
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

echo Compiling main.c with %GCC_PATH%...
"%GCC_PATH%" -O2 -Wall main.c -o aquacore8_sim.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed!
    exit /b 1
)
echo [OK] Compiled successfully: aquacore8_sim.exe

if "%1"=="--no-pause" goto end
pause
:end
