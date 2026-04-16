#include "Key_Manager.h"

Key_Manager::Key_Manager()
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_tMouseState, sizeof(m_tMouseState));

	ZeroMemory(m_bKeyUpState, sizeof(m_bKeyUpState));
	ZeroMemory(m_bKeyDownState, sizeof(m_bKeyDownState));
	ZeroMemory(m_bKeyPressingState, sizeof(m_bKeyPressingState));

	ZeroMemory(m_bMouseUpState, sizeof(m_bMouseUpState));
	ZeroMemory(m_bMouseDownState, sizeof(m_bMouseDownState));
	ZeroMemory(m_bMousePressingState, sizeof(m_bMousePressingState));
}



Key_Manager::~Key_Manager()
{
}


HRESULT Key_Manager::Initialize()
{

	
	return S_OK;
}

HRESULT Key_Manager::Ready_Key(HINSTANCE hInst, HWND hWnd) {
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(m_pInputSDK.GetAddressOf()),
		NULL)))
		return E_FAIL;


	if (FAILED(m_pInputSDK->CreateDevice(
		GUID_SysKeyboard,
		m_pKeyBoard.GetAddressOf(),
		nullptr)))
		return E_FAIL;



	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, m_pMouse.GetAddressOf(), nullptr)))
		return E_FAIL;


	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	return S_OK;

}

void Key_Manager::Update_InputDev(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

	// 키 체킹
	{
		ZeroMemory(m_bKeyUpState, sizeof(m_bKeyUpState));
		ZeroMemory(m_bKeyDownState, sizeof(m_bKeyDownState));
		_char keyDevState[256];
		m_pKeyBoard->GetDeviceState(256, keyDevState);

		for (int i = 0; i < 256; ++i)
		{
			bool nowPressed = keyDevState[i] & 0x80;

			// 이전에 눌렸는데 지금 안눌렸으면 UP
			if (m_bKeyPressingState[i] && !nowPressed)
			{
				m_bKeyUpState[i] = true;
			}
			// 이전에 안 눌렸는데 지금 눌렸으면 DOWN
			else if (!m_bKeyPressingState[i] && nowPressed)
			{
				m_bKeyDownState[i] = true;
			}

			m_bKeyPressingState[i] = nowPressed;
		}
	}

	// 마우스 체킹
	{
		ZeroMemory(m_bMouseUpState, sizeof(m_bMouseUpState));
		ZeroMemory(m_bMouseDownState, sizeof(m_bMouseDownState));
		DIMOUSESTATE mouseDevState;
		m_pMouse->GetDeviceState(sizeof(mouseDevState), &mouseDevState);

		for (int i = 0; i < DIM_END; ++i)
		{
			bool nowPressed = mouseDevState.rgbButtons[i] & 0x80;

			// 이전에 눌렸는데 지금 안눌렸으면 UP
			if (m_bMousePressingState[i] && !nowPressed)
			{
				m_bMouseUpState[i] = true;
			}
			// 이전에 안 눌렸는데 지금 눌렸으면 DOWN
			else if (!m_bMousePressingState[i] && nowPressed)
			{
				m_bMouseDownState[i] = true;
			}

			m_bMousePressingState[i] = nowPressed;
		}
	}
}


unique_ptr<Key_Manager> Key_Manager::Create()
{
	auto		pInstance = unique_ptr<Key_Manager>(new Key_Manager());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Key_Manager");
		return nullptr;
	}

	return pInstance;
}
