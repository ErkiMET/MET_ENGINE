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

global_variable BITMAPINFO BitmapInfo;
global_variable void* BitmapMemory;
global_variable HBITMAP BitmapHandle;

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
    1. Resizes the backbuffer to match the new window size (when user changes  the window size).
*/ 

internal void Win32ResizeDIBSection(int height, int width);


/* Parameters: 
    HDC DeviceContext - Window owns the window, so you have to ask the window permission to draw on it.
    int X - window is a rectnangle so X is the x coordinate of the top left corner
    int Y - window is a rectnangle so Y is the y coordinate of the top
    int Width - width of the rectangle
    int Height - height of the rectangle

    What it does:
    1. Calls a scretchDIBits function that copies a rectangle from our own backbuffer to the window.
*/
internal void Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height);


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
            Win32UpdateWindow(DeviceContext, X, Y , width, height);
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
    // Memory we reserve for the screen. Height * Width for pixels on the screen and * 4 for Colors, blue, red, Green, padding
    size_t BitmapMemorySize = (Height * Width) * 4;

    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    /*BitmapHandle = CreateDIBSection(
        BitmapMemory, &BitmapInfo,
        DIB_RGB_COLORS,
        &BitmapMemory,
        NULL, NULL );
        */
}

internal void Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
    // Copies rectangle of a rectangle that you can copy to another buffer
    StretchDIBits(
        DeviceContext,            // permission from window to draw on a window
        X, Y, Width, Height,      // Destination of a window we want to draw to
        X, Y, Width, Height,      // What we want to draw from our backbuffer
        BitmapMemory,             // Pointer to the backbuffer
        &BitmapInfo,              // Information about the backbuffer
        DIB_RGB_COLORS, SRCCOPY); // We want to copy RGB colors. And we specify the raster operation to copy the bits.);
}
