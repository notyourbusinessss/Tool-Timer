# Usage : 
This is what timer --help gives :
```bash
Options:
  -T <time>       Set countdown time. Use seconds (e.g. -T 90),
                 or with 'm' for minutes (2m), 'h' for hours (1h)
  -N <true|false> Show popup when done (default: true)
  -ASC <true|false|both> ASCII-only, GUI-only, or both
  -h, --help      Show this help message
```

# Automatic Installation (Windows)
1. Install [g++ via MSYS2 or MinGW](https://www.msys2.org/)
2. Open a terminal and run:

```bash
git clone https://github.com/notyourbusinessss/Tool-Time
cd timer-tool
setup.bat  ← run this file
```
3. Restart your terminal
4. Now you can run :
```bash
timer -T 60m [...]
```

# Manual instalation 
1. Install [g++ via MSYS2 or MinGW](https://www.msys2.org/)
2. Clone repo
```bash
git clone https://github.com/notyourbusinessss/Tool-Time
cd timer-tool
```
3. Compile the program 
```bash
g++ timer.cpp -o timer.exe -municode -lgdi32 -luser32
```
4. create the instalation Directory 
For example 
```
mkdir C:\Tools
```
5. move the executable 
```bash
copy timer.exe C:\Tools\
```
6. Add the Folder to Your System PATH
Open the Start Menu, search "Environment Variables", and click "Edit the system environment variables"

In the window that appears, click "Environment Variables..."

Under System variables, find Path, select it, and click Edit

Click New, then add:
```bash
C:\tools
```
Finally click ok.
7. Restart the terminal !
