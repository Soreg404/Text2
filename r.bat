@pushd %~dp0
IF EXIST bin\mingw\main.exe ( bin\mingw\main.exe %* ) ELSE echo main.exe not found
@popd