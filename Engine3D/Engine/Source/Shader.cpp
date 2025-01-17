#include "Application.h"
#include "FileSystem.h"
#include "Shader.h"
#include "ShaderImporter.h"

Shader::Shader() : Resource(ResourceType::SHADERS)
{

}

Shader::Shader(uint uid, std::string& assets, std::string& library) : Resource(uid, ResourceType::SHADERS, assets, library), parameters({})
{
	name = assets;
	app->fs->GetFilenameWithoutExtension(name);
	std::string metaPath = SHADERS_FOLDER + std::string("shader_") + std::to_string(uid) + ".meta";
	ShaderImporter::CreateMetaShader(metaPath, parameters, assets, uid, name, library);
}

Shader::~Shader()
{

}

void Shader::Load()
{

}

void Shader::SetMatrix4(std::string name, GLfloat* value)
{
	int uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniformMatrix4fv(uinformLoc, 1, GL_FALSE, value);
}

void Shader::SetVec2f(std::string name, GLfloat* value)
{
	uint uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniform2fv(uinformLoc, 1, value);
}

void Shader::SetVec3f(std::string name, GLfloat* value)
{
	uint uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniform3fv(uinformLoc, 1, value);
}

void Shader::SetVec4f(std::string name, GLfloat* value)
{
	uint uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniform4fv(uinformLoc, 1, value);
}

void Shader::SetVec2i(std::string name, GLint* value)
{
	uint uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniform2iv(uinformLoc, 1, value);
}

void Shader::SetVec3i(std::string name, GLint* value)
{
	uint uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniform3iv(uinformLoc, 1, value);
}

void Shader::SetVec4i(std::string name, GLint* value)
{
	uint uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniform4iv(uinformLoc, 1, value);
}

void Shader::Set1f(std::string name, GLfloat value)
{
	uint uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniform1f(uinformLoc, value);
}

void Shader::Set1i(std::string name, GLint value)
{
	uint uinformLoc = glGetUniformLocation(parameters.shaderID, name.c_str());
	glUniform1i(uinformLoc, value);
}

