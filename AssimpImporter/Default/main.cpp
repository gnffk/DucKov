#include "pch.h"
#include "Importer.h"


int main() {
	shared_ptr<Importer> import = make_shared<Importer>();

	import->LoadFolder("../../Resources/Fbx/Dynamic/", MODEL::ANIM);
	import->LoadFolder("../../Resources/Fbx/Static/", MODEL::NONANIM);

}