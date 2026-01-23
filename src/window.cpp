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

struct Win32BackBuffer
{
    BITMAPINFO bitmapInfo;
    void *memory;
    int width;
    int height;
    int pitch;
};

struct Win32WindowDimension
{
    int width;
    int height;
};

global_variable HWND window;
global_variable bool windowRunning = false;
const global_variable int BYTES_PER_PIXEL = 4;
global_variable struct Win32BackBuffer globalBuffer;


internal Win32WindowDimension Win32GetWindowDimension(HWND window);

/*
    Parameters:
    HWND Window - handler to the window.
    UINT Message - Windows commmunicates by a queue of messages. Each message has a unique identifier.
    WPARAM WParam - not important for now
    LPARAM LParam - Give additional information about the message

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

internal void Win32ResizeDIBSection(Win32BackBuffer *Buffer, int width, int height);


/* Parameters: 
    HDC DeviceContext - Window owns the window, so you have to ask the window permission to draw on it.
    int X - X Coordinate of rectangle
    int Y - Y coordinate of the rectangle
    int Width - width of the rectangle
    int Height - height of the rectangle

    What it does:
    1. Calls a scretchDIBits function that copies a rectangle from our own backbuffer to the window.
*/
internal void Win32CopyBufferToWindow(Win32BackBuffer *Buffer, HDC deviceContext, int windowWidth, int windowHeight);
/* Parameters: 
    int xOffset - offset for the gradient in the x axis
    int yOffset - offset for the gradient in the y axis

    What it does:
    Fills the backbuffer with a gradient pattern.
*/
internal void drawGradient(Win32BackBuffer *Buffer, int BlueOffset, int GreenOffset);


internal void drawGradient(Win32BackBuffer *Buffer, int BlueOffset, int GreenOffset)
{
    // TODO(Erkik) See if passing by reference is faster

    uint8_t *row = static_cast<uint8_t *>(Buffer->memory);
    for (int y = 0; y < Buffer->height; y++)
    {
        uint32_t *pixel = reinterpret_cast<uint32_t *>(row);
        for (int x = 0; x < Buffer->width; x++)
        {
            uint8_t red = 0;
            uint8_t blue = static_cast<uint8_t>(x + BlueOffset);

            uint8_t green = static_cast<uint8_t>(y + GreenOffset);

            /* 
                Our pixel is a 32 bit value it holds 8 bits for each color channel RGB + padding.
                When we write pixels in memory we want to write them like this
                Memory : | Red | Green | Blue | Padding |
                but because microsoft uses little endian it reverses the order when storing in memory like this
                Register : | Padding | Blue | Green | Red |
                So in order to get the correct order we have to shift greens bits by 8 and 
                
            */
            *pixel++ = ( (red << 16 ) | (green << 8 ) | blue);

        }
        row += Buffer->pitch;
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
            Win32WindowDimension dimension = Win32GetWindowDimension(window);
            Win32CopyBufferToWindow(&globalBuffer, deviceContext, dimension.width, dimension.height);
            EndPaint(window, &Paint);

        }break;

        // When the application is activated or deactivated.
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");

        }break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32_t VKCode = LOWORD(wParam);
            bool WasDown = ((lParam & (1 << 30)) != 0);

            if(VKCode == VK_ESCAPE)
            {
                DestroyWindow(window);
            }
            else if (VKCode == 'W')
            {
                OutputDebugStringA("W key pressed\n");
            }
            else if (VKCode == 'A')
            {
                OutputDebugStringA("A key pressed\n");
            }
            else if (VKCode == 'S')
            {
                OutputDebugStringA("S key pressed\n");
            }
            else if (VKCode == 'D')
            {
                OutputDebugStringA("D key pressed\n");
            }
        }break;

        // Default case to handle all other messages
        default:
        {
            Result = DefWindowProc(window, message, wParam, lParam);
        }break;
    }
    return Result;
}

// Windows version of main function. The C CRT calls this function to start the program.
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

    Win32ResizeDIBSection(&globalBuffer, 1280, 720);
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = win32MainWindowCallBack;
    WindowClass.hInstance = instance;
    WindowClass.lpszClassName = L"Met Engine";
    // set this to NULL so window background is not drawn automatically. We will handle it ourselves
    WindowClass.hbrBackground = NULL;
    


    if(RegisterClass(&WindowClass))
    {
        HWND window = CreateWindowEx(
            0,                                                          //optional window styles
            METENGINE,                                                  // Window class name
            L"Met Engine",                                              // Window title
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,                           // Window styles
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,  // Size and position of window
            NULL,                                                        // Parent window 
            NULL,                                                        // Menu 
            instance,                                                    // Instance of the window
            NULL                                                         // Additional application data
            );
            
        if(window)
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


                drawGradient(&globalBuffer, xOffset++, yOffset);
                Win32WindowDimension dimension = Win32GetWindowDimension(window);
                HDC deviceContext = GetDC(window);
                Win32CopyBufferToWindow(&globalBuffer, deviceContext, dimension.width, dimension.height);
                ReleaseDC(window, deviceContext);
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

internal void Win32ResizeDIBSection(Win32BackBuffer *Buffer, int width, int height)
{   
    if(Buffer->memory)
    {
        VirtualFree(Buffer->memory, 0, MEM_RELEASE);
    }

    Buffer->width = width;
    Buffer->height = height;
    Buffer->bitmapInfo.bmiHeader.biSize = sizeof(Buffer->bitmapInfo.bmiHeader);
    Buffer->bitmapInfo.bmiHeader.biWidth = Buffer->width;
    Buffer->bitmapInfo.bmiHeader.biHeight = -Buffer->height;  
    Buffer->bitmapInfo.bmiHeader.biPlanes = 1;
    Buffer->bitmapInfo.bmiHeader.biBitCount = 32;
    Buffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;

    size_t bitmapMemorySize = (Buffer->width * Buffer->height) * BYTES_PER_PIXEL;

    //We reserve memory from windows for our backbuffer and write to it
    Buffer-> memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    // TODO(Erkik): Probably clear this to black

    Buffer -> pitch = width * BYTES_PER_PIXEL;
}

internal void Win32CopyBufferToWindow(Win32BackBuffer *Buffer, HDC deviceContext, int windowWidth, int windowHeight)
{

    StretchDIBits(
        deviceContext,                                            // permission from windows to draw on a window
        /*
        x, y, width, height,
        x, y, width, height,
        */                                             
        0, 0, windowWidth, windowHeight,                          // Destination of a window we want to draw to
        0, 0, Buffer->width, Buffer->height,                      // What we want to draw from our backbuffer
        Buffer->memory,                                           // Pointer to the backbuffer
        &Buffer->bitmapInfo,                                      // Information about the backbuffer
        DIB_RGB_COLORS, SRCCOPY);                                 // We want to copy RGB colors. And we specify the raster operation to copy the bits.);
}

internal Win32WindowDimension Win32GetWindowDimension(HWND window)
{
   Win32WindowDimension result;

   RECT clientRect;
    GetClientRect(window, &clientRect);
    result.width = clientRect.right - clientRect.left;
    result.height = clientRect.bottom - clientRect.top;
    return result;
}