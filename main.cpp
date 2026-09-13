#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdint.h>

static BITMAPINFO wBitmapInfo;
static void* wBitmapMemory;
static int wBitmapWidth;
static int wBitmapHeight;

static void wAllocateDIBSection(int width, int height) {
    // We need to free the memory and give it back to the operating system
    // In this case we'll release it before we re-allocate with VirtualAlloc
    if (wBitmapMemory) {
        VirtualFree(wBitmapMemory, 0, MEM_RELEASE);
    }

    wBitmapWidth = width;
    wBitmapHeight = height;

    wBitmapInfo.bmiHeader.biSize = sizeof(wBitmapInfo.bmiHeader);
    wBitmapInfo.bmiHeader.biWidth = wBitmapWidth;
    wBitmapInfo.bmiHeader.biHeight = -wBitmapHeight;
    wBitmapInfo.bmiHeader.biPlanes = 1;
    wBitmapInfo.bmiHeader.biBitCount = 32;
    wBitmapInfo.bmiHeader.biCompression = BI_RGB;

    // Calculate back buffer size
    int bytesPerPixel = 4;
    int bitmapMemorySize = (wBitmapWidth * wBitmapHeight) * bytesPerPixel;

    // Allocate back buffer memory manually
    wBitmapMemory = VirtualAlloc(NULL, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    // Write pixels
    int pitch = wBitmapWidth * bytesPerPixel;
    uint8_t *row = (uint8_t *)wBitmapMemory;

    for (int y = 0; y < wBitmapHeight; ++y) {
        uint8_t* pixel = (uint8_t *)row;
        for (int x = 0; x < wBitmapWidth; ++x) {
            *pixel = 255;
            ++pixel;
            *pixel = 0;
            ++pixel;
            *pixel = 0;
            ++pixel;
            *pixel = 0;
            ++pixel;
        }
        row += pitch;
     }
}

static void wUpdate(HDC hdc, RECT *wRect, int x, int y, int width, int height) {
    int wWidth = wRect->right - wRect->left;
    int wHeight = wRect->bottom - wRect->top;
    StretchDIBits(
        hdc,
        0, 0, wBitmapWidth, wBitmapHeight,
        0, 0, wWidth, wHeight,
        wBitmapMemory,
        &wBitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        RECT cRect;
        GetClientRect(hwnd, &cRect);
        wAllocateDIBSection(
            cRect.right - cRect.left,
            cRect.bottom - cRect.top
        );
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            int x = ps.rcPaint.left;
            int y = ps.rcPaint.top;
            int width = ps.rcPaint.right - ps.rcPaint.left;
            int height = ps.rcPaint.bottom - ps.rcPaint.top;

            RECT rect;
            GetClientRect(hwnd, &rect);

            wUpdate(hdc, &rect, x, y, width, height);

            EndPaint(hwnd, &ps);
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}