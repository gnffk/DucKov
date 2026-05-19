#include "PartObject.h"
#include "GameInstance.h"

PartObject::PartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : GameObject{ pDevice, pContext }
{
}

PartObject::PartObject(const PartObject& Prototype)
    : GameObject{ Prototype }
{
}

PartObject::~PartObject()
{
}

HRESULT PartObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT PartObject::Initialize(void* pArg)
{
    auto        pDesc = static_cast<PARTOBJECT_DESC*>(pArg);

    m_pParentMatrix = pDesc->pParentMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void PartObject::Priority_Update(_float fTimeDelta)
{
}

void PartObject::Update(_float fTimeDelta)
{
}

void PartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT PartObject::Render()
{
    return S_OK;
}

void PartObject::Make_CombinedWorldMatrix(_fmatrix ChildMatrix)
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        ChildMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

HRESULT PartObject::Bind_WorldMatrix(shared_ptr<class Shader>  pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_CombinedWorldMatrix);
}
