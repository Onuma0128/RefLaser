#include "ModelBase.h"

#include "DirectXEngine.h"

ModelBase* ModelBase::instance_ = nullptr;

ModelBase* ModelBase::GetInstance()
{
	if (instance_ == nullptr) {
		instance_ = new ModelBase;
	}
	return instance_;
}

void ModelBase::Initialize(DirectXEngine* dxEngine)
{
	dxEngine_ = dxEngine;
}

void ModelBase::Finalize()
{
	delete instance_;
	instance_ = nullptr;
}
