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
		//XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;
		//XMFLOAT3 vTangent;

		static constexpr uint32_t		iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

	}VTXTEX;


	typedef struct Mesh_VIBUFFER
	{
		ComPtr<ID3D11Buffer>			m_pVB = { nullptr };
		ComPtr<ID3D11Buffer>			m_pIB = { nullptr };

		UINT m_iNumVertexBuffers = 1;
		UINT m_iNumVertices = 0;
		UINT m_iVertexStride = sizeof(VTXTEX);

		UINT m_iNumIndices = 0;
		UINT m_iIndexStride = sizeof(uint16_t);
		DXGI_FORMAT m_eIndexFormat = DXGI_FORMAT_R16_UINT;
		D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		shared_ptr<vector<VTXTEX>> vertices;
		shared_ptr<vector<uint16_t>> indices;
	}MESHVI;




	typedef struct CB_DATA
	{
		XMMATRIX matWorld;
		XMMATRIX matView;
		XMMATRIX matProj;


	}cbData;

}


#endif // Engine_Struct_h__
