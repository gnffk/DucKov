#include "pch.h"
#include "Importer.h"

int main() {
	shared_ptr<Importer> import = make_shared<Importer>();

	import->LoadFolder("../../Resources/Fbx/");

}