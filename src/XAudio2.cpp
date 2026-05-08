
#include "xaudio2.h"

XAudio2::XAudio2()
{
    m_xAudio2 = 0;
    m_masterVoice = 0;
}

XAudio2::XAudio2(const XAudio2& other)
{
}

XAudio2::~XAudio2()
{
}

bool XAudio2::initialize()
{
    HRESULT result;

    //Initialize COM first
    result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if(FAILED(result))
    {
        return false;
    }

    // Create instance of Xaudio2 Engine
    result = XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR );
    if(FAILED(result))
    {
        return false;
    }

    //Create mastering voice
    result = m_xAudio2 -> CreateMasteringVoice(&m_masterVoice);
    if(FAILED(result))
    {
        return false;
    }

    return true;
}

void XAudio2::shutDown()
{
    //Release Master voice
    if(m_masterVoice)
    {
        m_masterVoice -> DestroyVoice();
        m_masterVoice = 0;
    }

    // release Xaudio2 interface
    if(m_xAudio2)
    {
        m_xAudio2 -> Release();
        m_xAudio2 = 0;
    }

    // unitialize COM
    CoUninitialize();
    return;
}

IXAudio2* XAudio2::getXAudio2()
{
    return m_xAudio2;
}