@echo off
setlocal
cd /d "%~dp0"

if not exist "aquacore8_sim.exe" (
    call build.bat --no-pause
)

aquacore8_sim.exe
echo.
pause
