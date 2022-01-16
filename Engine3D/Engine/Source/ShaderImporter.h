#pragma once

#include <string>

#include "MathGeoLib/src/Math/float2.h"
#include "MathGeoLib/src/Math/float3.h"

#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "Shader.h"

class JsonParsing;
class Shader;
struct ModelParameters;

typedef unsigned int uint;

namespace ShaderImporter
{
	void ImportShader(std::string path);
	void SetShaderUniforms(Shader* shader);
	std::vector<Uniform> GetShaderUniforms(uint32 vertexID);
	int ImportVertex(std::string shaderFile);
	int ImportFragment(std::string shaderFile);
	void CreateMetaShader(std::string& path, ShaderParameters& data, std::string& assets, uint uid, std::string& name, std::string& library);

}
