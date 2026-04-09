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
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;
		XMFLOAT3 vTangent;
	}VTXTEX;


	typedef struct Mesh_VIBUFFER
	{
		UINT m_iNumVertexBuffers = 1;
		UINT m_iNumVertices = 0;
		UINT m_iVertexStride = sizeof(VTXTEX);

		UINT m_iNumIndices = 0;
		UINT m_iIndexStride = 2;
		DXGI_FORMAT m_eIndexFormat = DXGI_FORMAT_R16_UINT;
		D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		shared_ptr<vector<VTXTEX>> vertices;
		shared_ptr<vector<uint16_t>> indices;
	}MESHVI;
}


#endif // Engine_Struct_h__
