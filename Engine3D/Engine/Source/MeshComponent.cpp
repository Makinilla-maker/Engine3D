#include "Application.h"
#include "Globals.h"
#include "MeshComponent.h"

#include "ModuleScene.h"
#include "CameraComponent.h"
#include "MaterialComponent.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "FileSystem.h"
#include "ResourceManager.h"
#include "ShaderImporter.h"
#include "ModuleScene.h"

#include "Mesh.h"

#include "Imgui/imgui.h"

#include "glew/include/GL/glew.h"

#include "Profiling.h"

MeshComponent::MeshComponent(GameObject* own, TransformComponent* trans) : material(nullptr), transform(trans), faceNormals(false), verticesNormals(false), normalLength(1.0f), colorNormal(150.0f, 0.0f, 255.0f), shader(nullptr)
{
	type = ComponentType::MESH_RENDERER;
	owner = own;
	mesh = nullptr;
	shader = new Shader();
	material = owner->GetComponent<MaterialComponent>();
	showShaderEditor = false;
	

	showMeshMenu = false;
}

MeshComponent::MeshComponent(MeshComponent* meshComponent, TransformComponent* trans) : material(nullptr), showMeshMenu(false)
{
	transform = trans;
	mesh = meshComponent->GetMesh();

	faceNormals = meshComponent->faceNormals;
	verticesNormals = meshComponent->verticesNormals;
	normalLength = meshComponent->normalLength;
	colorNormal = meshComponent->colorNormal;
	shader = new Shader();
	showShaderEditor = false;

	localBoundingBox = meshComponent->localBoundingBox;
}

MeshComponent::~MeshComponent()
{
	if (mesh.use_count() - 1 == 1) mesh->UnLoad();
}
uint32 MeshComponent::SetDefaultShader()
{
	LoadShader("Assets\/Shaders\/DefaultShader.shader");
	return GetShaderID();
}
void MeshComponent::Draw()
{
	uint32 shaderProgram = 0;
	if (GetShader())	shaderProgram = GetShaderID();
	shaderProgram ? shaderProgram : shaderProgram = SetDefaultShader();
	
	

	glUseProgram(shaderProgram);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	glMultMatrixf(transform->GetGlobalTransform().Transposed().ptr());
	
	if (material != nullptr && material->GetActive()) material->BindTexture();

	if (shaderProgram != 0)
	{
		GetShader()->SetUniform1i("hasTexture", (GLint)true);

		GetShader()->SetUniformMatrix4("modelMatrix", transform->GetLocalTransform().Transposed().ptr());

		GetShader()->SetUniformMatrix4("viewMatrix", app->scene->mainCamera->GetRawViewMatrix());

		GetShader()->SetUniformMatrix4("projectionMatrix", app->scene->mainCamera->GetProjectionMatrix());

		GetShader()->SetUniform1f("time", a);

		GetShader()->SetUniformVec3f("cameraPosition", (GLfloat*)&app->camera->cameraFrustum.Pos());

		ShaderImporter::SetShaderUniforms(GetShader());
	}
	if (GetShader())	GetShader()->SetUniform1f("time", a);
	

	if (mesh != nullptr) mesh->Draw(verticesNormals, faceNormals, colorNormal, normalLength);

	if (material != nullptr && material->GetActive()) material->UnbindTexture();
	
	glUseProgram(0);
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	a+=0.05;
}

void MeshComponent::DrawScene()
{
	uint32 shaderProgram = 0;
	if (GetShader())	shaderProgram = GetShaderID();
	shaderProgram ? shaderProgram : shaderProgram = SetDefaultShader();
	


	glUseProgram(shaderProgram);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	glMultMatrixf(transform->GetGlobalTransform().Transposed().ptr());

	if (material != nullptr && material->GetActive()) material->BindTexture();

	if (shaderProgram != 0)
	{
		GetShader()->SetUniform1i("hasTexture", (GLint)true);

		GetShader()->SetUniformMatrix4("modelMatrix", transform->GetLocalTransform().Transposed().ptr());

		GetShader()->SetUniformMatrix4("viewMatrix", app->camera->GetRawViewMatrix());

		GetShader()->SetUniformMatrix4("projectionMatrix", app->camera->GetProjectionMatrix());

		GetShader()->SetUniform1f("time", a);

		GetShader()->SetUniformVec3f("cameraPosition", (GLfloat*)&app->camera->cameraFrustum.Pos());

		ShaderImporter::SetShaderUniforms(GetShader());
	}
	if (GetShader())	GetShader()->SetUniform1f("time", a);
	

	if (mesh != nullptr) mesh->Draw(verticesNormals, faceNormals, colorNormal, normalLength);

	if (material != nullptr && material->GetActive()) material->UnbindTexture();

	glUseProgram(0);
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	a += 0.05;
}

void MeshComponent::DrawOutline()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	float4x4 testGlobal = transform->GetGlobalTransform();

	testGlobal.scaleX += 0.05f;
	testGlobal.scaleY += 0.05f;
	testGlobal.scaleZ += 0.05f;
	glMultMatrixf(testGlobal.Transposed().ptr());

	if (mesh != nullptr) mesh->Draw(verticesNormals, faceNormals, colorNormal, normalLength);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MeshComponent::OnEditor()
{
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader("Mesh Renderer"))
	{
		Checkbox(this, "Active", active);
		ImGui::Text("Select mesh");
		ImGui::SameLine();
		if (ImGui::Button(mesh ? mesh->GetName().c_str() : "No Mesh"))
		{
			showMeshMenu = true;
		}
		ImGui::Text("Number of vertices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", mesh ? mesh->GetVerticesSize() : 0);
		ImGui::Text("Number of indices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", mesh ? mesh->GetIndicesSize() : 0);
		ImGui::Checkbox("Vertices normals", &verticesNormals);
		ImGui::Checkbox("Face normals", &faceNormals);
		ImGui::DragFloat("Normal Length", &normalLength, 0.200f);
		ImGui::DragFloat3("Normal Color", colorNormal.ptr(), 1.0f, 0.0f, 255.0f);
		ImGui::Text("Reference Count: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", mesh ? mesh.use_count() : 0);
		ImGui::Separator();
	}

	if (showMeshMenu)
	{
		ImGui::Begin("Meshes", &showMeshMenu, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 mouse = ImGui::GetIO().MousePos;
		if (!(mouse.x < winPos.x + size.x && mouse.x > winPos.x && 
			mouse.y < winPos.y + size.y && mouse.y > winPos.y))
		{
			if (ImGui::GetIO().MouseClicked[0]) showMeshMenu = false;
		}

		std::vector<std::string> files;
		app->fs->DiscoverFiles("Library/Meshes/", files);
		for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
		{
			if ((*it).find(".rgmesh") != std::string::npos)
			{
				app->fs->GetFilenameWithoutExtension(*it);
				*it = (*it).substr((*it).find_last_of("_") + 1, (*it).length());
				uint uid = std::stoll(*it);
				std::shared_ptr<Resource> res = ResourceManager::GetInstance()->LoadResource(uid);
				if (ImGui::Selectable(res->GetName().c_str()))
				{
					if (mesh.use_count() - 1 == 1) mesh->UnLoad();
					SetMesh(res);
				}
			}
		}

		ImGui::End();
	}

	ImGui::PopID();
}

bool MeshComponent::OnLoad(JsonParsing& node)
{
	mesh = std::static_pointer_cast<Mesh>(ResourceManager::GetInstance()->LoadResource(std::string(node.GetJsonString("Path"))));

	active = node.GetJsonBool("Active");

	if (mesh)
	{
		localBoundingBox.SetNegativeInfinity();
		localBoundingBox.Enclose(mesh->GetVerticesData(), mesh->GetVerticesSize());

		owner->SetAABB(localBoundingBox);
	}

	return true;
}

bool MeshComponent::OnSave(JsonParsing& node, JSON_Array* array)
{
	JsonParsing file = JsonParsing();

	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "Type", (int)type);
	file.SetNewJsonString(file.ValueToObject(file.GetRootValue()), "Path", mesh->GetAssetsPath().c_str());
	file.SetNewJsonBool(file.ValueToObject(file.GetRootValue()), "Active", active);

	node.SetValueToArray(array, file.GetRootValue());

	return true;
}

void MeshComponent::SetMesh(std::shared_ptr<Resource> m)
{
	mesh = std::static_pointer_cast<Mesh>(m);

	if (mesh)
	{
		localBoundingBox.SetNegativeInfinity();
		localBoundingBox.Enclose(mesh->GetVerticesData(), mesh->GetVerticesSize());

		owner->SetAABB(localBoundingBox);
	}
}

std::string MeshComponent::GetNamefromPath(std::string path)
{
	size_t separator = path.find_last_of("\\/");
	size_t dot = path.find_last_of(".");

	if (separator < path.length())
		return path.substr(separator + 1, dot - separator - 1);
	else
		return path.substr(0, dot);
}

void MeshComponent::EditorShader()
{
	TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();

	fileToEdit = GetShader()->parameters.path;
	editor.SetShowWhitespaces(false);

	std::ifstream text(fileToEdit.c_str());
	if (text.good())
	{
		std::string str((std::istreambuf_iterator<char>(text)), std::istreambuf_iterator<char>());
		editor.SetText(str);
	}

	showShaderEditor = true;

	shadertoRecompily = GetShader();
}
void MeshComponent::LoadShader(std::string path)
{
	char* buffer;
	int size = app->fs->Load(path.c_str(), &buffer);

	shader->parameters.name = GetNamefromPath(path);
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