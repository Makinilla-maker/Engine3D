#include "Globals.h"

#include "Application.h"
#include "ModuleImport.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "GameObject.h"

#include <vector>
#include <queue>
#include "SDL/include/SDL_opengl.h"
#include "Math/float2.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

#include <fstream>


ModuleImport::ModuleImport(Application* app, bool start_enabled) : Module(app, start_enabled) {}


// Called before render is available
bool ModuleImport::Init()
{
	//LOG("Creating 3D Renderer context");
	bool ret = true;

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

update_status ModuleImport::Update(float dt) {

	return UPDATE_CONTINUE;
}

bool ModuleImport::LoadGeometry(const char* path) {

	//-- Own structure	
	GameObject* root = nullptr;
	std::string new_root_name(path);

	//-- Assimp stuff
	aiMesh* assimpMesh = nullptr;
	const aiScene* scene = nullptr;
	aiMaterial* texture = nullptr;
	aiString texturePath;

	//Create path buffer and import to scene
	char* buffer = nullptr;
	uint bytesFile = App->fileSystem->Load(path, &buffer);

	if (buffer == nullptr) {
		std::string normPathShort = "Assets/Models/" + App->fileSystem->SetNormalName(path);
		bytesFile = App->fileSystem->Load(normPathShort.c_str(), &buffer);
	}
	if (buffer != nullptr) {
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	}
	else {
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	}

	
	if (scene != nullptr && scene->HasMeshes()) {
		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			bool nameFound = false;
			std::string name;
			aiMatrix4x4 transform;
			FindNodeName(scene, i, name, transform);
			NodesS(scene->mMeshes[i], scene->mRootNode, transform, scene);

			GameObject* newGameObject = App->scene->CreateGameObject(name);
			ComponentMesh* mesh = newGameObject->CreateComponent<ComponentMesh>();
			assimpMesh = scene->mMeshes[i];

			if (scene->HasMaterials()) {
				//texture = scene->mMaterials[assimpMesh->mMaterialIndex];
				aiMaterial* material = scene->mMaterials[assimpMesh->mMaterialIndex];

				//if (texture != nullptr) {
				if (material != nullptr) {
					//aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, assimpMesh->mMaterialIndex, &texturePath);
					material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
					std::string new_path(texturePath.C_Str());
					new_path = App->fileSystem->NormalizePath(texturePath.C_Str());
					if (new_path.size() > 0) {
						mesh->texturePath = "Assets/Textures/" + App->fileSystem->SetNormalName(new_path.c_str());
						if (!App->textures->Find(mesh->texturePath))
						{
							const TextureObject& textureObject = App->textures->Load(mesh->texturePath);
							ComponentMaterial* materialComp = newGameObject->CreateComponent<ComponentMaterial>();
							materialComp->SetTexture(textureObject);

						}
						else
						{
							const TextureObject& textureObject = App->textures->Get(mesh->texturePath);
							ComponentMaterial* materialComp = newGameObject->CreateComponent<ComponentMaterial>();
							materialComp->SetTexture(textureObject);
						}
					}
				}
			}

			mesh->numVertices = assimpMesh->mNumVertices;
			mesh->vertices.resize(assimpMesh->mNumVertices);

			memcpy(&mesh->vertices[0], assimpMesh->mVertices, sizeof(float3) * assimpMesh->mNumVertices);
			LOG("New mesh with %d vertices", assimpMesh->mNumVertices);

			// -- Copying faces --//
			if (assimpMesh->HasFaces()) {
				mesh->numIndices = assimpMesh->mNumFaces * 3;
				mesh->indices.resize(mesh->numIndices);

				for (size_t i = 0; i < assimpMesh->mNumFaces; i++)
				{
					if (assimpMesh->mFaces[i].mNumIndices != 3) {
						LOG("WARNING, geometry face with != 3 indices!")
					}
					else {
						memcpy(&mesh->indices[i * 3], assimpMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}

			// -- Copying Normals info --//
			if (assimpMesh->HasNormals()) {

				mesh->normals.resize(assimpMesh->mNumVertices);
				memcpy(&mesh->normals[0], assimpMesh->mNormals, sizeof(float3) * assimpMesh->mNumVertices);
			}

			// -- Copying UV info --//
			if (assimpMesh->HasTextureCoords(0))
			{
				mesh->texCoords.resize(assimpMesh->mNumVertices);
				for (size_t j = 0; j < assimpMesh->mNumVertices; ++j)
				{
					memcpy(&mesh->texCoords[j], &assimpMesh->mTextureCoords[0][j], sizeof(float2));
				}
			}

			mesh->GenerateBuffers();
			mesh->GenerateBounds();
			mesh->ComputeNormals();

			mesh->GenerateGlobalBounds(newGameObject->parent->transform->transformMatrixLocal);
			
			TransformButWellMade(transform, newGameObject);
			
			std::string newName(path);
			newName = newName.substr(newName.find_last_of("/") + 1);
			newName = newName.substr(0, newName.find_first_of("."));
			newName += ".wtf";
			//Save(mesh, newName.c_str());

		}
		aiReleaseImport(scene);
		RELEASE_ARRAY(buffer);

	}
	else
		LOG("Error loading scene %s", path);

	RELEASE_ARRAY(buffer);

	return true;
}
void ModuleImport::TransformButWellMade(aiMatrix4x4 transform, GameObject* gm)
{
	aiVector3D position, scale;
	aiQuaternion rotation;

	transform.Decompose(scale, rotation, position);

	gm->transform->SetPosition(float3(position.x, position.y, position.z));
	gm->transform->SetRotation(Quat(rotation.x, rotation.y, rotation.z, rotation.w).ToEulerXYZ());//if we multiply that QUat with RADTODEG, doesns't work
	gm->transform->SetScale(float3(scale.x, scale.y, scale.z));
}

void ModuleImport::FindNodeName(const aiScene* scene, const size_t i, std::string& name, aiMatrix4x4& transform)
{
	bool nameFound = false;
	std::queue<aiNode*> Q;
	Q.push(scene->mRootNode);
	while (!Q.empty() && !nameFound)
	{
		aiNode* node = Q.front();
		Q.pop();
		for (size_t j = 0; j < node->mNumMeshes; ++j)
		{
			if (node->mMeshes[j] == i)
			{
				nameFound = true;
				name = node->mName.C_Str();
			}
		}
		transform = node->mTransformation;
		if (!nameFound)
		{
			for (size_t j = 0; j < node->mNumChildren; ++j)
			{
				Q.push(node->mChildren[j]);
			}
		}
	}
}
void ModuleImport::NodesS(const aiMesh* meshScene, aiNode* node, aiMatrix4x4& transform, const aiScene* scene)
{
	for (int i = 0; i < node->mNumChildren; i++)
	{
		if (node->mChildren[i]->mNumMeshes > 0)
		{
			for (int j = 0; j < node->mChildren[i]->mNumMeshes; j++)
			{
				if (scene->mMeshes[node->mChildren[i]->mMeshes[j]] == meshScene)
				{
					transform = node->mChildren[i]->mTransformation;
				}
			}
		}
		if (node->mChildren[i]->mNumChildren > 0)
		{
			NodesS(meshScene, node->mChildren[i], transform, scene);
		}
	}
}

uint64 ModuleImport::Save(const ComponentMesh* mesh, const char* name)
{
	uint ranges[2] =
	{
		mesh->numIndices,
		mesh->numVertices
	};
	uint size =
		sizeof(ranges)
		+ sizeof(uint) * mesh->numIndices
		+ sizeof(float) * mesh->numVertices * 3;

	char* buffer = new char[size];
	char* cursor = buffer;

	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	bytes = sizeof(uint) * mesh->numIndices;
	memcpy(cursor, &mesh->indices, bytes);
	cursor += bytes;

	bytes = sizeof(float3) * mesh->numVertices;
	memcpy(cursor, &mesh->vertices, bytes);
	cursor += bytes;

	//texturePath
	//numNormalFaces
	//call GenerateBuffers?
	//call ComputeNormals? etc
	//AABB?

	std::ofstream outfile(name, std::ofstream::binary);//bin?
	outfile.write(buffer, size);
	outfile.close();
	delete[] buffer;

	return uint64();//?
}

void ModuleImport::Load(const char* name)
{
	std::ifstream infile(name, std::ifstream::binary);//bin?
	infile.seekg(0, infile.end);
	long size = infile.tellg();
	infile.seekg(0);
	char* buffer = new char[size];
	infile.read(buffer, size);
	infile.close();

	GameObject* newGameObject = App->scene->CreateGameObject(name);
	ComponentMesh* mesh = newGameObject->CreateComponent<ComponentMesh>();

	uint ranges[2];
	char* cursor = buffer;

	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	mesh->numIndices = ranges[0];
	bytes = sizeof(uint) * mesh->numIndices;
	mesh->indices.resize(mesh->numIndices);
	memcpy(&mesh->indices, cursor, bytes);
	cursor += bytes;

	mesh->numVertices = ranges[1];
	bytes = sizeof(float3) * mesh->numVertices;
	mesh->vertices.resize(mesh->numVertices);
	memcpy(&mesh->vertices, cursor, bytes);
	cursor += bytes;

	mesh->GenerateBuffers();
	mesh->GenerateBounds();
	mesh->ComputeNormals();
}

// Called before quitting
bool ModuleImport::CleanUp()
{
	//-- Detach log stream
	aiDetachAllLogStreams();

	return true;
}