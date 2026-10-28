@echo off
setlocal
cd /d "%~dp0"

if not exist "aquacore8_sim.exe" (
    echo Binary aquacore8_sim.exe not found. Building first...
    call build.bat --no-pause
)

set ARG=%1
if /i "%ARG%"=="test" goto opt_tests_cli
if /i "%ARG%"=="batch" goto opt_batch_cli
if /i "%ARG%"=="cli" goto opt_c_sim_cli
if /i "%ARG%"=="sim" goto opt_c_sim_cli

:menu
cls
echo ===============================================================================
echo        AquaCore-8: Automated Dishwasher Microprocessor Simulator Suite
echo        Department of CSE ^| East West University (EWU)
echo        Course: CSE360 Computer Architecture
echo ===============================================================================
echo   Project Group Members:
echo     * Nusrat Rahman Aurna (ID: 2023-3-60-057, 2023-3-60-057@std.ewubd.edu)
echo     * Pulok Akibuzzaman   (ID: 2023-3-60-051, 2023-3-60-051@std.ewubd.edu)
echo     * Md. Jakaria Masud   (ID: 2023-3-60-386, 2023-3-60-386@std.ewubd.edu)
echo ===============================================================================
echo.
echo   Please select an option to execute:
echo.
echo     [1] Run Automated Verification Tests (37/37 Unit & Safety Tests in C)
echo     [2] Launch Interactive C Terminal Simulator (Real-Time Monitor)
echo     [3] Run Full Dishwasher Wash Cycle Batch Trace (Trace to Console)
echo     [4] Open Official Academic Project Report (docs\Project_Report_CSE360.docx)
echo     [5] Open Full Project Executive Summary (docs\Full_Project_Summary.docx)
echo     [6] Recompile C Simulator (build.bat)
echo     [0] Exit
echo.
echo ===============================================================================
set /p "CHOICE=Enter your choice (0-6): "

if "%CHOICE%"=="1" goto opt_tests
if "%CHOICE%"=="2" goto opt_c_sim
if "%CHOICE%"=="3" goto opt_batch
if "%CHOICE%"=="4" goto opt_report
if "%CHOICE%"=="5" goto opt_eli5
if "%CHOICE%"=="6" goto opt_rebuild
if "%CHOICE%"=="0" goto opt_exit

echo [!] Invalid selection. Please enter a number between 0 and 6.
timeout /t 2 >nul
goto menu

:opt_tests
cls
aquacore8_sim.exe --test
echo.
echo Press any key to return to menu...
pause >nul
goto menu

:opt_tests_cli
aquacore8_sim.exe --test
goto end

:opt_c_sim
cls
aquacore8_sim.exe
echo.
echo Press any key to return to menu...
pause >nul
goto menu

:opt_c_sim_cli
aquacore8_sim.exe
goto end

:opt_batch
cls
aquacore8_sim.exe --batch
echo.
echo Press any key to return to menu...
pause >nul
goto menu

:opt_batch_cli
aquacore8_sim.exe --batch
goto end

:opt_report
cls
echo Opening Project_Report_CSE360.docx...
if exist "docs\Project_Report_CSE360.docx" (
    start "" "docs\Project_Report_CSE360.docx"
) else (
    start "" "docs\Project_Report_CSE360.html"
)
goto menu

:opt_eli5
cls
echo Opening Full_Project_Summary.docx...
if exist "docs\Full_Project_Summary.docx" (
    start "" "docs\Full_Project_Summary.docx"
) else (
    start "" "docs\Full_Project_Summary.md"
)
goto menu

:opt_rebuild
cls
echo Recompiling C Simulator...
call build.bat --no-pause
echo.
echo Press any key to return to menu...
pause >nul
goto menu

:opt_exit
echo Thank you for using AquaCore-8 Simulator. Goodbye!
exit /b 0

:end
