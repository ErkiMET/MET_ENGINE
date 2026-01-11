#ifndef UNICODE
#define UNICODE
#endif

#define global_variable static
#define local_persist static
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
    int height - height of the backbuffer (window)
    int width - width of the backbuffer (window)

    What it does:
    Asks memory from windows and allocates it to the backbuffer.
*/ 

internal void Win32ResizeDIBSection(int height, int width);


/* Parameters: 
    HDC DeviceContext - Window owns the window, so you have to ask the window permission to draw on it.
    int X - X Coordinate of rectangle
    int Y - Y coordinate of the rectangle
    int Width - width of the rectangle
    int Height - height of the rectangle

    What it does:
    1. Calls a scretchDIBits function that copies a rectangle from our own backbuffer to the window.
*/
internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height);


LRESULT CALLBACK Win32MainWindowCallBack(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{  
    LRESULT Result = 0;

    switch (Message)
    {   
        // When the window is resized
        case WM_SIZE:
        {   
            RECT ClientRect;

            GetClientRect(Window, &ClientRect);
            int height = ClientRect.bottom - ClientRect.top;
            int width = ClientRect.right - ClientRect.left; 
            Win32ResizeDIBSection(width, height);
            OutputDebugStringA("WM_SIZE\n");

        }break;

        // When the window is being destroyed
        case WM_DESTROY:
        {
            DestroyWindow(Window);
            OutputDebugStringA("WM_DESTROY\n");

        }break;
        
        // When the window is being closed
        case WM_CLOSE:
        {
            DestroyWindow(Window);
            OutputDebugStringA("WM_CLOSE\n");

        }break;

        // When the window needs to be repainted
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;

            HDC DeviceContext =  BeginPaint( Window, &Paint);
            
            LONG height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            LONG width = Paint.rcPaint.right - Paint.rcPaint.left; 
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;

            // (TODO) Clean up later
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, width, height);
            EndPaint(Window, &Paint);
        }break;

        // When the application is activated or deactivated
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");

        }break;

        // Default case to handle all other messages
        default:
        {

            Result = DefWindowProc(Window, Message, WParam, LParam);
        }break;
    }
    return Result;
}

// Windows version of main function. The C SRC calls this function to start the program.
int WINAPI WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    PSTR lpCmdLine, 
    int nCmdShow)
{
    const wchar_t METENGINE[] = L"Met Engine";
    WNDCLASS WindowClass = {};

    WindowClass.style = CS_OWNDC |CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallBack;
    WindowClass.hInstance = Instance;
    // WindowClass.hIcon ;
    WindowClass.lpszClassName = L"Met Engine";
    


    if(RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(
            0,                                                          //optional window styles
            METENGINE,                                                  // Window class name
            L"Met Engine",                                              // Window title
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,                           // Window styles
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,  // Size and position of window
            NULL,                                                        // Parent window 
            NULL,                                                        // Menu 
            Instance,                                                    // Instance of the window
            NULL                                                         // Additional application data
            );
            
        if(WindowHandle)
        {
            // Message variable holds the message windows picks up from queue
            MSG Message;
            while(GetMessage(&Message, NULL, 0, 0) > 0)
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
        }
        else
        {
            // Todo : log the error
        }
    }
    else
    {
        // Todo : log the error
    }
    return 0;
}

internal void Win32ResizeDIBSection(int Height, int Width)
{   
    if(bitmapMemory)
    {
        VirtualFree(bitmapMemory, 0, MEM_RELEASE);
    }

    bitmapWidth = Width;
    bitmapHeight = Height;

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = bitmapWidth;
    bitmapInfo.bmiHeader.biHeight = -bitmapHeight;  // - because we want to render form top to bottom
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;


    int bytesPerPixel = 4;
    size_t bitmapMemorySize = (Height * Width) * bytesPerPixel;
    //We reserve memory from windows for our backbuffer and write to it
    bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    uint8_t *row = static_cast<uint8_t*>(bitmapMemory);
    for(int Y = 0; Y < bitmapHeight; Y++)
    {
        uint32_t *Pixel =  reinterpret_cast<uint32_t*> (row);
        for( int X = 0; X < bitmapWidth; X++)
        {

        }
    }

}

internal void Win32UpdateWindow( HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
    int WindowWidth = WindowRect -> right - WindowRect -> left;
    int WindowHeight = WindowRect -> bottom - WindowRect -> top;

    StretchDIBits(
        DeviceContext,                                  // permission from window to draw on a window
        0, 0, bitmapWidth, bitmapHeight,                // Destination of a window we want to draw to
        0, 0, WindowWidth, WindowHeight,                // What we want to draw from our backbuffer
        bitmapMemory,                                   // Pointer to the backbuffer
        &bitmapInfo,                                    // Information about the backbuffer
        DIB_RGB_COLORS, SRCCOPY);                       // We want to copy RGB colors. And we specify the raster operation to copy the bits.);
}
