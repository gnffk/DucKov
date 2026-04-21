#include "AABB_Collider.h"

AABB_Collider::AABB_Collider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext) : BaseCollider{pDevice, pContext}
{
}

AABB_Collider::~AABB_Collider()
{
}

HRESULT AABB_Collider::Intersect(BaseCollider* pCollider)
{
	return true;
}

HRESULT AABB_Collider::Intersect(_vector vPos, _vector vDir, float& pOutDist)
{
	return m_boudingBox.Intersects(vPos, vDir,OUT pOutDist);
}



HRESULT AABB_Collider::Initialize_Prototype()
{
    vertices = make_shared<vector<VTXTEX>>();
    indices = make_shared<vector<uint16_t>>();


    VTXTEX v;
    // Front (z = -0.5)
    v.vPosition = _float3(-0.5f, 0.5f, -0.5f); (*vertices).emplace_back(v); // 0
    v.vPosition = _float3(0.5f, 0.5f, -0.5f); (*vertices).emplace_back(v); // 1
    v.vPosition = _float3(0.5f, -0.5f, -0.5f); (*vertices).emplace_back(v); // 2
    v.vPosition = _float3(-0.5f, -0.5f, -0.5f); (*vertices).emplace_back(v); // 3

    // Back (z = +0.5)
    v.vPosition = _float3(-0.5f, 0.5f, 0.5f); (*vertices).emplace_back(v); // 4
    v.vPosition = _float3(0.5f, 0.5f, 0.5f); (*vertices).emplace_back(v); // 5
    v.vPosition = _float3(0.5f, -0.5f, 0.5f); (*vertices).emplace_back(v); // 6
    v.vPosition = _float3(-0.5f, -0.5f, 0.5f); (*vertices).emplace_back(v); // 7


    // Front
    indices->insert(indices->end(), { 0,1,2, 0,2,3 });

    // Back
    indices->insert(indices->end(), { 4,6,5, 4,7,6 });

    // Left
    indices->insert(indices->end(), { 4,0,3, 4,3,7 });

    // Right
    indices->insert(indices->end(), { 1,5,6, 1,6,2 });

    // Top
    indices->insert(indices->end(), { 4,5,1, 4,1,0 });

    // Bottom
    indices->insert(indices->end(), { 3,2,6, 3,6,7 });


    m_iNumVertexBuffers = 1;
    m_iNumVertices = (UINT)vertices->size();
    m_iVertexStride = sizeof(VTXTEX);

    m_iNumIndices = (UINT)indices->size();
    m_iIndexStride = 2;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;





    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = static_cast<UINT>(vertices->size() * sizeof(VTXTEX));
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA          VertexInitialData{};
    VertexInitialData.pSysMem = vertices->data();

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, m_pVB.GetAddressOf()))) {

        return E_FAIL;
    }





    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = static_cast<UINT>(indices->size() * sizeof(uint16_t));
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;



    D3D11_SUBRESOURCE_DATA          IndexInitialData{};
    IndexInitialData.pSysMem = indices->data();

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, m_pIB.GetAddressOf())))
        return E_FAIL;


    return S_OK;

}

HRESULT AABB_Collider::Initialize(void* pArg) {

	__super::Initialize(pArg);


	return S_OK;

}
void AABB_Collider::Update(float Timedelta) {

}
HRESULT AABB_Collider::Render() {

    return S_OK;
}

unique_ptr<AABB_Collider> AABB_Collider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) {
	auto		pInstance = unique_ptr<AABB_Collider>(new AABB_Collider(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AABB_Collider");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> AABB_Collider::Clone(void* pArg) {
	auto		pInstance = shared_ptr<AABB_Collider>(new AABB_Collider(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : AABB_Collider");
		return nullptr;
	}

	return pInstance;
}