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

	// shader

	typedef struct tagBaseShader
	{
		wstring m_FileName;
		string m_VSEntry;
		string m_PSEntry;

		UINT m_StencilRef = 1;
		UINT m_SamplerSlot = 0;

		shared_ptr<vector<D3D11_INPUT_ELEMENT_DESC>> m_InputDescs;
		UINT m_NumElements = 0;
	}BASESHADER_DESC;
}


#endif // Engine_Struct_h__
