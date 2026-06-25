#pragma once

#include "Engine_Defines.h"
#include "fmod.h"

NS_BEGIN(Engine)

class Sound final
{
private:
	Sound(std::wstring_view svSoundPath);

public:
	~Sound();

public:
	FMOD_SOUND* Get_FmodSound() const {
		return m_pFmodSound;
	}

public:
	HRESULT Load(FMOD_SYSTEM* pSystem);
	void Unload();

public:
	static shared_ptr<Sound> Create(FMOD_SYSTEM* pSystem, std::wstring_view svSoundPath);

private:
	void Free();

private:
	std::wstring m_sSoundPath{};
	FMOD_SOUND* m_pFmodSound = nullptr;
};

NS_END