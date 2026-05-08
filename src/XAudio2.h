
#ifndef XAUDIO2_H
#define XAUDIO2_H

#include <xaudio2.h>
#include <stdio.h>

class XAudio2 
{
public:
    XAudio2();
    XAudio2(const XAudio2&);
    ~XAudio2();
    
    bool initialize();
    void shutDown();

    IXAudio2* getXAudio2();

private:
    IXAudio2* m_xAudio2;
    IXAudio2MasteringVoice* m_masterVoice;
};

#endif