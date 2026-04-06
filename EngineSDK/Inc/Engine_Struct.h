#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc {
		HWND hWnd;
		WINMODE eWinMode;
		int32_t iWinSizeX, iWinSizeY;
		int32_t		iNumLevels;
	}ENGINE_DESC;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vTexcoord;
	}VTXTEX;

	typedef struct tagIndex32
	{
		unsigned long	_0;
		unsigned long	_1;
		unsigned long	_2;

	}INDEX32;

}


#endif // Engine_Struct_h__
