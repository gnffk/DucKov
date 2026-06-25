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

	FMOD_System_SetDSPBufferSize(m_pSystem, 256, 4);

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

void Sound_Manager::PlaySoundLoop(std::wstring_view svSoundKey, CHANNELID eID, float fVolume)
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
		FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);
		FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
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
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void Sound_Manager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void Sound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	//FMOD_System_Update(m_pSystem);
}

void Sound_Manager::UpdateSound()
{
	FMOD_System_Update(m_pSystem);
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
