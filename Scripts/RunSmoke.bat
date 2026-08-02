@echo off
REM ============================================================
REM  WILDBOUND - Packaged build smoke test launcher
REM
REM  Starts the dedicated server and N clients side by side for
REM  the weekly packaging smoke test (W3 onward).
REM
REM  Usage:
REM    RunSmoke.bat                    server + 4 clients
REM    RunSmoke.bat 2                  server + 2 clients
REM    RunSmoke.bat 4 10.0.0.5:7777    connect only, no local server
REM
REM  Logs : <build>\Wildbound\Saved\Logs\WBSmoke-<stamp>-*.log
REM
REM  ---- FIXED MEASUREMENT CONDITIONS -------------------------
REM  Weekly numbers cannot be compared unless these hold:
REM    1. AC power connected. On battery the dGPU sits at P8 and
REM       frame time went 5.07 ms -> 18.02 ms (08.02 measurement).
REM    2. Rider / Chrome / Discord closed. They held ~6.5 GB and
REM       left only 1 GB free while four clients were running.
REM    3. Desktop at 1920x1080. The window layout below assumes it.
REM
REM  ---- NOTES ------------------------------------------------
REM  -DefaultViewportMouseCaptureMode=NoCapture frees the cursor so
REM  you can click between the four windows. Remove it from W3 when
REM  testing mouse-driven combat.
REM
REM  Window position does NOT match join order - clients finish
REM  loading at different speeds. Identify a player by its log file,
REM  not by which quadrant it sits in.
REM
REM  This file must stay ASCII-only with CRLF line endings. cmd.exe
REM  reads .bat in the system ANSI codepage. See .gitattributes.
REM ============================================================
setlocal

set COUNT=%~1
if "%COUNT%"=="" set COUNT=4

set ADDRESS=%~2
set STARTSERVER=1
if not "%ADDRESS%"=="" set STARTSERVER=0
if "%ADDRESS%"=="" set ADDRESS=127.0.0.1:7777

set ROOT=%~dp0..
set CLIENT=%ROOT%\PackagedBuilds\Windows\Wildbound.exe
set SERVER=%ROOT%\PackagedBuilds\WindowsServer\WildboundServer.exe

REM Per-client window size. Four of these tile a 1920x1080 desktop.
set RESX=960
set RESY=540

REM Frame cap. Keeps four uncapped clients from starving each other.
REM Set to 0 to measure single-client performance without a ceiling.
set MAXFPS=60

REM Seconds to wait for the server to bind before clients connect.
set SERVERWAIT=15

REM Seconds between client launches. Staggering avoids disk contention.
set CLIENTSTAGGER=18

if not exist "%CLIENT%" (
    echo [ERROR] Client build not found: %CLIENT%
    echo         Run Scripts\PackageDev.bat first.
    exit /b 1
)
if "%STARTSERVER%"=="1" (
    if not exist "%SERVER%" (
        echo [ERROR] Server build not found: %SERVER%
        echo         Run Scripts\PackageDev.bat first.
        exit /b 1
    )
)

for /f %%I in ('powershell -NoProfile -Command "Get-Date -Format yyyyMMdd-HHmmss"') do set STAMP=%%I
for /f %%I in ('powershell -NoProfile -Command "(Get-CimInstance Win32_Battery).BatteryStatus"') do set BATT=%%I

echo ============================================================
echo  WILDBOUND smoke test
echo  clients : %COUNT%
echo  address : %ADDRESS%
echo  server  : %STARTSERVER%   (1 = start locally)
echo  stamp   : %STAMP%
echo ============================================================
echo.

if "%BATT%"=="1" (
    echo [WARN] This machine is running on battery.
    echo        The GPU drops to P8 and frame time roughly triples.
    echo        Plug in AC power before recording any measurement.
    echo.
)

echo Closing leftover processes from a previous run...
taskkill /IM WildboundServer.exe /F >nul 2>&1
taskkill /IM Wildbound.exe /F >nul 2>&1
timeout /t 3 /nobreak >nul

if "%STARTSERVER%"=="1" (
    echo Starting dedicated server...
    start "" "%SERVER%" L_Lobby -log -LOG=WBSmoke-%STAMP%-Server.log
    echo Waiting %SERVERWAIT%s for the server to bind port 7777...
    timeout /t %SERVERWAIT% /nobreak >nul
) else (
    echo Skipping local server - connecting to %ADDRESS%
)

for /L %%I in (1,1,%COUNT%) do call :launch %%I

echo.
echo ============================================================
echo  All %COUNT% client(s) launched.
echo.
echo  Console key is the tilde. Useful commands:
echo    WBSetProbe 42 / WBShowProbe   travel data survival
echo    WBTravelRaid / WBTravelLobby  level transition
echo    stat unit / stat net          performance
echo.
echo  Server log:
echo    PackagedBuilds\WindowsServer\Wildbound\Saved\Logs\WBSmoke-%STAMP%-Server.log
echo  Client logs:
echo    PackagedBuilds\Windows\Wildbound\Saved\Logs\WBSmoke-%STAMP%-ClientN.log
echo ============================================================

endlocal
exit /b 0

REM ------------------------------------------------------------
REM  :launch <index>   place one client in its screen quadrant
REM ------------------------------------------------------------
:launch
set IDX=%~1
set "X=0"
set "Y=0"
if "%IDX%"=="2" ( set "X=960" & set "Y=0" )
if "%IDX%"=="3" ( set "X=0" & set "Y=540" )
if "%IDX%"=="4" ( set "X=960" & set "Y=540" )

echo Launching client %IDX% at %X%,%Y% ...
start "" "%CLIENT%" %ADDRESS% -windowed -ResX=%RESX% -ResY=%RESY% -WinX=%X% -WinY=%Y% -DefaultViewportMouseCaptureMode=NoCapture -ExecCmds="t.MaxFPS %MAXFPS%" -LOG=WBSmoke-%STAMP%-Client%IDX%.log
timeout /t %CLIENTSTAGGER% /nobreak >nul
goto :eof
