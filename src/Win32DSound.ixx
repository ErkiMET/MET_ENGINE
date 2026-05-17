
module;
#include <windows.h>
#include <dsound.h>
#include <cstdint>

export module Win32DSound;

export LPDIRECTSOUNDBUFFER globalSecondaryBuffer;



export void win32InitDsound(HWND window, uint32_t samplesPerSecond, uint32_t bufferSize){
    #define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
    typedef DIRECT_SOUND_CREATE(direct_sound_create);

    HMODULE dSoundLibrary = LoadLibraryA("dsound.dll");
    if(dSoundLibrary)
    {
       direct_sound_create *DirectSoundCreate = (direct_sound_create *) GetProcAddress(dSoundLibrary, "DirectSoundCreate");
        // Pointer to DirectSound interface
       LPDIRECTSOUND directSound;
       if(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &directSound, 0)))
       {
            WAVEFORMATEX WaveFormat =  {0};
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = samplesPerSecond;
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nBlockAlign * WaveFormat.nSamplesPerSec ;
            WaveFormat.cbSize = 0;

            if(SUCCEEDED(directSound -> SetCooperativeLevel(window, DSSCL_PRIORITY)))
            {
                //Create primary buffer
                DSBUFFERDESC BufferDescription = {0};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER primaryBuffer;
                if(SUCCEEDED(directSound -> CreateSoundBuffer(&BufferDescription, &primaryBuffer, 0)))
                {
                    if(SUCCEEDED(primaryBuffer -> SetFormat(&WaveFormat)))
                    {
                        // Format is set!
                    }
                    else
                    {
                        // TODO(Erkik): Log error
                    }
                }
            }
            else
            {
                //Todo(Erkik): Log error
            }

            //create Secondary buffer
            DSBUFFERDESC BufferDescription = {0};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = 0;
            BufferDescription.dwBufferBytes = bufferSize;
            BufferDescription.lpwfxFormat = &WaveFormat;
            HRESULT error = directSound -> CreateSoundBuffer(&BufferDescription, &globalSecondaryBuffer, 0);
            if(SUCCEEDED(error))
            {
                OutputDebugStringA("Secondary buffer created succesfully.\n");
            }
            else
            {
                //Todo(Erkik): Log error
            }
       }
       else
       {
         //Todo(Erkik): Log error
       }
    }
    
}