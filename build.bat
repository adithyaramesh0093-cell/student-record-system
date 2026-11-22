@echo off
rem ---------------------------------------------------------------
rem  Windows build script (needs gcc from MinGW-w64 / MSYS2 / WinLibs)
rem
rem    build.bat          compile the program   -> build\student.exe
rem    build.bat run      compile and start it
rem    build.bat test     compile and run the automated tests
rem ---------------------------------------------------------------
setlocal enabledelayedexpansion
cd /d "%~dp0"

where gcc >nul 2>nul
if errorlevel 1 goto :no_gcc

if not exist build mkdir build

rem Every .c file in src\ except main.c (so new files are picked up automatically)
set "LIB="
for %%f in (src\*.c) do (
    if /i not "%%~nxf"=="main.c" set "LIB=!LIB! %%f"
)

set "FLAGS=-std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude"

if /i "%~1"=="test" goto :test

gcc %FLAGS% src\main.c %LIB% -o build\student.exe
if errorlevel 1 goto :failed
echo Build OK: build\student.exe
if /i "%~1"=="run" build\student.exe
exit /b 0

:test
gcc %FLAGS% tests\test_main.c %LIB% -o build\run_tests.exe
if errorlevel 1 goto :failed
build\run_tests.exe
exit /b %errorlevel%

:no_gcc
echo gcc was not found. Install MinGW-w64 (for example from https://winlibs.com
echo or via MSYS2), then open a NEW terminal and check that "gcc --version" works.
exit /b 1

:failed
echo Build failed - see the messages above.
exit /b 1
