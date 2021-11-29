#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"


namespace MaterialImporter
{
	void Import(const aiMaterial* material, Material* ourMaterial);
	uint64 Save(const Material* ourMaterial, char** fileBuffer);
	void Load(const char* fileBuffer, Material* ourMaterial);
};