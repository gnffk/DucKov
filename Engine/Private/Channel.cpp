#include "Channel.h"
#include "Model.h"
#include "Bone.h"

Channel::Channel()
{
}

Channel::~Channel()
{
}

HRESULT Channel::Initialize()
{
    

    return S_OK;
}

HRESULT Channel::Initialize(ifstream& _file)
{
    _file.read((char*)&m_iBoneIndex, sizeof(int32_t));

    if (-1 == m_iBoneIndex)
        return E_FAIL;

    _file.read((char*)&m_iNumKeyFrames, sizeof(uint32_t));

  
    m_KeyFrames.reserve(m_iNumKeyFrames);
    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME            KeyFrame = {};

  


        _file.read((char*)&KeyFrame.vScale, sizeof(XMFLOAT3));
        _file.read((char*)&KeyFrame.vRotation, sizeof(XMFLOAT4));
        _file.read((char*)&KeyFrame.vTranslation, sizeof(XMFLOAT3));
        _file.read((char*)&KeyFrame.fTrackPosition, sizeof(float));


        m_KeyFrames.emplace_back(KeyFrame);
    }

    return S_OK;
}
void Channel::Update_TransformationMatrix(uint32_t& iCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<shared_ptr<Bone>>& Bones)
{
    if (0.f == fCurrentTrackPosition)
        iCurrentKeyFrameIndex = 0;

    KEYFRAME        LastKeyFrame = m_KeyFrames.back();

    _vector         vScale, vRotation, vTranslation;

    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    else
    {
        while (fCurrentTrackPosition >= m_KeyFrames[iCurrentKeyFrameIndex + 1].fTrackPosition)
            ++iCurrentKeyFrameIndex;

        _float      fRatio = (fCurrentTrackPosition - m_KeyFrames[iCurrentKeyFrameIndex].fTrackPosition) /
            (m_KeyFrames[iCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[iCurrentKeyFrameIndex].fTrackPosition);

        vScale = XMVectorLerp(
            XMLoadFloat3(&m_KeyFrames[iCurrentKeyFrameIndex].vScale),
            XMLoadFloat3(&m_KeyFrames[iCurrentKeyFrameIndex + 1].vScale),
            fRatio
        );

        vRotation = XMQuaternionSlerp(
            XMLoadFloat4(&m_KeyFrames[iCurrentKeyFrameIndex].vRotation),
            XMLoadFloat4(&m_KeyFrames[iCurrentKeyFrameIndex + 1].vRotation),
            fRatio
        );

        vTranslation = XMVectorSetW(XMVectorLerp(
            XMLoadFloat3(&m_KeyFrames[iCurrentKeyFrameIndex].vTranslation),
            XMLoadFloat3(&m_KeyFrames[iCurrentKeyFrameIndex + 1].vTranslation),
            fRatio
        ), 1.f);
    }

    _matrix         TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

shared_ptr<Channel> Channel::Create()
{
    auto		pInstance = shared_ptr<Channel>(new Channel());

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : Channel");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Channel> Channel::Create(ifstream& _file)
{
    auto		pInstance = shared_ptr<Channel>(new Channel());

    if (FAILED(pInstance->Initialize(_file)))
    {
        MSG_BOX("Failed to Created : Channel");
        return nullptr;
    }

    return pInstance;
}
