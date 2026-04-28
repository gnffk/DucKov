#include "Bone.h"

Bone::Bone()
{
}

Bone::~Bone()
{
}

HRESULT Bone::Initialize( int32_t iParentIndex)
{
    
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    m_iParentBoneIndex = iParentIndex;

    return S_OK;
}

void Bone::Update_CombinedTransformationMatrix(const vector<shared_ptr<Bone>>& Bones)
{
    if (-1 == m_iParentBoneIndex)
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix));

    XMStoreFloat4x4(&m_CombinedTransformationMatrix,
        XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

shared_ptr<Bone> Bone::Create( int32_t iParentIndex)
{
    auto		pInstance = shared_ptr<Bone>(new Bone());

    if (FAILED(pInstance->Initialize(iParentIndex)))
    {
        MSG_BOX("Failed to Created : Bone");
        return nullptr;
    }

    return pInstance;
}