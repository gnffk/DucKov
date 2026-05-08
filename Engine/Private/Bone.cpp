#include "Bone.h"

Bone::Bone()
{
}

Bone::~Bone()
{
}

HRESULT Bone::Initialize(const _char* name, _float4x4 TransformMatrix, int32_t ParentBoneIndex)
{
    strcpy_s(m_szName, name);

    memcpy(&m_TransformationMatrix, &TransformMatrix, sizeof(_float4x4));

    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    m_iParentBoneIndex = ParentBoneIndex;

    return S_OK;
}

void Bone::Update_CombinedTransformationMatrix(const vector<shared_ptr<Bone>>& Bones, _fmatrix PreTransformMatrix)
{
    if (-1 == m_iParentBoneIndex) {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
        return;
    }
        

    XMStoreFloat4x4(&m_CombinedTransformationMatrix,
        XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

shared_ptr<Bone> Bone::Create(const _char* name, _float4x4 TransformMatrix, int32_t ParentBoneIndex)
{
    auto		pInstance = shared_ptr<Bone>(new Bone());

    if (FAILED(pInstance->Initialize(name, TransformMatrix, ParentBoneIndex)))
    {
        MSG_BOX("Failed to Created : Bone");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Bone> Bone::Clone()
{
    return shared_ptr<Bone>(new Bone(*this));
}
