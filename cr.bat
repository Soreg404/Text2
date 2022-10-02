@echo off

setlocal enableextensions

IF EXIST %~dp0bin\mingw\main.exe (
	pushd %~dp0Text2
	%~dp0bin\mingw\main.exe %*
	popd
) ELSE ( echo main.exe does not exist )

endlocal