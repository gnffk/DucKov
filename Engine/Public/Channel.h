#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Channel final
{
private:
	Channel();
public:
	~Channel();

public:
	HRESULT Initialize();
	HRESULT Initialize(ifstream& _file);
	void Update_TransformationMatrix(uint32_t& iCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<shared_ptr<class Bone>>& Bones);
private:
	char				m_szName[MAX_PATH] = {};
	int32_t				m_iBoneIndex = {};
	uint32_t			m_iNumKeyFrames = {};
	vector<KEYFRAME>	m_KeyFrames;


public:
	static shared_ptr<Channel> Create();
	static shared_ptr<Channel> Create(ifstream& _file);
};

NS_END