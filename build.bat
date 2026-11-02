@echo off
title Course Registration and Seat Allocation System - ACP
echo ======================================================================
echo    Compiling Course Registration & Seat Allocation System (C99)...
echo ======================================================================

gcc -Wall -Wextra -std=c99 -o course_system.exe main.c course_system.c

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Compilation finished without errors or warnings!
    echo Launching system...
    echo.
    course_system.exe
) else (
    echo [ERROR] Compilation failed. Please verify GCC is installed and on PATH.
    pause
)
