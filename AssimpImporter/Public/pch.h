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

typedef struct FileHeader
{
	uint32_t magic;
	uint32_t version = 1;
}FILEHEADER;

typedef struct ChunkHeader
{
	uint32_t type;
	uint32_t size;
}CHUCKHEADER;

enum FileHeaderType { FILEHEADER_MODEL,END};
enum ChunkType { CHUNK_MESH = 1, CHUNK_MATERIAL, CHUNK_TEXTURE, CHUNK_SKELETON, CHUNK_ANIMATION };


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


// Material Á¤º¸ <BIN>
enum MATERIALTYPE {
	NONE = 0, DIFFUSE = 1, SPECULAR = 2, AMBIENT = 3, EMISSIVE = 4, HEIGHT = 5, NORMALS = 6, SHININESS = 7, OPACITY = 8, DISPLACEMENT = 9, LIGHTMAP = 10, REFLECTION = 11, BASE_COLOR = 12,
	NORMAL_CAMERA = 13, EMISSION_COLOR = 14, METALNESS = 15, DIFFUSE_ROUGHNESS = 16, AMBIENT_OCCLUSION = 17, UNKNOWN = 18, SHEEN = 19, CLEARCOAT = 20, TRANSMISSION = 21, MAYA_BASE = 22,
	MAYA_SPECULAR = 23, MAYA_SPECULAR_COLOR = 24, MAYA_SPECULAR_ROUGHNESS = 25, ANISOTROPY = 26, GLTF_METALLIC_ROUGHNESS = 27, MATERIAL_END
};

typedef struct tagTexture {
	uint32_t m_textureType;
	uint32_t m_textureNum;
	string File;
	string Ext;
}TEXTUREINFO;

