#ifndef UNICODE
#define UNICODE
#endif

#define global_variable static
#define local_persist static
// internal is used for functions limited to a single file / class
#define internal static


#include <windows.h>
#include <winuser.h>
#include <wingdi.h>
#include <memoryapi.h>
#include <stdint.h>

// (NOTE) temporary global variables just to get things work 
global_variable BITMAPINFO bitmapInfo;
global_variable void* bitmapMemory;

global_variable HBITMAP bitmapHandle;
global_variable int bitmapWidth;
global_variable int bitmapHeight;
global_variable int BYTES_PER_PIXEL = 4;
global_variable bool windowRunning = false;
/*
    Parameters:
    HWND Window - handler to the window.
    UINT Message - Windows commmunicates by a queue of messages. Each message has a unique identifier.
    WPARAM WParam - not important for now
    LPARAM LParam - not important for now

    What it does:
    1. Takes a message from the windows message queue and depending on it flag it does what is specified in the switch case.
    2. Returns a LRESULT which is a long integer type that holds the message.
*/

LRESULT CALLBACK MainWindowCallBack(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);

/* Parameters: 
    int width - width of the backbuffer (window)
    int width - width of the backbuffer (window)

    What it does:
    Asks memory from windows and allocates it to the backbuffer.
*/ 

internal void Win32ResizeDIBSection(int width, int height);


/* Parameters: 
    HDC DeviceContext - Window owns the window, so you have to ask the window permission to draw on it.
    int X - X Coordinate of rectangle
    int Y - Y coordinate of the rectangle
    int Width - width of the rectangle
    int Height - height of the rectangle

    What it does:
    1. Calls a scretchDIBits function that copies a rectangle from our own backbuffer to the window.
*/
internal void Win32UpdateWindow(HDC deviceContext, RECT *clientRect, int x, int y, int width, int height);
/* Parameters: 
    int xOffset - offset for the gradient in the x axis
    int yOffset - offset for the gradient in the y axis

    What it does:
    Fills the backbuffer with a gradient pattern.
*/
internal void drawRadient(int xOffset, int yOffset);
internal void drawRadient(int xOffset, int yOffset)
{
    int width = bitmapWidth;
    int height = bitmapHeight;

    int pitch = bitmapWidth * BYTES_PER_PIXEL;
    uint8_t *row = static_cast<uint8_t *>(bitmapMemory);

    for (int y = 0; y < bitmapHeight; y++)
    {
        uint32_t *pixel = reinterpret_cast<uint32_t *>(row);
        for (int x = 0; x < bitmapWidth; x++)
        {
            uint8_t blue = static_cast<uint8_t>(x + xOffset);

            uint8_t green = static_cast<uint8_t>(y + yOffset);

            /* 
                microsoft uses little endian format so the order of bytes we write in memory are reversed.
                In memory pixels are written like this:     BB GG RR xx
                so the order in regsiter will be reverse:   xx BB GG RR 
                So in order to write the colors in correct order we have to shift green by 8 bits.
            */
            *pixel++ = ((green << 8 ) | blue);

        }
        row += pitch;
    }
}

LRESULT CALLBACK win32MainWindowCallBack(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{  
    LRESULT Result = 0;

    switch (message)
    {   
        // When the window is resized
        case WM_SIZE:
        {   
            RECT clientRect;

            GetClientRect(window, &clientRect);
            int height = clientRect.bottom - clientRect.top;
            int width = clientRect.right - clientRect.left; 
            Win32ResizeDIBSection(width, height);
            OutputDebugStringA("WM_SIZE\n");

        }break;

        // When the window is being destroyed
        case WM_DESTROY:
        {
            DestroyWindow(window);
            OutputDebugStringA("WM_DESTROY\n");

        }break;
        
        // When the window is being closed
        case WM_CLOSE:
        {
            DestroyWindow(window);
            OutputDebugStringA("WM_CLOSE\n");

        }break;

        // When the window needs to be repainted
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;

            HDC deviceContext =  BeginPaint( window, &Paint);
            
            LONG height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            LONG width = Paint.rcPaint.right - Paint.rcPaint.left; 
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;

            // (TODO) Clean up later
            RECT clientRect;
            GetClientRect(window, &clientRect);
            Win32UpdateWindow(deviceContext, &clientRect, X, Y, width, height);
            EndPaint(window, &Paint);

        }break;

        // When the application is activated or deactivated
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");

        }break;

        // Default case to handle all other messages
        default:
        {

            Result = DefWindowProc(window, message, wParam, lParam);
        }break;
    }
    return Result;
}

// Windows version of main function. The C SRC calls this function to start the program.
int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    PSTR lpCmdLine, 
    int nCmdShow)
{
    // name for our window class
    const wchar_t METENGINE[] = L"Met Engine";

    // Define a window class and set attributes
    WNDCLASS WindowClass = {};
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = win32MainWindowCallBack;
    WindowClass.hInstance = instance;
    WindowClass.lpszClassName = L"Met Engine";
    // set this to NULL so window background is not drawn automatically. We will handle it ourselves
    WindowClass.hbrBackground = NULL;
    


    if(RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(
            0,                                                          //optional window styles
            METENGINE,                                                  // Window class name
            L"Met Engine",                                              // Window title
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,                       // Window styles
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,  // Size and position of window
            NULL,                                                        // Parent window 
            NULL,                                                        // Menu 
            instance,                                                    // Instance of the window
            NULL                                                         // Additional application data
            );
            
        if(WindowHandle)
        {
            windowRunning = true;
            int xOffset = 0;
            int yOffset = 0;
            // Main message loop
            while(windowRunning)
            {
                MSG Message;
                // PeekMessage checks message queue for any new messages, if there is one removes it from queue and processes it.
                // return 0 if there is no messages
                while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                    {
                        windowRunning = false;
                    }

                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                drawRadient(xOffset++, yOffset);

                HDC deviceContext = GetDC(WindowHandle);

                RECT ClientRect;
                GetClientRect(WindowHandle, &ClientRect);
                int windowWidth = ClientRect.right - ClientRect.left;
                int windowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(deviceContext, &ClientRect, 0, 0, windowWidth, windowHeight);
                ReleaseDC(WindowHandle, deviceContext);
                ++xOffset;
            }
        }

    }
    else
    {
        // Todo : log the error
    }
    return 0;
}

internal void Win32ResizeDIBSection(int width, int height)
{   
    if(bitmapMemory)
    {
        VirtualFree(bitmapMemory, 0, MEM_RELEASE);
    }

    bitmapWidth = width;
    bitmapHeight = height;

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = bitmapWidth;
    bitmapInfo.bmiHeader.biHeight = -bitmapHeight;  // - because we want to render form top to bottom
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    size_t bitmapMemorySize = (bitmapWidth * bitmapHeight) * BYTES_PER_PIXEL;

    //We reserve memory from windows for our backbuffer and write to it
    bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    // TODO(Erkik): Probably clear this to black
}

internal void Win32UpdateWindow(HDC deviceContext, RECT *clientRect, int x, int y, int width, int height)
{
    int windowWidth = clientRect -> right - clientRect -> left;
    int windowHeight = clientRect -> bottom - clientRect -> top;

    StretchDIBits(
        deviceContext,                             // permission from window to draw on a window
        /*
        x, y, width, height,
        x, y, width, height,
        */                                             
        0, 0, windowWidth, windowHeight,                // Destination of a window we want to draw to
        0, 0, bitmapWidth, bitmapHeight ,               // What we want to draw from our backbuffer
        bitmapMemory,                                   // Pointer to the backbuffer
        &bitmapInfo,                                    // Information about the backbuffer
        DIB_RGB_COLORS, SRCCOPY);                       // We want to copy RGB colors. And we specify the raster operation to copy the bits.);
}
