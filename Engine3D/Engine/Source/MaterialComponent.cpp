#include "MaterialComponent.h"

#include "Application.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Texture.h"

#include "FileSystem.h"
#include "ShaderImporter.h"

#include "Imgui/imgui.h"

#include "Profiling.h"

MaterialComponent::MaterialComponent(GameObject* own) : diff(nullptr), showTexMenu(false), shader(nullptr)
{
	type = ComponentType::MATERIAL;
	owner = own;
	checker = false;
	shader = new Shader();
	showShaderMenu = false;
	
	active = true;
}

MaterialComponent::MaterialComponent(MaterialComponent* mat) : showTexMenu(false)
{
	checker = mat->checker;
	diff = mat->diff;

	shader = new Shader();
}

MaterialComponent::~MaterialComponent()
{
	if (diff.use_count() - 1 == 1) diff->UnLoad();
}

void MaterialComponent::OnEditor()
{
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader("Material"))
	{
		Checkbox(this, "Active", active);
		if (diff != nullptr)
		{
			ImGui::Text("Select texture: ");
			ImGui::SameLine();
			if (ImGui::Button(diff ? diff->GetName().c_str() : ""))
			{
				showTexMenu = true;
			}
			ImGui::Text("Path: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", diff->GetAssetsPath().c_str());
			ImGui::Text("Width: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", diff->GetWidth());
			ImGui::Text("Height: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", diff->GetHeight());
			ImGui::Text("Reference Count: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d (Warning: There's already one instance of it on the resources map)", diff.use_count());
			ImGui::Image((ImTextureID)diff->GetId(), ImVec2(128, 128));
		}
		else
		{
			ImGui::Text("Select texture: ");
			ImGui::SameLine();
			if (ImGui::Button("No Texture"))
			{
				showTexMenu = true;
			}
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "There's no texture");
			ImGui::Text("Width: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", 0);
			ImGui::Text("Height: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", 0);
		}
		if (shader != nullptr)
		{
			ImGui::Text("Select Shader: ");
			ImGui::SameLine();
			if (ImGui::Button(diff ? diff->GetName().c_str() : ""))
			{
				showShaderMenu = true;
			}
			ImGui::Text("Path: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", shader->GetPath().c_str());
			if (ImGui::Button("Default shader", { 200,25 }))
			{
				owner->GetComponent<MeshComponent>()->GetMaterial()->LoadShader("Assets\/Shaders\/DefaultShader.shader");
			}
			if (ImGui::Button("Water shader", { 200,25 }))
			{
				owner->GetComponent<MeshComponent>()->GetMaterial()->LoadShader("Assets\/Shaders\/WaterShader.shader");
			}
			if (ImGui::Button("Reflection Shader", { 200,25 }))
			{
				owner->GetComponent<MeshComponent>()->GetMaterial()->LoadShader("Assets\/Shaders\/ReflectionShader.shader");
			}
		}
		ImGui::Separator();
	}

	if (showTexMenu)
	{
		ImGui::Begin("Textures", &showTexMenu);
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 mouse = ImGui::GetIO().MousePos;
		if (!(mouse.x < winPos.x + size.x && mouse.x > winPos.x &&
			mouse.y < winPos.y + size.y && mouse.y > winPos.y))
		{
			if (ImGui::GetIO().MouseClicked[0]) showTexMenu = false;
		}

		std::vector<std::string> files;
		app->fs->DiscoverFiles("Library/Textures/", files);
		for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
		{
			if ((*it).find(".rgtexture") != std::string::npos)
			{
				app->fs->GetFilenameWithoutExtension(*it);
				*it = (*it).substr((*it).find_last_of("_") + 1, (*it).length());
				uint uid = std::stoll(*it);
				std::shared_ptr<Resource> res = ResourceManager::GetInstance()->GetResource(uid);
				if (ImGui::Selectable(res->GetName().c_str()))
				{
					res->Load();
					if (diff.use_count() - 1 == 1) diff->UnLoad();
					SetTexture(res);
				}
			}
		}

		ImGui::End();
	}

	
	ImGui::PopID();
}

bool MaterialComponent::OnLoad(JsonParsing& node)
{
	diff = std::static_pointer_cast<Texture>(ResourceManager::GetInstance()->LoadResource(std::string(node.GetJsonString("Path"))));
	active = node.GetJsonBool("Active");

	return true;
}

bool MaterialComponent::OnSave(JsonParsing& node, JSON_Array* array)
{
	JsonParsing file = JsonParsing();

	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "Type", (int)type);
	file.SetNewJsonString(file.ValueToObject(file.GetRootValue()), "Path", diff->GetAssetsPath().c_str());
	file.SetNewJsonBool(file.ValueToObject(file.GetRootValue()), "Active", active);
	
	node.SetValueToArray(array, file.GetRootValue());

	return true;
}

void MaterialComponent::BindTexture()
{
	if (diff) diff->Bind();
}

void MaterialComponent::UnbindTexture()
{
	if (diff) diff->Unbind();
}

void MaterialComponent::SetTexture(std::shared_ptr<Resource> tex)
{
	diff = std::static_pointer_cast<Texture>(tex);
}


void MaterialComponent::LoadShader(std::string path)
{
	char* buffer;
	int size = app->fs->Load(path.c_str(), &buffer);
	std::string a = path;
	if (size <= 0)
	{
		delete[] buffer;
		LOG("Shader: %s not found or can't be loaded.", fullPath);
		return;
	}
	std::string file(buffer);
	if (file.find("__Vertex_Shader__") != std::string::npos)
	{
		shader->parameters.vertexID = ShaderImporter::ImportVertex(file);
	}
	if (file.find("__Fragment_Shader__") != std::string::npos)
	{
		shader->parameters.fragmentID = ShaderImporter::ImportFragment(file);
	}
	if (shader->parameters.vertexID != 0 && shader->parameters.fragmentID != 0)
	{
		GLint outcome;
		shader->parameters.vertexID = (GLuint)shader->parameters.vertexID;
		shader->parameters.fragmentID = (GLuint)shader->parameters.fragmentID;

		shader->parameters.shaderID = glCreateProgram();
		glAttachShader(shader->parameters.shaderID, shader->parameters.vertexID);
		glAttachShader(shader->parameters.shaderID, shader->parameters.fragmentID);
		glLinkProgram(shader->parameters.shaderID);

		glGetProgramiv(shader->parameters.shaderID, GL_LINK_STATUS, &outcome);
		if (outcome == 0)
		{
			GLchar info[512];
			glGetProgramInfoLog(shader->parameters.shaderID, 512, NULL, info);
			LOG("Shader compiling error: %s", info);
		}
		else if (shader->parameters.uniforms.size() == 0)
		{
			shader->parameters.uniforms = ShaderImporter::GetShaderUniforms(shader->parameters.shaderID);
		}

		glDeleteShader(shader->parameters.vertexID);
		glDeleteShader(shader->parameters.fragmentID);
		glDeleteShader(shader->parameters.shaderID);
	}
	else
	{
		LOG("ERROR, Vertex shader: &d or Fragment shader: %d are not correctly compiled.", shader->vertexID, shader->fragmentID);
	}
	delete[] buffer;
}
