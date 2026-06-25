#include <io.h>
#include "Sound_Manager.h" 
#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")
#include "Sound.h"


Sound_Manager::Sound_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	ZeroMemory(&m_pChannelArr, sizeof(m_pChannelArr));
}

Sound_Manager::~Sound_Manager()
{
	Free();
}

HRESULT Sound_Manager::Ready_SoundMgr()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	FMOD_System_SetDSPBufferSize(m_pSystem, 1024, 4);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	return S_OK;
}


HRESULT Sound_Manager::Add_Sound(std::wstring_view svSoundKey, std::wstring_view svSoundPath)
{
	if (nullptr == m_pSystem)
		return E_FAIL;

	std::wstring strKey{ svSoundKey };

	if (m_Sounds.contains(strKey))
		return S_OK;

	shared_ptr<Sound> pSound = Sound::Create(m_pSystem, svSoundPath);

	if (nullptr == pSound)
		return E_FAIL;

	m_Sounds.emplace(strKey, pSound);

	return S_OK;
}

shared_ptr<Sound> Sound_Manager::Find_Sound(std::wstring_view svSoundKey)
{
	auto iter = m_Sounds.find(std::wstring{ svSoundKey });

	if (iter == m_Sounds.end())
		return nullptr;

	return iter->second;
}

void Sound_Manager::PlaySoundLoop(
	std::wstring_view svSoundKey,
	CHANNELID eID,
	float fVolume,
	float fPitch
)
{
	shared_ptr<Sound> pSoundObj = Find_Sound(svSoundKey);

	if (nullptr == pSoundObj)
		return;

	FMOD_SOUND* pSound = pSoundObj->Get_FmodSound();

	if (nullptr == pSound)
		return;

	FMOD_System_PlaySound(
		m_pSystem,
		pSound,
		nullptr,
		TRUE,
		&m_pChannelArr[eID]
	);

	if (m_pChannelArr[eID])
	{
		FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);
		FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

		float fFrequency = 0.f;
		FMOD_Channel_GetFrequency(m_pChannelArr[eID], &fFrequency);
		FMOD_Channel_SetFrequency(m_pChannelArr[eID], fFrequency * fPitch);

		FMOD_Channel_SetPaused(m_pChannelArr[eID], FALSE);
	}
}

void Sound_Manager::PlaySoundOne(std::wstring_view svSoundKey, CHANNELID eID, float fVolume)
{
	shared_ptr<Sound> pSoundObj = Find_Sound(svSoundKey);

	if (nullptr == pSoundObj)
		return;

	FMOD_SOUND* pSound = pSoundObj->Get_FmodSound();

	if (nullptr == pSound)
		return;

	FMOD_System_PlaySound(m_pSystem, pSound, nullptr, TRUE, &m_pChannelArr[eID]);

	if (m_pChannelArr[eID])
	{
		FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_DEFAULT);
		FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
		FMOD_Channel_SetPaused(m_pChannelArr[eID], FALSE);
	}
}

bool Sound_Manager::IsChannelPlaying(CHANNELID eID)
{
	FMOD_BOOL bPlay = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay);
	return bPlay;
}

void Sound_Manager::StopSound(CHANNELID eID)
{
	if (eID < 0 || eID >= MAXCHANNEL)
		return;

	if (m_pChannelArr[eID])
	{
		FMOD_Channel_Stop(m_pChannelArr[eID]);
		m_pChannelArr[eID] = nullptr;
	}

	m_FadeDescs[eID].bFading = false;
}

void Sound_Manager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
	{
		if (m_pChannelArr[i])
		{
			FMOD_Channel_Stop(m_pChannelArr[i]);
			m_pChannelArr[i] = nullptr;
		}

		m_FadeDescs[i].bFading = false;
	}
}

void Sound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	//FMOD_System_Update(m_pSystem);
}

void Sound_Manager::UpdateSound(float fTimeDelta)
{
	for (int i = 0; i < MAXCHANNEL; ++i)
	{
		if (m_FadeDescs[i].bFading == false)
			continue;

		if (m_pChannelArr[i] == nullptr)
		{
			m_FadeDescs[i].bFading = false;
			continue;
		}

		FMOD_BOOL bPlaying = FALSE;
		FMOD_Channel_IsPlaying(m_pChannelArr[i], &bPlaying);

		if (bPlaying == FALSE)
		{
			m_FadeDescs[i].bFading = false;
			continue;
		}

		FADE_DESC& FadeDesc = m_FadeDescs[i];

		FadeDesc.fAccTime += fTimeDelta;

		float fRatio = FadeDesc.fAccTime / FadeDesc.fDuration;
		fRatio = min(max(fRatio, 0.f), 1.f);

		float fVolume =
			FadeDesc.fStartVolume +
			(FadeDesc.fTargetVolume - FadeDesc.fStartVolume) * fRatio;

		FMOD_Channel_SetVolume(m_pChannelArr[i], fVolume);

		if (fRatio >= 1.f)
		{
			FadeDesc.bFading = false;

			if (FadeDesc.bStopAfterFade)
			{
				FMOD_Channel_Stop(m_pChannelArr[i]);
				m_pChannelArr[i] = nullptr;
			}
		}
	}

	if (m_pSystem)
		FMOD_System_Update(m_pSystem);
}
void Sound_Manager::FadeToVolume(CHANNELID eID, float fTargetVolume, float fDuration)
{
	if (eID < 0 || eID >= MAXCHANNEL)
		return;

	if (m_pChannelArr[eID] == nullptr)
		return;

	FMOD_BOOL bPlaying = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlaying);

	if (bPlaying == FALSE)
		return;

	float fCurVolume = 0.f;
	FMOD_Channel_GetVolume(m_pChannelArr[eID], &fCurVolume);

	m_FadeDescs[eID].bFading = true;
	m_FadeDescs[eID].fStartVolume = fCurVolume;
	m_FadeDescs[eID].fTargetVolume = fTargetVolume;
	m_FadeDescs[eID].fDuration = max(fDuration, 0.0001f);
	m_FadeDescs[eID].fAccTime = 0.f;
	m_FadeDescs[eID].bStopAfterFade = false;
}

void Sound_Manager::FadeOutSound(CHANNELID eID, float fDuration)
{
	if (eID < 0 || eID >= MAXCHANNEL)
		return;

	if (m_pChannelArr[eID] == nullptr)
		return;

	FMOD_BOOL bPlaying = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlaying);

	if (bPlaying == FALSE)
		return;

	float fCurVolume = 0.f;
	FMOD_Channel_GetVolume(m_pChannelArr[eID], &fCurVolume);

	m_FadeDescs[eID].bFading = true;
	m_FadeDescs[eID].fStartVolume = fCurVolume;
	m_FadeDescs[eID].fTargetVolume = 0.f;
	m_FadeDescs[eID].fDuration = max(fDuration, 0.0001f);
	m_FadeDescs[eID].fAccTime = 0.f;
	m_FadeDescs[eID].bStopAfterFade = true;
}


void Sound_Manager::Free()
{
	StopAll();

	m_Sounds.clear();

	if (m_pSystem)
	{
		FMOD_System_Close(m_pSystem);
		FMOD_System_Release(m_pSystem);
		m_pSystem = nullptr;
	}
}

unique_ptr<Sound_Manager> Sound_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{

	return unique_ptr<Sound_Manager>(new Sound_Manager(pDevice, pContext));
}
