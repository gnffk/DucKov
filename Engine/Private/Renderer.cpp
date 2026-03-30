#include "Renderer.h"

Renderer::Renderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
}

Renderer::~Renderer()
{
}

HRESULT Renderer::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject)
{
    return E_NOTIMPL;
}

HRESULT Renderer::Draw()
{
    return E_NOTIMPL;
}

unique_ptr<Renderer> Renderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return unique_ptr<Renderer>();
}
