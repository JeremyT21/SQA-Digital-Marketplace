@echo off
setlocal

rem used to figure out how to get the path to reference from the directory this .bat is run from: https://stackoverflow.com/questions/18309941/what-does-it-mean-by-command-cd-d-dp0-in-windows

cd /d "%~dp0"

if not exist "actual-outputs" mkdir "actual-outputs"
if not exist "actual-outputs" exit /b 1

rem used the following to implement using errorlevel: https://stackoverflow.com/questions/24866477/if-call-exit-and-errorlevel-in-a-bat

where cl >nul 2>nul

if errorlevel 1
(
    echo run this file from the visual studio developer command prompt
    exit /b 1
)

cl /nologo /EHsc /std:c++17 /Fo"actual-outputs\\" /Fe"actual-outputs\marketplace.exe" ..\code\DigitalMarketplace\main.cpp ..\code\DigitalMarketplace\front.cpp ..\code\DigitalMarketplace\market.cpp ..\code\DigitalMarketplace\user.cpp > "actual-outputs\build.txt" 2>&1
if errorlevel 1
(
    echo build failed see actual-outputs\build.txt
    exit /b 1
)

set failedtests=0

call :runtest login-user-validation\success-route
call :runtest login-user-validation\failure-route
call :runtest logout-requires-login\success-route
call :runtest logout-requires-login\failure-route
call :runtest login-blocked-if-logged-in\failure-route

echo completed five tests
echo process failures %failedtests%

if not "%failedtests%"=="0" exit /b 1
exit /b 0

:runtest

set testpath=input-output-for-tests\account-functionality\%1

set outputpath=actual-outputs\account-functionality\%1

if not exist "%outputpath%" mkdir "%outputpath%"
if not exist "%outputpath%"
(
    set /a failedtests+=1
    exit /b 1
)

echo running %1

type nul > "%outputpath%\daily_transaction_file.txt"
"actual-outputs\marketplace.exe" "%testpath%\current_user_accounts_file.txt" "%testpath%\available_games_file.txt" "%testpath%\game_collection_file.txt" "%outputpath%\daily_transaction_file.txt" < "%testpath%\input.txt" > "%outputpath%\output.txt" 2> "%outputpath%\errors.txt"

set testexitcode=%errorlevel%
> "%outputpath%\exit_code.txt" echo %testexitcode%

if not "%testexitcode%"=="0"
(
    echo process failed %1
    set /a failedtests+=1
)

exit /b 0