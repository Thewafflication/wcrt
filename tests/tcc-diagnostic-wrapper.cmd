@echo off
setlocal EnableExtensions DisableDelayedExpansion

if not defined WCRT_TEST_TINYCC (
    echo WCRT_TEST_TINYCC does not identify the underlying compiler. 1>&2
    exit /b 2
)

set "wcrt_add_diagnostics=0"
for %%A in (%*) do (
    if /I "%%~xA"==".exe" set "wcrt_add_diagnostics=1"
)
for %%A in (%*) do (
    if /I "%%~A"=="-c" set "wcrt_add_diagnostics=0"
    if /I "%%~A"=="-E" set "wcrt_add_diagnostics=0"
    if /I "%%~A"=="-r" set "wcrt_add_diagnostics=0"
    if /I "%%~A"=="-ar" set "wcrt_add_diagnostics=0"
    if /I "%%~A"=="-run" set "wcrt_add_diagnostics=0"
    if /I "%%~A"=="-shared" set "wcrt_add_diagnostics=0"
    if /I "%%~A"=="-impdef" set "wcrt_add_diagnostics=0"
    if /I "%%~A"=="-nostdlib" set "wcrt_add_diagnostics=0"
)

if "%wcrt_add_diagnostics%"=="1" (
    "%WCRT_TEST_TINYCC%" -g -bt30 %*
) else (
    "%WCRT_TEST_TINYCC%" %*
)
exit /b %ERRORLEVEL%
