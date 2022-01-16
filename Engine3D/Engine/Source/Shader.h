#ifndef __Shader_H__
#define __Shader_H__

#include "Globals.h"
#include "Resource.h"
#include "glew/include/GL/glew.h"
#include"MathGeoLib/src/Math/float2.h"
#include"MathGeoLib/src/Math/float3.h"
#include"MathGeoLib/src/Math/float4.h"
#include"MathGeoLib/src/Math/float4x4.h"

enum class UniformType
{
	INT,
	FLOAT,
	BOOL,
	INT_VEC2,
	INT_VEC3,
	INT_VEC4,
	FLOAT_VEC2,
	FLOAT_VEC3,
	FLOAT_VEC4,
	MATRIX4,
	NONE
};
struct Uniform
{
	UniformType uniformType = UniformType::NONE;
	std::string name;
	GLenum GLtype;

	bool boolean;
	GLint integer;
	GLfloat floatNumber;
	float2 vec2;
	float3 vec3;
	float4 vec4;
	float4x4 matrix4 = float4x4::zero;

};

struct ShaderParameters
{
	uint32 shaderID = 0;
	uint32 vertexID = 0;
	uint32 fragmentID = 0;
	uint uid;
	std::string path;
	std::string name;

	std::vector<Uniform> uniforms;
};

class Shader : public Resource
{
public:

	Shader();
	Shader(uint uid, std::string& assets, std::string& library);
	~Shader();

	void SetMatrix4(std::string name, GLfloat* value);
	void SetVec2f(std::string name, GLfloat* value);
	void SetVec3f(std::string name, GLfloat* value);
	void SetVec4f(std::string name, GLfloat* value);

	void SetVec2i(std::string name, GLint* value);
	void SetVec3i(std::string name, GLint* value);
	void SetVec4i(std::string name, GLint* value);

	void Set1f(std::string name, GLfloat value);
	void Set1i(std::string name, GLint value);

	std::string GetPath() { return parameters.path; }

	void Load() override;

public:

	ShaderParameters parameters;

};

#endif //__ResourceShader_H__
