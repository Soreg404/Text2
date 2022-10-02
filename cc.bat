@echo off

setlocal enableextensions

echo [36m

prompt $E[92m$G$E[36m

pushd %~dp0

md bin\mingw\int

@echo on
mingw32-make %*
@echo off

echo [0m

popd

endlocal