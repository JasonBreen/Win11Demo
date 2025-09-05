@echo off
setlocal
set SRC=intro.cpp
set OUT=intro.exe
set LIBS=user32.lib gdi32.lib opengl32.lib winmm.lib
set CLFLAGS=/EHsc /O2 /MT /GL /GS- /DNDEBUG
cl %CLFLAGS% %SRC% /link %LIBS% /SUBSYSTEM:WINDOWS /OUT:%OUT%
endlocal
