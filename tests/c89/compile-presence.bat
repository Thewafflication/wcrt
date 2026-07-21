@echo off
setlocal

where tcc >nul 2>nul
if errorlevel 1 (
    echo error: tcc was not found in PATH
    exit /b 1
)

if not exist build\tests\c89\presence mkdir build\tests\c89\presence
if errorlevel 1 exit /b 1

set WCRT_FAILED=0
for %%F in (tests\c89\presence\*.c) do call :compile "%%F"

if not "%WCRT_FAILED%"=="0" (
    echo C89 API presence tests failed.
    exit /b 1
)

echo C89 API presence tests passed.
exit /b 0

:compile
echo Compiling %~1
tcc -std=c89 -Werror -nostdinc -Iinclude -c %1 -o build\tests\c89\presence\%~n1.o
if errorlevel 1 set WCRT_FAILED=1
exit /b 0
