#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

Animation::Animation()
{
}

Animation::~Animation()
{
}

HRESULT Animation::Initialize()
{
 
    return S_OK;
}

HRESULT Animation::Initialize(ifstream& _file)
{

    _file.read((char*)&m_fDuration, sizeof(_float));

    _file.read((char*)&m_fTickPerSecond, sizeof(_float));

    _file.read((char*)&m_iNumChannels, sizeof(uint32_t));

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);


    m_Channels.reserve(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        shared_ptr<Channel>    pChannel = Channel::Create(_file);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.emplace_back(pChannel);
    }

    return S_OK;
}

_bool Animation::Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<Bone>>& Bones, _bool isLoop)
{
   
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        if (true == isLoop)
            m_fCurrentTrackPosition = 0.f;
        else
            return true;
    }



    for (uint32_t i = 0; i < m_iNumChannels; ++i)
    {
        m_Channels[i]->Update_TransformationMatrix(m_CurrentKeyFrameIndices[i], m_fCurrentTrackPosition, Bones);
    }

    return false;
}

shared_ptr<Animation> Animation::Create()
{
    auto		pInstance = shared_ptr<Animation>(new Animation());

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : Animation");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Animation> Animation::Create(ifstream& _file)
{
    auto		pInstance = shared_ptr<Animation>(new Animation());

    if (FAILED(pInstance->Initialize(_file)))
    {
        MSG_BOX("Failed to Created : Animation");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Animation> Animation::Clone()
{
    return shared_ptr<Animation>(new Animation(*this));
}
