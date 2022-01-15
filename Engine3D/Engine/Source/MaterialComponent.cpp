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

MaterialComponent::MaterialComponent(GameObject* own) : diff(nullptr), showTexMenu(false), shader(nullptr)
{
	type = ComponentType::MATERIAL;
	owner = own;
	checker = false;
	shader = new Shader();
	showShaderMenu = false;
	showShaderEditor = false;

	
	active = true;
}

MaterialComponent::MaterialComponent(MaterialComponent* mat) : showTexMenu(false)
{
	checker = mat->checker;
	diff = mat->diff;
	showShaderMenu = false;
	showShaderEditor = false;

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
			ImGui::Text("Selected Shader: ");
			ImGui::SameLine();
			if (ImGui::Button(shader ? shader->parameters.name.c_str() : ""))
			{
				showShaderMenu = true;
			}
			ImGui::Text("Path: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", shader->GetPath().c_str());
			if (ImGui::Button("Edit Shader", { 100,25 }))	EditorShader(owner->GetComponent<MeshComponent>()->GetMaterial());
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
		ImGui::Begin("Textures", &showShaderMenu);
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
			std::string a = GetNamefromPath((*it).second);
			if (ImGui::Selectable(a.c_str()))//, { 200,25 }))
			{
				owner->GetComponent<MeshComponent>()->GetMaterial()->LoadShader((*it).second.c_str());
			}
		}
		ImGui::End();
	}
	if (showShaderEditor)
	{
		ImGui::Begin("Shader Editor", &showShaderEditor, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);
		//Update
		auto cpos = editor.GetCursorPosition();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Button("Save"))
			{
				std::string shader = editor.GetText();

				app->fs->RemoveFile(fileToEdit.c_str());
				app->fs->Save(fileToEdit.c_str(), shader.c_str(), editor.GetText().size());

				glDetachShader(shadertoRecompily->parameters.shaderID, shadertoRecompily->parameters.vertexID);
				glDetachShader(shadertoRecompily->parameters.shaderID, shadertoRecompily->parameters.fragmentID);
				glDeleteProgram(shadertoRecompily->parameters.shaderID);

				owner->GetComponent<MeshComponent>()->GetMaterial()->LoadShader(fileToEdit.c_str());
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
			editor.IsOverwrite() ? "Ovr" : "Ins",
			editor.CanUndo() ? "*" : " ",
			editor.GetLanguageDefinition().mName.c_str(), fileToEdit.c_str());

		editor.Render("TextEditor");
		ImGui::End();
	}

	ShowUniforms();
	
	if (!owner->GetComponent<MeshComponent>()->GetMaterial()->GetShader()->parameters.uniforms.empty())
	{
		
	}
		
	ImGui::PopID();
}

void MaterialComponent::ShowUniforms()
{
	Shader* shader = owner->GetComponent<MeshComponent>()->GetMaterial()->GetShader();
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

void MaterialComponent::EditorShader(MaterialComponent* material)
{
	TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();

	fileToEdit = material->GetShader()->parameters.path;
	editor.SetShowWhitespaces(false);

	std::ifstream text(fileToEdit.c_str());
	if (text.good())
	{
		std::string str((std::istreambuf_iterator<char>(text)), std::istreambuf_iterator<char>());
		editor.SetText(str);
	}

	showShaderEditor = true;

	shadertoRecompily = material->GetShader();
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

std::string MaterialComponent::GetNamefromPath(std::string path)
{
	size_t separator = path.find_last_of("\\/");
	size_t dot = path.find_last_of(".");

	if (separator < path.length())
		return path.substr(separator + 1, dot - separator - 1);
	else
		return path.substr(0, dot);
}

void MaterialComponent::LoadShader(std::string path)
{
	char* buffer;
	int size = app->fs->Load(path.c_str(), &buffer);

	shader->parameters.name =GetNamefromPath(path);
	shader->parameters.path = path;

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
