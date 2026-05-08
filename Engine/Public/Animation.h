#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Animation final
{
private:
	Animation();
public:
	~Animation();

public:
	HRESULT Initialize();
	HRESULT Initialize(ifstream& _file);
	_bool Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<class Bone>>& Bones, _bool isLoop);
private:
	/* 이 애니메이션의 총 길이. */
	_float				m_fDuration = {};
	_float				m_fTickPerSecond = {};
	_float				m_fCurrentTrackPosition = {};

	/* 컨트롤해야하는 뼈의 갯수 */
	uint32_t							m_iNumChannels = {};
	vector<shared_ptr<class Channel>>	m_Channels;
	vector<uint32_t>					m_CurrentKeyFrameIndices;


public:
	static shared_ptr<Animation> Create();
	static shared_ptr<Animation> Create(ifstream& _file);
	shared_ptr<Animation> Clone();
};

NS_END

