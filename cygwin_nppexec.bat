@echo off
set PROJECT_PATH=%~dp0
set COMPILE=C:\cygwin64\bin\bash --login "/cygdrive/c/Users/x/Home/Projects/JS Term/macros/compile.sh"
set RUN=C:\cygwin64\bin\bash --login "/cygdrive/c/Users/x/Home/Projects/JS Term/macros/run.sh"

cd "C:\Program Files\Notepad++\"
start notepad++.exe