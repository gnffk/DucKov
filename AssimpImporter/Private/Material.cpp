#include "Material.h"

Material::Material() {

}

Material::~Material() {

}

HRESULT Material::Set_Material(uint32_t _materialNum, vector<vector<TEXTUREINFO>> _textures)
{
	m_materialNum = _materialNum;

	m_textures = _textures;

	return S_OK;
}


