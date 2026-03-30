#pragma once
#include "VIBuffer.h"

class FbxBuffer : public VIBuffer
{
protected:
	FbxBuffer() = default;
	virtual ~FbxBuffer() = default;

public:


	shared_ptr<Prototype> Clone(void* pArg) override;
};

