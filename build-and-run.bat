@echo off

cmake --build build

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b %errorlevel%
)

build\Debug\redis_from_scratch.exe

pause