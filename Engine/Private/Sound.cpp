#include "Sound.h"
#include "GameInstance.h"
NS_BEGIN(Engine)

Sound::Sound(std::wstring_view svSoundPath): m_sSoundPath{ svSoundPath }
{
}

Sound::~Sound()
{
	Free();
}

HRESULT Sound::Load(FMOD_SYSTEM* pSystem)
{
	if (nullptr == pSystem)
		return E_FAIL;

	if (m_sSoundPath.empty())
		return E_FAIL;

	if (m_pFmodSound != nullptr)
		return S_OK;

	FMOD_RESULT eResult = FMOD_System_CreateSound(
		pSystem,
		CGameInstance::Get().WStringToString(m_sSoundPath).c_str(),
		FMOD_DEFAULT,
		nullptr,
		&m_pFmodSound
	);

	if (eResult != FMOD_OK)
		return E_FAIL;

	return S_OK;
}

void Sound::Unload()
{
	Free();
}

shared_ptr<Sound> Sound::Create(FMOD_SYSTEM* pSystem, std::wstring_view svSoundPath)
{
	shared_ptr<Sound> pInstance = shared_ptr<Sound>(new Sound{ svSoundPath });

	if (FAILED(pInstance->Load(pSystem)))
	{
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

void Sound::Free()
{
	if (m_pFmodSound)
	{
		FMOD_Sound_Release(m_pFmodSound);
		m_pFmodSound = nullptr;
	}
}

NS_END