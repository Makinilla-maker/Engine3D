#include "MaterialComponent.h"

#include "Application.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "ModuleEditor.h"
#include "Texture.h"

#include "FileSystem.h"
#include "ShaderImporter.h"

#include "Imgui/imgui.h"

#include "Profiling.h"

#include "glew/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"

#include <map>

MaterialComponent::MaterialComponent(GameObject* own) : diff(nullptr), showTexMenu(false)
{
	type = ComponentType::MATERIAL;
	owner = own;
	checker = false;
	showShaderMenu = false;

	
	active = true;
}

MaterialComponent::MaterialComponent(MaterialComponent* mat) : showTexMenu(false)
{
	checker = mat->checker;
	diff = mat->diff;
	showShaderMenu = false;
}

MaterialComponent::~MaterialComponent()
{
	if (diff.use_count() - 1 == 1) diff->UnLoad();
}

void MaterialComponent::OnEditor()
{
	MeshComponent* meshCom = owner->GetComponent<MeshComponent>();
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
		if (meshCom->GetShader() != nullptr)
		{
			ImGui::Text("Selected Shader: ");
			ImGui::SameLine();
			if (ImGui::Button(meshCom->GetShader() ? meshCom->GetShader()->parameters.name.c_str() : ""))
			{
				showShaderMenu = true;
			}
			ImGui::Text("Path: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", meshCom->GetShader()->GetPath().c_str());
			if (ImGui::Button("Edit Shader", { 100,25 }))	meshCom->EditorShader();
			ShowUniforms();
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
	if (showShaderMenu)
	{
		ImGui::Begin("Shaders", &showShaderMenu);
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 mouse = ImGui::GetIO().MousePos;
		if (!(mouse.x < winPos.x + size.x && mouse.x > winPos.x &&
			mouse.y < winPos.y + size.y && mouse.y > winPos.y))
		{
			if (ImGui::GetIO().MouseClicked[0]) showShaderMenu = false;
		}
		std::map<uint, std::string> allShaders = ResourceManager::GetInstance()->GetShaderMap();
		std::map<uint, std::string>::iterator it;
		for (it = allShaders.begin(); it != allShaders.end(); ++it)
		{
			std::string a = meshCom->GetNamefromPath((*it).second);
			if (ImGui::Selectable(a.c_str()))//, { 200,25 }))
			{
				meshCom->LoadShader((*it).second.c_str());
			}
		}
		ImGui::End();
	}
	if (meshCom->showShaderEditor)
	{
		ImGui::Begin("Shader Editor", &meshCom->showShaderEditor, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);
		//Update
		auto cpos = meshCom->editor.GetCursorPosition();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Button("Save"))
			{
				std::string shader = meshCom->editor.GetText();

				app->fs->RemoveFile(meshCom->fileToEdit.c_str());
				app->fs->Save(meshCom->fileToEdit.c_str(), shader.c_str(), meshCom->editor.GetText().size());

				glDetachShader(meshCom->shadertoRecompily->parameters.shaderID, meshCom->shadertoRecompily->parameters.vertexID);
				glDetachShader(meshCom->shadertoRecompily->parameters.shaderID, meshCom->shadertoRecompily->parameters.fragmentID);
				glDeleteProgram(meshCom->shadertoRecompily->parameters.shaderID);

				meshCom->LoadShader(meshCom->fileToEdit.c_str());
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, meshCom->editor.GetTotalLines(),
			meshCom->editor.IsOverwrite() ? "Ovr" : "Ins",
			meshCom->editor.CanUndo() ? "*" : " ",
			meshCom->editor.GetLanguageDefinition().mName.c_str(), meshCom->fileToEdit.c_str());

		meshCom->editor.Render("TextEditor");
		ImGui::End();
	}

	//ShowUniforms();
	
	if (!meshCom->GetShader()->parameters.uniforms.empty())
	{
		
	}
		
	ImGui::PopID();
}

void MaterialComponent::ShowUniforms()
{
	Shader* shader = owner->GetComponent<MeshComponent>()->GetShader();
	if (shader->parameters.name != "DefaultShader")
	{
		for (uint i = 0; i < shader->parameters.uniforms.size(); i++)
		{
			switch (shader->parameters.uniforms[i].uniformType)
			{
			case  UniformType::INT:	ImGui::DragInt(shader->parameters.uniforms[i].name.c_str(), &shader->parameters.uniforms[i].integer, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::FLOAT: ImGui::DragFloat(shader->parameters.uniforms[i].name.c_str(), &shader->parameters.uniforms[i].floatNumber, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::INT_VEC2: ImGui::DragInt2(shader->parameters.uniforms[i].name.c_str(), (int*)&shader->parameters.uniforms[i].vec2, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::INT_VEC3: ImGui::DragInt3(shader->parameters.uniforms[i].name.c_str(), (int*)&shader->parameters.uniforms[i].vec3, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::INT_VEC4: ImGui::DragInt4(shader->parameters.uniforms[i].name.c_str(), (int*)&shader->parameters.uniforms[i].vec4, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::FLOAT_VEC2: ImGui::DragFloat2(shader->parameters.uniforms[i].name.c_str(), (float*)&shader->parameters.uniforms[i].vec2, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::FLOAT_VEC3: ImGui::DragFloat3(shader->parameters.uniforms[i].name.c_str(), (float*)&shader->parameters.uniforms[i].vec3, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case  UniformType::FLOAT_VEC4: ImGui::DragFloat4(shader->parameters.uniforms[i].name.c_str(), (float*)&shader->parameters.uniforms[i].vec4, 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			case UniformType::MATRIX4: ImGui::DragFloat4(shader->parameters.uniforms[i].name.c_str(), shader->parameters.uniforms[i].matrix4.ToEulerXYZ().ptr(), 0.02f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None); break;
			}
		}
	}
	
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


