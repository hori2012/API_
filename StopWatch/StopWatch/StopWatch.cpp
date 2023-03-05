#include "framework.h"
#include "StopWatch.h"
#include <string>
#include<stdlib.h>
using namespace std;

#define MAX_LOADSTRING 100
#define TM_START 1000
#define TM_STOP  1002
#define TM_RESET 1004
#define TM_RESULT 1008
#define TM_SPLIT 1006

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND hTimer, hResult;
HWND t_Timer = NULL;
int t_hours = 0;
int t_minutes = 0;
int t_seconds = 0;
int t_count = 0;
string timer = "";
void AddControl(HWND hWnd);
wstring TimerResult();
void ZeroTime();
void TwoDigit(int n, char* buff, int& index);
void UpdateTimer();
VOID CALLBACK TimerProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_STOPWATCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STOPWATCH));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}




ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STOPWATCH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_STOPWATCH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       200, 200, 500, 500, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        AddControl(hWnd);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case TM_START:
                SetTimer(hWnd, 0, 1000, (TIMERPROC)TimerProc);
                break;
            case TM_STOP:
                KillTimer(hWnd, 0);
                break;
            case TM_RESET:
                timer = "";
                t_count = 0;
                ZeroTime();
                SetWindowText(hTimer, L"00:00:00");
                SetWindowText(hResult, NULL);
                KillTimer(hWnd, 0);
                break;
            case TM_SPLIT:
                SetWindowText(hResult, TimerResult().c_str());
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void AddControl(HWND hWnd) {
    CreateWindowW(L"Static", L"STOP WATCH", WS_CHILD | WS_VISIBLE | SS_CENTER, 200, 50, 100, 30, hWnd, NULL, NULL, NULL);
    hTimer = CreateWindowW(L"Static", L"00:00:00", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 200, 100, 100, 30, hWnd, NULL, NULL, NULL);
    CreateWindowW(L"Button", L"Start", WS_CHILD | WS_VISIBLE | SS_CENTER, 60, 150, 80, 30, hWnd, (HMENU)TM_START, NULL, NULL);
    CreateWindowW(L"Button", L"Stop", WS_CHILD | WS_VISIBLE | SS_CENTER, 155, 150, 80, 30, hWnd, (HMENU)TM_STOP, NULL, NULL);
    CreateWindowW(L"Button", L"Reset", WS_CHILD | WS_VISIBLE | SS_CENTER, 255, 150, 80, 30, hWnd, (HMENU)TM_RESET, NULL, NULL);
    CreateWindowW(L"Static", L"Result", WS_CHILD | WS_VISIBLE | SS_CENTER, 100, 190, 60, 20, hWnd, NULL, NULL, NULL);
    hResult = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL, 100, 210, 300, 200, hWnd, (HMENU)TM_RESULT, NULL, NULL);
    CreateWindowW(L"Button", L"Split", WS_CHILD | WS_VISIBLE | SS_CENTER, 360, 150, 80, 30, hWnd, (HMENU)TM_SPLIT, NULL, NULL);
}
wstring TimerResult() {
    TCHAR txtResult[1000];
    string txtFinal = "";
    t_count++;
    GetWindowText(hResult, txtResult, 1000);
    string txt_result(txtResult, txtResult + _tcslen(txtResult) + 2);
    /* if (t_count >= 2) {
         txtFinal += txt_result + "\r\n" + "Result " + to_string(t_count) + " is : " + timer;
     }
     else {*/
    txtFinal += "Result " + to_string(t_count) + " is : " + timer + "\r\n" + txt_result;
    //}
    wstring temp = wstring(txtFinal.begin(), txtFinal.end());
    return temp;
}

void ZeroTime() {
    t_hours = 0;
    t_minutes = 0;
    t_seconds = 0;
}
void TwoDigit(int n, char* buff, int& index) {
    int hight = 0, low = 0;
    if (n >= 10) {
        hight = n / 10;
        low = n % 10;
    }
    else {
        hight = 0;
        low = n;
    }
    buff[index] = '0' + hight;
    index++;
    buff[index] = '0' + low;
    index++;
}

void UpdateTimer() {
    timer = "";
    char buf[32];
    int index = 0;
    TwoDigit(t_hours, buf, index);
    buf[index] = ':';
    index++;
    TwoDigit(t_minutes, buf, index);
    buf[index] = ':';
    index++;
    TwoDigit(t_seconds, buf, index);
    for (int i = 0; i < 8; i++) {
        timer += buf[i];
    }
    wstring temp = wstring(timer.begin(), timer.end());
    LPCWSTR result = temp.c_str();
    SetWindowText(hTimer, result);
}

VOID CALLBACK TimerProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime) {
    t_seconds++;
    if (t_seconds >= 60) {
        t_seconds = 0;
        t_minutes++;
        if (t_minutes >= 60) {
            t_minutes = 0;
            t_hours++;
            if (t_hours >= 25) {
                t_hours = 24;
                KillTimer(t_Timer, 0);
            }
        }
    }
    UpdateTimer();
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
