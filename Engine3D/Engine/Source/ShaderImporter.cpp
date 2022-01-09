#include "Application.h"
#include "ShaderImporter.h"
#include "Shader.h"
#include "Application.h"
#include "ModuleScene.h"
#include "FileSystem.h"
#include "GameObject.h"
#include "Globals.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "JsonConfig.h"

#include "Model.h"
#include "ResourceManager.h"
#include "Resource.h"

#include <stack>

#include "Profiling.h"

void ShaderImporter::ImportShader(std::string& path)
{
	RG_PROFILING_FUNCTION("Importing Shader");

	std::string p = path;

	if (ResourceManager::GetInstance()->CheckResource(path))
	{
		return;
	}
	else
	{
		char* buffer;
		int size = app->fs->Load(path.c_str(), &buffer);
		if (size <= 0)
		{
			delete[] buffer;
			LOG("Shader: %s not found or can't be loaded.", fullPath);
			return;
		}
		uint32 shaderID = 0;
		uint32 vertexID = 0;
		uint32 fragmentID = 0;
		std::vector<Uniform> uniforms;

		std::string file(buffer);
		if (file.find("__Vertex_Shader__") != std::string::npos)
		{
			vertexID = ImportVertex(file);
		}
		if (file.find("__Fragment_Shader__") != std::string::npos)
		{
			fragmentID = ImportFragment(file);
		}
		if (vertexID != 0 && fragmentID != 0)
		{
			GLint outcome;
			vertexID = (GLuint)vertexID;
			fragmentID = (GLuint)fragmentID;

			shaderID = glCreateProgram();
			glAttachShader(shaderID, vertexID);
			glAttachShader(shaderID, fragmentID);
			glLinkProgram(shaderID);

			glGetProgramiv(shaderID, GL_LINK_STATUS, &outcome);
			if (outcome == 0)
			{
				GLchar info[512];
				glGetProgramInfoLog(shaderID, 512, NULL, info);
				LOG("Shader compiling error: %s", info);
			}
			else if (uniforms.size() == 0)
			{
				uniforms = GetShaderUniforms(shaderID);
			}

			glDeleteShader(vertexID);
			glDeleteShader(fragmentID);
			glDeleteShader(shaderID);
		}
		else
		{
			LOG("ERROR, Vertex shader: &d or Fragment shader: %d are not correctly compiled.", shader->vertexID, shader->fragmentID);
		}
		if (app->fs->Save(path.c_str(), buffer, size) > 0)
			DEBUG_LOG("Shader saved!");
		delete[] buffer;

		std::string libraryPath;
		ResourceManager::GetInstance()->CreateResource(ResourceType::SHADERS, path, libraryPath);

		SaveShader(shaderID, vertexID, fragmentID, uniforms, libraryPath);
		/*std::string path1 = SHADERS_FOLDER + std::string("shader_") + std::to_string(1) + ".meta";
		losientomama(shaderID, vertexID, fragmentID, uniforms, path1);*/

	}
}
void ShaderImporter::SetShaderUniforms(Shader* shader)
{
	for (uint i = 0; i < shader->uniforms.size(); i++)
	{
		switch (shader->uniforms[i].uniformType)
		{
			//case UniformType::BOOL: shader->SetUniform1i(shader->uniforms[i].name.c_str(), shader->uniforms[i].boolean); break;
		case  UniformType::INT: shader->SetUniform1i(shader->uniforms[i].name.c_str(), shader->uniforms[i].integer); break;
		case  UniformType::FLOAT: shader->SetUniform1f(shader->uniforms[i].name.c_str(), shader->uniforms[i].floatNumber); break;
		case  UniformType::INT_VEC2: shader->SetUniformVec2f(shader->uniforms[i].name.c_str(), shader->uniforms[i].vec2.ptr()); break;
		case  UniformType::INT_VEC3: shader->SetUniformVec3f(shader->uniforms[i].name.c_str(), shader->uniforms[i].vec3.ptr()); break;
		case  UniformType::INT_VEC4: shader->SetUniformVec4f(shader->uniforms[i].name.c_str(), shader->uniforms[i].vec4.ptr()); break;
		case  UniformType::FLOAT_VEC2: shader->SetUniformVec2i(shader->uniforms[i].name.c_str(), (GLint*)shader->uniforms[i].vec2.ptr()); break;
		case  UniformType::FLOAT_VEC3: shader->SetUniformVec3i(shader->uniforms[i].name.c_str(), (GLint*)shader->uniforms[i].vec3.ptr()); break;
		case  UniformType::FLOAT_VEC4: shader->SetUniformVec4i(shader->uniforms[i].name.c_str(), (GLint*)shader->uniforms[i].vec4.ptr()); break;
		case UniformType::MATRIX4: shader->SetUniformMatrix4(shader->uniforms[i].name.c_str(), (GLfloat*)shader->uniforms[i].matrix4.Transposed().ptr()); break;
		default: break;
		}
	}
}
void ShaderImporter::SaveShader(uint32 shaderID, uint32 vertexID, uint32 fragmentID, std::vector<Uniform> uniforms, std::string path)
{
	JsonParsing metaShader;
	//JsonConfig jsonFile;
	//ArrayConfig jsonArrray = jsonFile.SetArray("Uniforms");

	metaShader.SetNewJsonNumber(metaShader.ValueToObject(metaShader.GetRootValue()), "ShaderID", shaderID);
	metaShader.SetNewJsonNumber(metaShader.ValueToObject(metaShader.GetRootValue()), "VertexID", vertexID);
	metaShader.SetNewJsonNumber(metaShader.ValueToObject(metaShader.GetRootValue()), "FragmentID", fragmentID);
	/*
	for (uint i = 0; i < uniforms.size(); i++)
	{
		JsonConfig& node = jsonArrray.AddNode();
		node.SetString("Name", uniforms[i].name);
		node.SetInteger("Type", (int)uniforms[i].uniformType);

		switch (uniforms[i].uniformType)
		{
			case  UniformType::BOOL: node.SetInteger("Value", uniforms[i].boolean); break;
			case  UniformType::INT: node.SetInteger("Value", uniforms[i].integer); break;
			case  UniformType::FLOAT: node.SetNumber("Value", uniforms[i].floatNumber); break;
			case  UniformType::INT_VEC2: node.SetFloat2("Value", uniforms[i].vec2); break;
			case  UniformType::INT_VEC3: node.SetFloat3("Value", uniforms[i].vec3); break;
			case  UniformType::INT_VEC4: node.SetFloat4("Value", uniforms[i].vec4); break;
			case  UniformType::FLOAT_VEC2: node.SetFloat2("Value", uniforms[i].vec2); break;
			case  UniformType::FLOAT_VEC3: node.SetFloat3("Value", uniforms[i].vec3); break;
			case  UniformType::FLOAT_VEC4: node.SetFloat4("Value", uniforms[i].vec4); break;
		}
	}*/
	char* buffer = nullptr;
	size_t size = metaShader.Save(&buffer);

	app->fs->Save(path.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);
}
std::vector<Uniform> ShaderImporter::GetShaderUniforms(uint32 shaderID)
{
	GLint activeUniforms;

	glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &activeUniforms);

	std::vector<Uniform> uniforms;

	for (uint i = 0; i < activeUniforms; i++)
	{
		Uniform uniform;
		GLint length;
		GLint size;
		GLchar name[32];
		glGetActiveUniform(shaderID, i, sizeof(name), &length, &size, &uniform.GLtype, name);
		uniform.name = name;
		if (uniform.name != "inColor" && uniform.name != "time" && uniform.name != "modelMatrix" && uniform.name != "viewMatrix" && uniform.name != "projectionMatrix")
		{
			uint uinformLoc = glGetUniformLocation(shaderID, uniform.name.c_str());

			switch (uniform.GLtype)
			{
			case GL_INT:
				uniform.uniformType = UniformType::INT;
				glGetUniformiv(shaderID, uinformLoc, &uniform.integer);
				break;
			case GL_FLOAT:
				uniform.uniformType = UniformType::FLOAT;
				glGetUniformfv(shaderID, uinformLoc, &uniform.floatNumber);
				break;
			case GL_BOOL:
				uniform.uniformType = UniformType::BOOL;
				break;
			case GL_INT_VEC2:
				uniform.uniformType = UniformType::INT_VEC2;
				glGetUniformiv(shaderID, uinformLoc, (GLint*)&uniform.vec2);
				break;
			case GL_INT_VEC3:
				uniform.uniformType = UniformType::INT_VEC3;
				glGetUniformiv(shaderID, uinformLoc, (GLint*)&uniform.vec3);
				break;
			case GL_INT_VEC4:
				uniform.uniformType = UniformType::INT_VEC4;
				glGetUniformiv(shaderID, uinformLoc, (GLint*)&uniform.vec4);
				break;
			case GL_FLOAT_VEC2:
				uniform.uniformType = UniformType::FLOAT_VEC2;
				glGetUniformfv(shaderID, uinformLoc, (GLfloat*)&uniform.vec2);
				break;
			case GL_FLOAT_VEC3:
				uniform.uniformType = UniformType::FLOAT_VEC3;
				glGetUniformfv(shaderID, uinformLoc, (GLfloat*)&uniform.vec3);
				break;
			case GL_FLOAT_VEC4:
				uniform.uniformType = UniformType::FLOAT_VEC4;
				glGetUniformfv(shaderID, uinformLoc, (GLfloat*)&uniform.vec4);
				break;
			case GL_FLOAT_MAT4:
				uniform.uniformType = UniformType::MATRIX4;
				glGetnUniformfv(shaderID, uinformLoc, sizeof(uniform.matrix4), &uniform.matrix4.v[0][0]);
				break;

			default: uniform.uniformType = UniformType::NONE; break;

			}

			if (uniform.uniformType != UniformType::NONE) uniforms.push_back(uniform);

		}


	}
	return uniforms;
}
int ShaderImporter::ImportVertex(std::string shaderFile)
{
	std::string vertexFile = std::string("#version 330 core\r\n") + std::string("#define ") + "__Vertex_Shader__" + "\r\n";
	std::string a = shaderFile;

	vertexFile += shaderFile;

	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* source = (const GLchar*)vertexFile.c_str();
	glShaderSource(vertexShader, 1, &source, nullptr);
	glCompileShader(vertexShader);

	GLint outcome;
	GLchar info[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &outcome);
	if (outcome == 0)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, info);
		DEBUG_LOG("Vertex shader compilation error (%s)", info);
	}



	return (outcome != 0) ? vertexShader : -1;
}
int ShaderImporter::ImportFragment(std::string shaderFile)
{
	std::string fragmentFile = std::string("#version 330 core\r\n") + std::string("#define ") + "__Fragment_Shader__" + "\r\n";
	fragmentFile += shaderFile;

	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* source = (const GLchar*)fragmentFile.c_str();
	glShaderSource(fragmentShader, 1, &source, nullptr);
	glCompileShader(fragmentShader);

	GLint outcome;
	GLchar info[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &outcome);
	if (outcome == 0)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, info);
		DEBUG_LOG("Vertex shader compilation error (%s)", info);
	}


	return (outcome != 0) ? fragmentShader : -1;
}
void ShaderImporter::CreateMetaShader(std::string& path, ShaderParameters& data, std::string& assets, uint uid, std::string& name, std::string& library)
{
	JsonParsing metaShader;
	const char* a = assets.c_str();
	metaShader.SetNewJsonNumber(metaShader.ValueToObject(metaShader.GetRootValue()), "Uuid", uid);
	metaShader.SetNewJsonString(metaShader.ValueToObject(metaShader.GetRootValue()), "Name", name.c_str());
	metaShader.SetNewJsonString(metaShader.ValueToObject(metaShader.GetRootValue()), "Type", "Shader");
	metaShader.SetNewJsonString(metaShader.ValueToObject(metaShader.GetRootValue()), "Assets Path", assets.c_str());
	metaShader.SetNewJsonString(metaShader.ValueToObject(metaShader.GetRootValue()), "Library Path", library.c_str());

	char* buffer = nullptr;
	size_t size = metaShader.Save(&buffer);

	app->fs->Save(path.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);
}
void ShaderImporter::losientomama(uint32 shaderID, uint32 vertexID, uint32 fragmentID, std::vector<Uniform> uniforms, std::string path)
{
	/*
	JsonConfig jsonFile;
	ArrayConfig jsonArrray = jsonFile.SetArray("Uniforms");

	for (uint i = 0; i < uniforms.size(); i++)
	{
		JsonConfig& node = jsonArrray.AddNode();
		node.SetString("Name", uniforms[i].name);
		node.SetInteger("Type", (int)uniforms[i].uniformType);
		switch (uniforms[i].uniformType)
		{
			//case  UniformType::BOOL: node.SetInteger("Value", shader->uniforms[i].boolean); break;
		case  UniformType::INT: node.SetInteger("Value", uniforms[i].integer); break;
		case  UniformType::FLOAT: node.SetNumber("Value", uniforms[i].floatNumber); break;
		case  UniformType::INT_VEC2: node.SetFloat2("Value", uniforms[i].vec2); break;
		case  UniformType::INT_VEC3: node.SetFloat3("Value", uniforms[i].vec3); break;
		case  UniformType::INT_VEC4: node.SetFloat4("Value", uniforms[i].vec4); break;
		case  UniformType::FLOAT_VEC2: node.SetFloat2("Value", uniforms[i].vec2); break;
		case  UniformType::FLOAT_VEC3: node.SetFloat3("Value", uniforms[i].vec3); break;
		case  UniformType::FLOAT_VEC4: node.SetFloat4("Value", uniforms[i].vec4); break;
		}
	}

	char** buffer = nullptr;
	uint size = jsonFile.SerializeConfig(buffer);
		
	app->fs->Save(path.c_str(), buffer, size);

	RELEASE_ARRAY(buffer);*/
}

