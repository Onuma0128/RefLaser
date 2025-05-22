#pragma once
#include "ResourceObject.h"
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <wrl.h>

using Microsoft::WRL::ComPtr;

ResourceObject CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);
