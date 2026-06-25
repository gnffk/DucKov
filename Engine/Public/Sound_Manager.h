#pragma once

#include "Engine_Defines.h"

struct FMOD_SOUND;
struct FMOD_CHANNEL;
struct FMOD_SYSTEM;

NS_BEGIN(Engine)

class Sound;

class Sound_Manager final
{
private:
	Sound_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	~Sound_Manager();

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

public:


public:
	FMOD_SYSTEM* Get_System() const { return m_pSystem; }

public:
	HRESULT Ready_SoundMgr();

public:
	HRESULT Add_Sound(std::wstring_view svSoundKey, std::wstring_view svSoundPath);
	shared_ptr<Sound> Find_Sound(std::wstring_view svSoundKey);

public:
	void PlaySoundLoop(std::wstring_view svSoundKey, CHANNELID eID, float fVolume);
	void PlaySoundOne(std::wstring_view svSoundKey, CHANNELID eID, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

	bool IsChannelPlaying(CHANNELID eID);

	void UpdateSound();

private:
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL]{};
	FMOD_SYSTEM* m_pSystem = nullptr;

private:
	std::map<std::wstring, shared_ptr<Sound>> m_Sounds;

private:
	void Free();

public:
	static unique_ptr<Sound_Manager> Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);
};

NS_END