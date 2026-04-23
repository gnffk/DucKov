#pragma once
#include <filesystem>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
using namespace DirectX;
#include  <any>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <ctime>
#include <memory>
#include <wrl/client.h>
#include <fstream>
using namespace Microsoft::WRL;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

#define			ETOI(_enum)			static_cast<int32_t>(_enum)
#define			ETOUI(_enum)		static_cast<uint32_t>(_enum)

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif


typedef struct tagVertexMesh
{
	XMFLOAT3	vPosition;
	XMFLOAT3	vNormal;
	XMFLOAT3	vTangent;
	XMFLOAT3	vBinormal;
	XMFLOAT2	vTexcoord;


	//static constexpr uint32_t		iNumElements = { 5 };
	//static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
	//};
}VTXMESH;

