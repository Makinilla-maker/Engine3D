#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"
#include "ComponentMaterial.h"


namespace MaterialImporter
{
	void Import(const aiMaterial* material, ComponentMaterial* ourMaterial);
	uint64 Save(const ComponentMaterial* ourMaterial, char** fileBuffer);
	void Load(const char* fileBuffer, ComponentMaterial* ourMaterial);
};