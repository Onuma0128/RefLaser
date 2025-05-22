#include "AudioManager.h"

#include <cassert>

AudioManager* AudioManager::instance_ = nullptr;

AudioManager* AudioManager::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new AudioManager;
    }
    return instance_;
}

void AudioManager::Finalize()
{
    for (auto& pair : soundDataMap_) {
        SoundUnload(&pair.second);
    }
}

void AudioManager::LoadAudioFile(const std::string& directoryPath, const std::string& filePath)
{
	const std::string fullFilePath = directoryPath + "/" + filePath;

	if (soundDataMap_.find(filePath) == soundDataMap_.end()) {
		soundDataMap_[filePath] = SoundLoadWave(fullFilePath.c_str());
	}
}

const AudioManager::SoundData& AudioManager::GetSoundData(const std::string& filePath)
{
    auto it = soundDataMap_.find(filePath);
    if (it == soundDataMap_.end()) {
		// 未ロードならロードする
		assert(0);
    }
    return it->second;
}

AudioManager::SoundData AudioManager::SoundLoadWave(const char* filePath)
{
	HRESULT result{};
	// 1.ファイルオープン
	std::ifstream file;
	file.open(filePath, std::ios_base::binary);
	assert(file.is_open());
	// 2.wavデータ読み込み
	RiffHeader riff{};
	file.read((char*)&riff, sizeof(riff));
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	ChunkHeader data{};
	file.read((char*)&data, sizeof(data));
	if (strncmp(data.id, "JUNK", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}
	while (strncmp(data.id, "data", 4) != 0) {
		// 現在のチャンクをスキップ
		file.seekg(data.size, std::ios_base::cur);

		// 次のチャンクを読み込む
		file.read((char*)&data, sizeof(data));

		// ファイル終端に到達した場合、エラーとする
		if (file.eof()) {
			assert(0 && "Failed to find 'data' chunk in WAV file.");
			return SoundData{}; // 空のSoundDataを返す（安全性向上）
		}
	}
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	// 3.ファイルクローズ
	file.close();
	// 4.読み込んだ音声データをreturn
	SoundData soundData{};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void AudioManager::SoundUnload(SoundData* soundData)
{
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}
