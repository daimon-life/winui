@echo off


rem duilib
if exist duilib_src rd /s /q duilib_src
xcopy /e .\duilib                                           .\duilib_src\

rem TIMSDK
if exist duilib_tim rd /s /q duilib_tim
xcopy /e .\TIMSDK\cross-platform\Windows\IMApp\Basic\duilib .\duilib_tim\

rem NIM_Duilib_Framework
if exist duilib_nim rd /s /q duilib_nim
xcopy /e .\NIM_Duilib_Framework\duilib                      .\duilib_nim\

rem DuiLib_Ultimate
if exist duilib_qdtroy rd /s /q duilib_src
xcopy /e .\DuiLib_Ultimate                                  .\duilib_qdtroy\

