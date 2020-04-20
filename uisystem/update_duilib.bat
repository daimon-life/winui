@echo off

rem duilib_c
:duilib_c
if exist duilib_c rd /s /q duilib_c
xcopy /e .\_DuiLib_c\DuiLib                                  .\duilib_c\
:endduilib_c

rem duilib_faw
:duilib_faw
if exist duilib_faw rd /s /q duilib_faw
xcopy /e .\_DuiLib_Faw\DuiLib                                .\duilib_faw\
:endduilib_faw
goto end

rem duilib
:duilib_src
if not exist _duilib\.git git submodule update --init -- _duilib
if exist _duilib\.git git pull origin
if %errorlevel% NEQ 0 goto endduilib_src
xcopy /f /y duilib_src\duilib_src.* ..\template\
if exist duilib_src rd /s /q duilib_src
xcopy /e .\_duilib\DuiLib                                    .\duilib_src\
if exist duilib_src\.git erase duilib_src\.git
xcopy /f /y ..\template\duilib_src.* .\duilib_src\
:endduilib_src


rem DuiLib_Ultimate
:duilib_qdtroy
if not exist _DuiLib_Ultimate\.git git submodule update --init -- _DuiLib_Ultimate
if exist _DuiLib_Ultimate\.git git pull origin
if %errorlevel% NEQ 0 goto endduilib_qdtroy
xcopy /f /y duilib_qdtroy\duilib_qdtroy.* ..\template\
if exist duilib_qdtroy rd /s /q duilib_qdtroy
xcopy /e .\_DuiLib_Ultimate\DuiLib                           .\duilib_qdtroy\
if exist duilib_qdtroy\.git erase duilib_qdtroy\.git
xcopy /f /y ..\template\duilib_qdtroy.* .\duilib_qdtroy\
:endduilib_qdtroy


rem NIM_Duilib_Framework
:duilib_nim
if not exist _NIM_Duilib_Framework\.git git submodule update --init -- _NIM_Duilib_Framework
if exist _NIM_Duilib_Framework\.git git pull origin
if %errorlevel% NEQ 0 goto endduilib_nim
xcopy /f /y duilib_nim\duilib_nim.* ..\template\
if exist duilib_nim rd /s /q duilib_nim
xcopy /e .\_NIM_Duilib_Framework\duilib                      .\duilib_nim\
if exist duilib_nim\.git erase duilib_nim\.git
xcopy /f /y ..\template\duilib_nim.* .\duilib_nim\
:endduilib_nim


rem TIMSDK
:duilib_tim
if not exist _TIMSDK\.git git submodule update --init -- _TIMSDK
if exist _TIMSDK\.git git pull origin
if %errorlevel% NEQ 0 goto endduilib_tim
xcopy /f /y duilib_tim\duilib_tim.* ..\template\
if exist duilib_tim rd /s /q duilib_tim
xcopy /e .\_TIMSDK\cross-platform\Windows\IMApp\Basic\duilib .\duilib_tim\
if exist duilib_tim\.git erase duilib_tim\.git
xcopy /f /y ..\template\duilib_tim.* .\duilib_tim\
:endduilib_tim


rem end
:end
