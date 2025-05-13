#define UNICODE
#include <windows.h>
#include <tchar.h>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include <cwchar>
#include <io.h>
#include <fcntl.h>

int totalSeconds = 1200;
int remainingSeconds = 1200;
bool notify = true;
volatile bool paused = false;
HWND hPauseButton = nullptr;
HBRUSH hDarkBrush;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

enum class Mode { GUI, ASCII, BOTH };
Mode runMode = Mode::GUI;

void SetConsoleCursorVisible(bool visible) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void ShowHelp() {
    wprintf(
        L"Usage: timer.exe [options]\n\n"
        L"Options:\n"
        L"  -T <time>       Set countdown time. Use seconds (e.g. -T 90),\n"
        L"                 or with 'm' for minutes (2m), 'h' for hours (1h)\n"
        L"  -N <true|false> Show popup when done (default: true)\n"
        L"  -ASC <true|false|both> ASCII-only, GUI-only, or both\n"
        L"  -h, --help      Show this help message\n"
    );
}

void DrawCountdown(HDC hdc, RECT rect, HWND hWnd) {
    int cx = (rect.left + rect.right) / 2;
    int cy = (rect.top + rect.bottom) / 2;
    int radius = 100;

    double angle = 360.0 * remainingSeconds / totalSeconds;

    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rect, blackBrush);
    DeleteObject(blackBrush);

    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
    HBRUSH hBrush = CreateSolidBrush(RGB(128, 128, 128));
    SelectObject(hdc, hPen);
    SelectObject(hdc, hBrush);

    Pie(hdc,
        cx - radius, cy - radius,
        cx + radius, cy + radius,
        cx + radius, cy,
        cx + radius * cos(angle * 3.14159 / 180),
        cy - radius * sin(angle * 3.14159 / 180));

    DeleteObject(hPen);
    DeleteObject(hBrush);

    int hours = remainingSeconds / 3600;
    int minutes = (remainingSeconds % 3600) / 60;
    int seconds = remainingSeconds % 60;

    wchar_t buffer[32];
    swprintf(buffer, 32, L"%02d : %02d : %02d%s", hours, minutes, seconds, paused ? L" (Paused)" : L"");
    std::wstring timeText = buffer;

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, timeText.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SetWindowTextW(hWnd, timeText.c_str());
}

DWORD WINAPI CountdownThread(LPVOID hwnd) {
    HWND hWnd = (HWND)hwnd;
    while (remainingSeconds > 0) {
        if (!paused) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            remainingSeconds--;
            InvalidateRect(hWnd, NULL, TRUE);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    MessageBeep(MB_ICONEXCLAMATION);
    if (notify) {
        MessageBox(hWnd, L"Time's up!", L"Timer", MB_OK);
    }

    DeleteObject(hDarkBrush);
    PostQuitMessage(0);
    return 0;
}

int wmain(int argc, wchar_t* argv[]) {
    HINSTANCE hInstance = GetModuleHandleW(NULL);

    for (int i = 1; i < argc; ++i) {
        std::wstring arg = argv[i];

        if (arg == L"-h" || arg == L"--help") {
            ShowHelp();
            return 0;
        }

        if ((arg == L"-T" || arg == L"--time") && i + 1 < argc) {
            std::wstring val = argv[++i];
            wchar_t suffix = towlower(val.back());

            try {
                if (suffix == L'm') {
                    totalSeconds = std::stoi(val.substr(0, val.size() - 1)) * 60;
                } else if (suffix == L'h') {
                    totalSeconds = std::stoi(val.substr(0, val.size() - 1)) * 3600;
                } else {
                    totalSeconds = std::stoi(val);
                }
                remainingSeconds = totalSeconds;
            } catch (...) {
                fwprintf(stderr, L"Invalid -T value: %s\n", val.c_str());
                return 1;
            }

        } else if ((arg == L"-N" || arg == L"--notify") && i + 1 < argc) {
            std::wstring val = argv[++i];
            if (val == L"true" || val == L"1") {
                notify = true;
            } else if (val == L"false" || val == L"0") {
                notify = false;
            } else {
                fwprintf(stderr, L"Invalid -N value: %s (use true/false)\n", val.c_str());
                return 1;
            }
        } else if ((arg == L"-ASC" || arg == L"--ascii") && i + 1 < argc) {
            std::wstring val = argv[++i];
            if (val == L"true" || val == L"1") {
                runMode = Mode::ASCII;
            } else if (val == L"false" || val == L"0") {
                runMode = Mode::GUI;
            } else if (val == L"both") {
                runMode = Mode::BOTH;
            } else {
                fwprintf(stderr, L"Invalid -ASC value: %s (use true/false/both)\n", val.c_str());
                return 1;
            }
        } else {
            fwprintf(stderr, L"Unknown argument: %s\nUse --help for usage.\n", arg.c_str());
            return 1;
        }
    }

    if (runMode == Mode::ASCII) {
        SetConsoleCursorVisible(false);
        while (remainingSeconds > 0) {
            int h = remainingSeconds / 3600;
            int m = (remainingSeconds % 3600) / 60;
            int s = remainingSeconds % 60;
            wprintf(L"\r%02d:%02d:%02d", h, m, s);
            fflush(stdout);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            remainingSeconds--;
        }
        if (notify) wprintf(L"\nTime's up!\n");
        SetConsoleCursorVisible(true);
        return 0;
    }

    hDarkBrush = CreateSolidBrush(RGB(30, 30, 30));

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TimerWindow";
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        L"TimerWindow", L"Countdown Timer",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
        NULL, NULL, hInstance, NULL
    );

    hPauseButton = CreateWindow(
        L"BUTTON", L"",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        150, 300, 100, 30,
        hWnd, (HMENU)1, hInstance, NULL
    );

    if (!hWnd) return 0;

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    CreateThread(NULL, 0, CountdownThread, hWnd, 0, NULL);

    if (runMode == Mode::BOTH) {
        SetConsoleCursorVisible(false);
        std::thread([]() {
            int last = -1;
            while (remainingSeconds > 0) {
                if (!paused && remainingSeconds != last) {
                    int h = remainingSeconds / 3600;
                    int m = (remainingSeconds % 3600) / 60;
                    int s = remainingSeconds % 60;
                    wprintf(L"\r%02d:%02d:%02d", h, m, s);
                    fflush(stdout);
                    last = remainingSeconds;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (notify) wprintf(L"\nTime's up!\n");
            SetConsoleCursorVisible(true);
        }).detach();
    }

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawCountdown(hdc, ps.rcPaint, hWnd);
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                paused = !paused;
                SetWindowText(hPauseButton, paused ? L"Resume" : L"Pause");
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
            if (dis->CtlID == 1) {
                HDC hdc = dis->hDC;
                HBRUSH brush = CreateSolidBrush(RGB(30, 30, 30));
                FillRect(hdc, &dis->rcItem, brush);
                DeleteObject(brush);

                SetTextColor(hdc, RGB(255, 255, 255));
                SetBkMode(hdc, TRANSPARENT);
                const wchar_t* label = paused ? L"Resume" : L"Pause";
                DrawTextW(hdc, label, -1, &dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                FrameRect(hdc, &dis->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
                return TRUE;
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_ERASEBKGND:
            return 1;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
