#pragma once
#include "Shader.h"

NS_BEGIN(Engine)

class ENGINE_DLL BaseShaderDX11 final : public ShaderDX11
{
protected:
	BaseShaderDX11(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context);

public:
	virtual ~BaseShaderDX11() = default;


public:
	virtual HRESULT SetShader() override;
	virtual HRESULT Recompile() override;

	virtual HRESULT Initialize() override;


public:
	static shared_ptr<BaseShaderDX11> Create(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context);
};

NS_END
