#include "Fbx_Resource.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

Fbx_Resource::Fbx_Resource(wstring filename) : m_wsfilename(filename)
{
}

Fbx_Resource::~Fbx_Resource()
{
}

HRESULT Fbx_Resource::Load() {
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(m_wsfilename,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->directory_ = m_wsfilename.substr(0, m_wsfilename.find_last_of("/\\"));

	this->dev_ = dev;
	this->devcon_ = devcon;
	this->hwnd_ = hwnd;

	processNode(pScene->mRootNode, pScene);

	return true;
}
