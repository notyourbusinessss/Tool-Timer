@echo off
setlocal

:: CONFIG
set TOOLSDIR=C:\Tools
set EXENAME=timer.exe
set SOURCE=timer.cpp

:: 1. Check for g++
where g++ >nul 2>nul
if errorlevel 1 (
    echo ERROR: g++ not found. Please install MinGW or MSYS2 and ensure g++ is in your PATH.
    pause
    exit /b 1
)

:: 2. Compile timer.cpp
echo Compiling %SOURCE%...
g++ %SOURCE% -o %EXENAME% -municode -lgdi32 -luser32
if errorlevel 1 (
    echo Compilation failed.
    pause
    exit /b 1
)

:: 3. Create C:\Tools if needed
if not exist "%TOOLSDIR%" (
    echo Creating %TOOLSDIR%...
    mkdir "%TOOLSDIR%"
)

:: 4. Copy EXE
echo Copying %EXENAME% to %TOOLSDIR%...
copy /Y "%EXENAME%" "%TOOLSDIR%"

:: 5. Add to PATH if not already
echo Checking PATH...
echo %PATH% | find /I "%TOOLSDIR%" >nul
if errorlevel 1 (
    echo Adding %TOOLSDIR% to PATH...
    setx /M PATH "%PATH%;%TOOLSDIR%"
) else (
    echo %TOOLSDIR% is already in PATH.
)

echo Installation complete!
echo You can now use the 'timer' command from any terminal window.
pause
