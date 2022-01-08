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
	void ImportShader(std::string& path);
	void SaveShader(uint32 shaderID, uint32 vertexID, uint32 fragmentID, std::vector<Uniform> uniforms, std::string path);
	std::vector<Uniform> GetShaderUniforms(uint32 vertexID);
	int ImportVertex(std::string shaderFile);
	int ImportFragment(std::string shaderFile);
	void CreateMetaShader(std::string& path, ShaderParameters& data, std::string& assets, uint uid, std::string& name);
	
	//borrar
	void losientomama(uint32 shaderID, uint32 vertexID, uint32 fragmentID, std::vector<Uniform> uniforms, std::string path);
}
