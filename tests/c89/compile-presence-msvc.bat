@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "WCRT_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%WCRT_VSWHERE%" (
    echo error: vswhere.exe was not found
    exit /b 1
)

set "WCRT_VSROOT="
for /f "usebackq delims=" %%I in (`"%WCRT_VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "WCRT_VSROOT=%%I"
if not defined WCRT_VSROOT (
    echo error: a Visual Studio C toolchain was not found
    exit /b 1
)

call "%WCRT_VSROOT%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1

set "WCRT_FAILED=0"
for %%F in (tests\c89\presence\*.c) do call :compile "%%F"

if not "%WCRT_FAILED%"=="0" (
    echo Microsoft CRT API presence comparison failed.
    exit /b 1
)

echo Microsoft CRT API presence comparison passed.
exit /b 0

:compile
echo Compiling %~1
cl /nologo /TC /Zs /W3 /WX /D_CRT_SECURE_NO_WARNINGS %1
if errorlevel 1 set "WCRT_FAILED=1"
exit /b 0
