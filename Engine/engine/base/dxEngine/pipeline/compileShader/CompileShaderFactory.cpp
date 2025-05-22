#include "CompileShaderFactory.h"

#include <format>
#include <unordered_map>

#include "Logger.h"
#include "StringUtility.h"

D3D12_SHADER_BYTECODE& CompileShaderFactory::GetCompileShader_VS(PipelineType type, ComPtr<IDxcUtils> dxcUtils, ComPtr<IDxcCompiler3>& dxcCompiler, ComPtr<IDxcIncludeHandler> includeHandler)
{
	static std::unique_ptr<CompileShaderBase> compileShader = nullptr;

	compileShader = GetCompileShaderPtr(type, dxcUtils, dxcCompiler, includeHandler);

	return compileShader->BuildVS_Shader();
}

D3D12_SHADER_BYTECODE& CompileShaderFactory::GetCompileShader_PS(PipelineType type, ComPtr<IDxcUtils> dxcUtils, ComPtr<IDxcCompiler3>& dxcCompiler, ComPtr<IDxcIncludeHandler> includeHandler, PostEffectType effectType)
{
	static std::unique_ptr<CompileShaderBase> compileShader = nullptr;

	compileShader = GetCompileShaderPtr(type, dxcUtils, dxcCompiler, includeHandler);

	return compileShader->BuildPS_Shader(effectType);
}

D3D12_SHADER_BYTECODE CompileShaderFactory::CreateCompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	ComPtr<IDxcUtils> dxcUtils,
	ComPtr<IDxcCompiler3> dxcCompiler,
	ComPtr<IDxcIncludeHandler> includeHandler)
{
	ComPtr<IDxcBlob> vsBlob = CompileShader(
		filePath, profile,dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get()
	);
	assert(vsBlob != nullptr);

	D3D12_SHADER_BYTECODE vsBytecode{};
	vsBytecode.pShaderBytecode = vsBlob->GetBufferPointer();
	vsBytecode.BytecodeLength = vsBlob->GetBufferSize();

	return vsBytecode;
}

ComPtr<IDxcBlob> CompileShaderFactory::CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	ComPtr<IDxcUtils> dxcUtils,
	ComPtr<IDxcCompiler3> dxcCompiler,
	ComPtr<IDxcIncludeHandler> includeHandler)
{
	//ここの中身をこの後書いて行く
	// 1
	//これからシェーダーをコンパイルする旨をログに出す
	Logger::Log(StringUtility::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知
	// 2
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定、基本的にmain以外にはしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug", //デバッグ用の情報を埋め込む
		L"-Od", //最適化を外しておく
		L"-Zpr", //メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルをする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer, //読み込んだファイル
		arguments,           //コンパイルオプション
		_countof(arguments), //コンパイルオプションの数
		includeHandler.Get(),      //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxc画起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
	// 3
	//警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Logger::Log(shaderError->GetStringPointer());
		//警告・エラーダメ絶対
		assert(false);
	}
	// 4
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Logger::Log(StringUtility::ConvertString(std::format(L"Compile Succeeded, path{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

std::unique_ptr<CompileShaderBase> CompileShaderFactory::GetCompileShaderPtr(PipelineType type, ComPtr<IDxcUtils> dxcUtils, ComPtr<IDxcCompiler3>& dxcCompiler, ComPtr<IDxcIncludeHandler> includeHandler)
{
	static std::unordered_map<PipelineType, std::unique_ptr<CompileShaderBase>> compileShader;

	switch (type) {
	case PipelineType::Object3d:
		compileShader[type] = std::make_unique<Object3dCompileShader>(dxcUtils, dxcCompiler, includeHandler);
		break;
	case PipelineType::Sprite:
		compileShader[type] = std::make_unique<SpriteCompileShader>(dxcUtils, dxcCompiler, includeHandler);
		break;
	case PipelineType::Line3d:
		compileShader[type] = std::make_unique<Line3dCompileShader>(dxcUtils, dxcCompiler, includeHandler);
		break;
	case PipelineType::Particle:
		compileShader[type] = std::make_unique<ParticleCompileShader>(dxcUtils, dxcCompiler, includeHandler);
		break;
	case PipelineType::PrimitiveDrawr:
		compileShader[type] = std::make_unique<PrimitiveDrawrCompileShader>(dxcUtils, dxcCompiler, includeHandler);
		break;
	case PipelineType::Animation:
		compileShader[type] = std::make_unique<AnimationCompileShader>(dxcUtils, dxcCompiler, includeHandler);
		break;
	case PipelineType::RenderTexture:
		compileShader[type] = std::make_unique<RenderTextureCompileShader>(dxcUtils, dxcCompiler, includeHandler);
		break;
	case PipelineType::Skybox:
		compileShader[type] = std::make_unique<SkyboxCompileShader>(dxcUtils, dxcCompiler, includeHandler);
		break;
	default:
		assert(false && "Invalid CompileShaderType");
		break;
	}

	return std::move(compileShader[type]);
}
