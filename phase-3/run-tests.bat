@echo off
setlocal

rem used to figure out how to get the path to reference from the directory this .bat is run from: https://stackoverflow.com/questions/18309941/what-does-it-mean-by-command-cd-d-dp0-in-windows

cd /d "%~dp0"

rem prepare output files

if not exist "actual-outputs" (
    mkdir "actual-outputs"
)

if not exist "actual-outputs" (
    exit /b 1
)

rem build from a visual studio developer command prompt

rem used the following to implement using errorlevel: https://stackoverflow.com/questions/24866477/if-call-exit-and-errorlevel-in-a-bat

where cl >nul 2>nul

if errorlevel 1 (
    goto compiler_missing
)

rem compiles .obj and .exe files and saves messages into the build.txt file
rem used https://stackoverflow.com/questions/62915524/specify-command-line-c-version-for-cl-exe-visual-studio-code

cl /nologo /EHsc /std:c++17 /Fo"actual-outputs\\" /Fe"actual-outputs\marketplace.exe" ..\code\DigitalMarketplace\main.cpp ..\code\DigitalMarketplace\front.cpp ..\code\DigitalMarketplace\market.cpp ..\code\DigitalMarketplace\user.cpp > "actual-outputs\build.txt" 2>&1

if errorlevel 1 (
    goto build_failed
)

set tests_run=0
set failed_tests=0
set process_failures=0
set terminal_failures=0
set transaction_failures=0
type nul > "actual-outputs\results.txt"

rem run each phase one case
rem using labels learned from: https://stackoverflow.com/questions/32639462/how-to-use-labels-in-batch

call :run_test account-functionality\add-credit\failure-route\buy-standard
call :run_test account-functionality\add-credit\failure-route\full-standard
call :run_test account-functionality\add-credit\failure-route\sell-standard
call :run_test account-functionality\add-credit\success-route\buy-standard
call :run_test account-functionality\add-credit\success-route\full-standard
call :run_test account-functionality\add-credit\success-route\sell-standard
call :run_test account-functionality\add-credit-limit\failure-route
call :run_test account-functionality\add-credit-limit\success-route
call :run_test account-functionality\admin-add-credit\failure-route
call :run_test account-functionality\admin-add-credit\success-route
call :run_test account-functionality\bad-number-field\failure-route\addcredit
call :run_test account-functionality\bad-number-field\failure-route\create
call :run_test account-functionality\bad-number-field\success-route\addcredit
call :run_test account-functionality\bad-number-field\success-route\create
call :run_test account-functionality\create-admin-only\failure-route
call :run_test account-functionality\create-admin-only\success-route
call :run_test account-functionality\create-and-type\failure-route
call :run_test account-functionality\create-and-type\success-route
call :run_test account-functionality\create-username-length\failure-route
call :run_test account-functionality\create-username-length\success-route
call :run_test account-functionality\create-username-unique\failure-route
call :run_test account-functionality\create-username-unique\success-route
call :run_test account-functionality\credit-overflow\failure-route
call :run_test account-functionality\credit-overflow\success-route
call :run_test account-functionality\delete-admin-only\failure-route
call :run_test account-functionality\delete-admin-only\success-route
call :run_test account-functionality\delete-current-admin\failure-route
call :run_test account-functionality\delete-current-admin\success-route
call :run_test account-functionality\delete-target-check\failure-route\blank-target
call :run_test account-functionality\delete-target-check\failure-route\missing-target
call :run_test account-functionality\delete-target-check\success-route
call :run_test account-functionality\file-reading\failure-route
call :run_test account-functionality\file-reading\success-route
call :run_test account-functionality\invalid-input\failure-route
call :run_test account-functionality\invalid-input\success-route
call :run_test account-functionality\login-blocked-if-logged-in\failure-route
call :run_test account-functionality\login-blocked-if-logged-in\success-route
call :run_test account-functionality\login-required-before-transaction\failure-route
call :run_test account-functionality\login-required-before-transaction\success-route
call :run_test account-functionality\login-user-validation\failure-route
call :run_test account-functionality\login-user-validation\success-route
call :run_test account-functionality\logout-requires-login\failure-route
call :run_test account-functionality\logout-requires-login\success-route
call :run_test account-functionality\session-end\failure-route
call :run_test account-functionality\session-end\success-route
call :run_test buying-functionality\admin-only-refund\failure-route
call :run_test buying-functionality\admin-only-refund\success-route
call :run_test buying-functionality\admin-refund-credits\failure-route
call :run_test buying-functionality\admin-refund-credits\success-route
call :run_test buying-functionality\admin-refund-rebought\failure-route
call :run_test buying-functionality\admin-refund-rebought\success-route
call :run_test buying-functionality\admin-refund-users\failure-route\missing-buyer
call :run_test buying-functionality\admin-refund-users\failure-route\missing-seller
call :run_test buying-functionality\admin-refund-users\success-route
call :run_test buying-functionality\buy-invalid-user\failure-route\missing-buyer
call :run_test buying-functionality\buy-invalid-user\failure-route\missing-seller
call :run_test buying-functionality\buy-invalid-user\success-route
call :run_test buying-functionality\buy-update\failure-route
call :run_test buying-functionality\buy-update\success-route
call :run_test buying-functionality\buyable-games\failure-route
call :run_test buying-functionality\buyable-games\success-route
call :run_test buying-functionality\daily-format-transaction\failure-route\purchase
call :run_test buying-functionality\daily-format-transaction\failure-route\refund
call :run_test buying-functionality\daily-format-transaction\success-route\purchase
call :run_test buying-functionality\daily-format-transaction\success-route\refund
call :run_test buying-functionality\refund-overflow\failure-route
call :run_test buying-functionality\refund-overflow\success-route
call :run_test buying-functionality\seller-not-overflow\failure-route
call :run_test buying-functionality\seller-not-overflow\success-route
call :run_test cart-functionality\deleted-user-transaction\failure-route
call :run_test cart-functionality\deleted-user-transaction\success-route
call :run_test cart-functionality\sell-account-check\failure-route
call :run_test cart-functionality\sell-account-check\success-route\admin
call :run_test cart-functionality\sell-account-check\success-route\full-standard
call :run_test cart-functionality\sell-account-check\success-route\sell-standard
call :run_test cart-functionality\sell-formatting\failure-route
call :run_test cart-functionality\sell-formatting\success-route
call :run_test cart-functionality\sell-game-name-limit\failure-route
call :run_test cart-functionality\sell-game-name-limit\success-route
call :run_test cart-functionality\sell-game-name-unique\failure-route
call :run_test cart-functionality\sell-game-name-unique\success-route
call :run_test cart-functionality\sell-new-locked\failure-route
call :run_test cart-functionality\sell-new-locked\success-route
call :run_test cart-functionality\sell-price-limit\failure-route
call :run_test cart-functionality\sell-price-limit\success-route

echo completed %tests_run% tests
echo failed tests %failed_tests%
echo process failures %process_failures%
echo terminal failures %terminal_failures%
echo transaction failures %transaction_failures%

>> "actual-outputs\results.txt" echo completed %tests_run% tests
>> "actual-outputs\results.txt" echo failed tests %failed_tests%
>> "actual-outputs\results.txt" echo process failures %process_failures%
>> "actual-outputs\results.txt" echo terminal failures %terminal_failures%
>> "actual-outputs\results.txt" echo transaction failures %transaction_failures%

rem call :save_run

if not "%failed_tests%"=="0" (
    exit /b 1
)

exit /b 0

:compiler_missing
echo run this file from a visual studio developer command prompt
exit /b 1

:build_failed
echo build failed see actual-outputs\build.txt
exit /b 1

:run_test
set test_name=%1
set input_directory=input-output-for-tests\%test_name%
set output_directory=actual-outputs\%test_name%
set test_failed=0
set /a tests_run+=1

if not exist "%output_directory%" (
    mkdir "%output_directory%"
)

if not exist "%output_directory%" (
    goto output_failed
)

echo running %test_name%

rem save terminal output and transactions, runs the .exe so it can have test inputs applied

type nul > "%output_directory%\daily_transaction_file.txt"
"actual-outputs\marketplace.exe" "%input_directory%\current_user_accounts_file.txt" "%input_directory%\available_games_file.txt" "%input_directory%\game_collection_file.txt" "%output_directory%\daily_transaction_file.txt" < "%input_directory%\input.txt" > "%output_directory%\output.txt" 2> "%output_directory%\errors.txt"
set exit_code=%errorlevel%
> "%output_directory%\exit_code.txt" echo %exit_code%

if not "%exit_code%"=="0" (
    set /a process_failures+=1
    set test_failed=1
)

rem compare actual terminal output with expected terminal output

fc /b "%input_directory%\output.txt" "%output_directory%\output.txt" > "%output_directory%\terminal_diff.txt" 2>&1
set terminal_exit_code=%errorlevel%

if not "%terminal_exit_code%"=="0" (
    set /a terminal_failures+=1
    set test_failed=1
)

rem compare actual transactions with expected transactions

fc /b "%input_directory%\daily_transaction_file.txt" "%output_directory%\daily_transaction_file.txt" > "%output_directory%\transaction_diff.txt" 2>&1
set transaction_exit_code=%errorlevel%

if not "%transaction_exit_code%"=="0" (
    set /a transaction_failures+=1
    set test_failed=1
)

if "%test_failed%"=="1" (
    goto test_failed
)

echo pass %test_name%
>> "actual-outputs\results.txt" echo pass %test_name%
exit /b 0

:output_failed
echo could not create output directory for %test_name%

:test_failed
set /a failed_tests+=1
echo fail %test_name%
>> "actual-outputs\results.txt" echo fail %test_name%
exit /b 0

rem :save_run

rem keep results from each full run

rem set run_number=1
rem :find_run

rem if not exist "test-runs\run-%run_number%" (
rem     goto copy_run
rem )

rem set /a run_number+=1
rem goto find_run
rem :copy_run

rem to get batch files working needed add switches (e.g. /s), help from: https://stackoverflow.com/questions/9114887/batch-to-copy-files-with-xcopy

rem xcopy "actual-outputs\*.txt" "test-runs\run-%run_number%\" /s /i /y >nul

rem if errorlevel 1 (
rem     echo could not save run history
rem )

rem echo saved run %run_number%
rem exit /b 0
