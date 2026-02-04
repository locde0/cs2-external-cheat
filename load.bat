@echo off
setlocal
title kmd loader
color 07

openfiles >nul 2>&1
if %errorlevel% NEQ 0 (
    echo [!] requesting admin privileges..
    powershell -Command "Start-Process '%~dpnx0' -Verb RunAs"
    exit /b
)

set "ROOT_DIR=%~dp0"
set "DRIVER_PATH=%ROOT_DIR%build\kmd.sys"
set "KDMAPPER_PATH=%ROOT_DIR%build\kdmapper.exe"
set "CLIENT_PATH=%ROOT_DIR%build\client.exe"

if not exist "%KDMAPPER_PATH%" (
    color 0C
    echo [-] error: kdmapper.exe not found at:
    echo "%KDMAPPER_PATH%"
    echo.
    echo please build project!
    pause
    exit /b
)

if not exist "%DRIVER_PATH%" (
    color 0C
    echo [-] error: kmd.sys not found at:
    echo "%DRIVER_PATH%"
    echo.
    echo please build project!
    pause
    exit /b
)

if not exist "%CLIENT_PATH%" (
    color 0C
    echo [-] error: client.exe not found at:
    echo "%CLIENT_PATH%"
    echo.
    echo please build project!
    pause
    exit /b
)

cls
echo [>] found kdmapper: %KDMAPPER_PATH%
echo [>] found driver: %DRIVER_PATH%
echo [>] found client: %CLIENT_PATH%
echo.
echo [!] attempting to load driver..
echo.

"%KDMAPPER_PATH%" "%DRIVER_PATH%"

if %errorlevel% equ 0 (
    echo.
    echo [+] driver loaded successfully.
    echo [>] starting client..
    
    timeout /t 1 /nobreak >nul
    
    "%CLIENT_PATH%"
) else (
    echo.
    echo [-] loader failed with error code %errorlevel%.
)

echo.
echo [!] client process terminated.
pause