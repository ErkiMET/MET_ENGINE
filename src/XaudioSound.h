
#ifndef _XAUDIOSOUND_H_
#define _XAUDIOSOUND_H_

#include "xaudio2.h"
#include <memory>

class XAudioSound
{
public:
    XAudioSound();
    XAudioSound(const XAudioSound&);
    ~XAudioSound();
    bool loadTrack(IXAudio2*, char*, float);
    void releaseTrack();

    bool playTrack();
    bool stopTrack();

private:
    struct RiffWaveHeaderType
    {
        char chunkId[4];
        unsigned long chunkSize;
        char format[4];

    };

    struct SubChunkHeaderType
    {
        char subChunkId[4];
        unsigned long subChunkSize;
    };

    struct FmtType
    {
        unsigned short audioFormat;
        unsigned short numChannels;
        unsigned long sampleRate;
        unsigned long bytesPerSecond;
        unsigned short blockAlign;
        unsigned short bitsPerSample;
    };

    bool loadStereoWave(IXAudio2*, char*, float);
    void releaseWaveFile();

    std::unique_ptr<unsigned char[]> m_waveData;
    XAUDIO2_BUFFER m_audioBuffer;
    IXAudio2SourceVoice* m_sourceVoice;

};

#endif

