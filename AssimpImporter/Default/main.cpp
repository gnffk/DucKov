#include "pch.h"
#include "Importer.h"


int main() {
	shared_ptr<Importer> import = make_shared<Importer>();

	import->LoadFolder("../../Resources/Fbx/Dynamic/", MODEL::ANIM);
	import->LoadFolder("../../Resources/Fbx/Static/", MODEL::NONANIM);
	import->LoadFolder("../../Resources/Fbx/Static_Stage1/NONANIM/", MODEL::NONANIM);
	//import->LoadFolder("../../Resources/Fbx/Static/Stage1/NOANIM/", MODEL::NONANIM);
}