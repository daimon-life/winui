@echo off


rem duilib
:duilib_src
if not exist _duilib\.git git submodule update --init -- _duilib
if exist _duilib\.git git pull origin
if %errorlevel% NEQ 0 goto endduilib_src
if exist duilib_src rd /s /q duilib_src
xcopy /e .\_duilib                                           .\duilib_src\
if exist duilib_src\.git erase duilib_src\.git
:endduilib_src


rem DuiLib_Ultimate
:duilib_qdtroy
if not exist _DuiLib_Ultimate\.git git submodule update --init -- _DuiLib_Ultimate
if exist _DuiLib_Ultimate\.git git pull origin
if %errorlevel% NEQ 0 goto endduilib_qdtroy
if exist duilib_qdtroy rd /s /q duilib_qdtroy
xcopy /e .\_DuiLib_Ultimate                                  .\duilib_qdtroy\
if exist duilib_qdtroy\.git erase duilib_qdtroy\.git
:endduilib_qdtroy


rem NIM_Duilib_Framework
:duilib_nim
if not exist _NIM_Duilib_Framework\.git git submodule update --init -- _NIM_Duilib_Framework
if exist _NIM_Duilib_Framework\.git git pull origin
if %errorlevel% NEQ 0 goto endduilib_nim
if exist duilib_nim rd /s /q duilib_nim
xcopy /e .\_NIM_Duilib_Framework\duilib                      .\duilib_nim\
if exist duilib_nim\.git erase duilib_nim\.git
:endduilib_nim


rem TIMSDK
:duilib_tim
if not exist _TIMSDK\.git git submodule update --init -- _TIMSDK
if exist _TIMSDK\.git git pull origin
if %errorlevel% NEQ 0 goto endduilib_tim
if exist duilib_tim rd /s /q duilib_tim
xcopy /e .\_TIMSDK\cross-platform\Windows\IMApp\Basic\duilib .\duilib_tim\
if exist duilib_tim\.git erase duilib_tim\.git
:endduilib_tim


rem end
:end
