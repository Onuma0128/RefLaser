#pragma once

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <fstream>

#include <string>
#include <unordered_map>

class AudioManager
{
public:

	struct ChunkHeader {
		char id[4];
		int32_t size;
	};

	struct RiffHeader {
		ChunkHeader chunk;
		char type[4];
	};

	struct FormatChunk {
		ChunkHeader chunk;
		WAVEFORMATEX fmt;
	};

	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer;
		// バッファのサイズ
		unsigned int bufferSize;
	};

private:

	static AudioManager* instance_;

	AudioManager() = default;
	~AudioManager() = default;
	AudioManager(AudioManager&) = delete;
	AudioManager& operator=(AudioManager&) = delete;

public:

	static AudioManager* GetInstance();

	void Finalize();

	void LoadAudioFile(const std::string& directoryPath, const std::string& filePath);

	const SoundData& GetSoundData(const std::string& filePath);

private:

	// 音声データの読み込み
	SoundData SoundLoadWave(const char* filePath);

	// 音声データの解放
	void SoundUnload(SoundData* soundData);

private:

	std::unordered_map<std::string, SoundData> soundDataMap_;
};