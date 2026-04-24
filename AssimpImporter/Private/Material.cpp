#include "Material.h"

Material::Material() {

}

Material::~Material() {

}

HRESULT Material::Set_Material(uint32_t _materialtype, vector<TEXTUREINFO> _textures)
{
	m_materialtype = _materialtype ;
	m_texturesSize = _textures.size();
	m_textures = _textures;

	return S_OK;
}


