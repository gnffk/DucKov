#include "BaseCollider.h"

BaseCollider::BaseCollider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext) : Component{ pDevice, pContext }
{
}

BaseCollider::~BaseCollider()
{
}

HRESULT BaseCollider::Initialize_Prototype() {

	return S_OK;
}
HRESULT BaseCollider::Initialize(void* pArg) {
	return S_OK;
}


HRESULT BaseCollider::Bind_Resources()
{



    return S_OK;
}

HRESULT BaseCollider::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch)
{

  
    return S_OK;

}
