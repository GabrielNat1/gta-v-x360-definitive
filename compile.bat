@echo off
cd %~dp0
:start
echo SC-CL example.c
set INPUT=
set /P INPUT=Press ENTER to compile or I to compile and inject %=%
If /I "%INPUT%"=="i" goto inject
cls
"../../bin/SC-CL.exe" -platform=X360 -target=GTAV -out-dir="GTAV/PS3/bin/" example.c ../../include/common.c -- -I "../../include/"
goto start
:inject
cls
"../../bin/SC-CL.exe" -platform=X360 -target=GTAV -out-dir="GTAV/PS3/bin/" -name="example" example.c ../../include/common.c -- -I "../../include/"
"../../bin/Script Injector.exe" -in="GTAV/PS3/bin/example.xsc" -i-c=
goto start