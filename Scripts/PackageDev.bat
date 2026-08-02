@echo off
REM ============================================================
REM  WILDBOUND - Development Packaging (Client + Dedicated Server)
REM
REM  UAT BuildCookRun wrapper. Reused for the weekly packaging smoke
REM  test from W3 onward.
REM
REM  Output : PackagedBuilds\WindowsClient , PackagedBuilds\WindowsServer
REM  Log    : Saved\PackageLogs\PackageDev-<timestamp>.log
REM
REM  Engine path can be overridden with the UE_ROOT env var.
REM
REM  NOTE: This file must stay ASCII-only with CRLF line endings.
REM        cmd.exe reads .bat in the system ANSI codepage, so UTF-8
REM        multibyte text corrupts the parser. See .gitattributes.
REM ============================================================
setlocal

if "%UE_ROOT%"=="" set UE_ROOT=C:\UE58

set ROOT=%~dp0..
set PROJECT=%ROOT%\Wildbound.uproject
set UAT=%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat
set ARCHIVE=%ROOT%\PackagedBuilds
set LOGDIR=%ROOT%\Saved\PackageLogs

if not exist "%PROJECT%" (
    echo [ERROR] uproject not found: %PROJECT%
    exit /b 1
)
if not exist "%UAT%" (
    echo [ERROR] RunUAT.bat not found: %UAT%
    echo         Set the UE_ROOT env var if the engine lives elsewhere.
    exit /b 1
)
if not exist "%LOGDIR%" mkdir "%LOGDIR%"

for /f %%I in ('powershell -NoProfile -Command "Get-Date -Format yyyyMMdd-HHmmss"') do set STAMP=%%I
set LOG=%LOGDIR%\PackageDev-%STAMP%.log

echo ============================================================
echo  WILDBOUND Development Packaging
echo  start   : %DATE% %TIME%
echo  project : %PROJECT%
echo  engine  : %UE_ROOT%
echo  archive : %ARCHIVE%
echo  log     : %LOG%
echo ============================================================
echo.
echo  Tail the log from another window with:
echo    powershell -Command "Get-Content -Wait -Tail 30 '%LOG%'"
echo.

call "%UAT%" BuildCookRun -project="%PROJECT%" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -server -cook -build -stage -pak -archive -archivedirectory="%ARCHIVE%" -utf8output > "%LOG%" 2>&1

set RESULT=%ERRORLEVEL%

echo.
echo ============================================================
echo  end   : %DATE% %TIME%
echo  exit  : %RESULT%
if "%RESULT%"=="0" (
    echo  RESULT: SUCCESS
    echo  Output: %ARCHIVE%
) else (
    echo  RESULT: FAILED
    echo  Check the log: %LOG%
    echo  Search for "Error:" or "LogCook: Error"
)
echo ============================================================

endlocal
exit /b %RESULT%
