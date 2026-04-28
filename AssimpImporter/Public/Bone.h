#pragma once
#include "pch.h"


class Bone
{
public:
	Bone();

	~Bone();


	bool Compare_Name(string pBoneName) {
		if (pBoneName == m_name) {
			return true;
		}
		else {
			return false;
		}
	}
	string m_name;
	int32_t m_patrentBoneIndex;
	XMFLOAT4X4 m_TransformationMatrix;
};

