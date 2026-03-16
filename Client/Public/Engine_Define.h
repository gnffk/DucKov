#pragma once
#include <Windows.h>
#include <iostream>
#include <memory>

using namespace std;

namespace Client {
	static const int g_winCX = 1280;
	static const int g_winCY = 720;

	#ifndef			MSG_BOX
	#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
	#define			MSG_BOX_STR(_message)			MessageBox(NULL, _message, L"System Message", MB_OK)
	#endif
	
	#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
	#define			END						}
	
	#define			USING(NAMESPACE)	using namespace NAMESPACE;
	
	#ifdef	ENGINE_EXPORTS
	#define ENGINE_DLL		_declspec(dllexport)
	#else
	#define ENGINE_DLL		_declspec(dllimport)
	#endif
}



#pragma warning(disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace std;
using namespace Client;