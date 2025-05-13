# Usage : 
Usage: timer.exe [options]
```bash
Options:
  -T <time>       Set countdown time. Use seconds (e.g. -T 90),
                 or with 'm' for minutes (2m), 'h' for hours (1h)
  -N <true|false> Show popup when done (default: true)
  -ASC <true|false|both> ASCII-only, GUI-only, or both
  -h, --help      Show this help message
```

# 🛠 Installation (Windows)
1. Install [g++ via MSYS2 or MinGW](https://www.msys2.org/)
2. Open a terminal and run:

```bash
git clone https://github.com/your-username/timer-tool
cd timer-tool
setup.bat  ← run this file
```
3. Restart your terminal
4. Now you can run :
```bash
timer -T 60m [...]
```

