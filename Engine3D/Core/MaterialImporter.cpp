#include "Globals.h"
#include "MaterialImporter.h"

#include "DevIL/include/il.h"
#include "Devil/include/ilut.h"
#include <memory>
#include <vector>

void MaterialImporter::Import(const aiMaterial* material, ComponentMaterial* ourMaterial)
{
}

uint64_t MaterialImporter::Save(const ComponentMaterial* ourMaterial, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer
	if (size > 0) {
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0)
		{
			// Save to buffer with the ilSaveIL function
			*fileBuffer = (char*)data;
		}

		RELEASE_ARRAY(data);
	}

	return uint64_t();
}

void MaterialImporter::Load(const char* fileBuffer, ComponentMaterial* ourMaterial)
{
}

