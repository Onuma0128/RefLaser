#include "Audio.h"

#include <cassert>

#include "AudioManager.h"

Audio::Audio()
{
    HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(hr));

    hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
    assert(SUCCEEDED(hr));
}

Audio::~Audio()
{
    if (masterVoice_) {
        masterVoice_->DestroyVoice();
    }
    xAudio2_.Reset();
}

void Audio::SoundPlayWave(const std::string& filePath)
{
    const AudioManager::SoundData& soundData = AudioManager::GetInstance()->GetSoundData(filePath);

    IXAudio2SourceVoice* sourceVoice = nullptr;
    HRESULT hr = xAudio2_->CreateSourceVoice(&sourceVoice, &soundData.wfex);
    assert(SUCCEEDED(hr));

    XAUDIO2_BUFFER buffer = {};
    buffer.pAudioData = soundData.pBuffer;
    buffer.AudioBytes = soundData.bufferSize;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    hr = sourceVoice->SubmitSourceBuffer(&buffer);
    assert(SUCCEEDED(hr));

    hr = sourceVoice->Start();
    assert(SUCCEEDED(hr));
}
