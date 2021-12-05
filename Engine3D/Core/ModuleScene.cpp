#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "glew.h"
#include "ImGui/imgui.h"
#include "ModuleImport.h"
#include "ModuleTextures.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "Component.h"
#include "File.h"
#include <stack>
#include <queue>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

bool ModuleScene::Init()
{
	root = new GameObject("Root");
	//Loading house and textures since beginning

	cameraGame = CreateGameObject("Camera", root);
	cameraGame->CreateComponent<ComponentCamera>();

	return true;
}

bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	
	App->import->LoadGeometry("Assets/Models/street.fbx");

	cameraGame->GetComponent<ComponentCamera>()->Start();

	cameraGame->GetComponent<ComponentTransform>()->SetPosition(float3(0,3,-14));
	cameraGame->GetComponent<ComponentCamera>()->LookAt(float3(0,0,0));

	assetFile = new File("Assets");
	assetFile->path = assetFile->name;
	assetFile->Read();
	
	return true;
}

bool ModuleScene::CleanUp()
{
	std::stack<GameObject*> S;
	for (GameObject* child : root->children)	
	{
		S.push(child);
	}
	root->children.clear();

	while (!S.empty())
	{
		GameObject* go = S.top();
		S.pop();
		for (GameObject* child : go->children)
		{
			S.push(child);
		}
		go->children.clear();
		delete go;
	}

	delete root;

	return true;
}

update_status ModuleScene::Update(float dt)
{
	std::queue<GameObject*> S;
	for (GameObject* child : root->children)
	{
		S.push(child);
	}

	while (!S.empty())
	{
		GameObject* go = S.front();
		go->Update(dt);
		S.pop();
		for (GameObject* child : go->children)
		{
			S.push(child);
		}
	}
	App->editor->DrawGrid();
	App->editor->viewPortScene.PostUpdate(dt);

	if (cameraGame != nullptr)
	{
		cameraGame->GetComponent<ComponentCamera>()->StartBuffer(dt);
		std::queue<GameObject*> S;
		for (GameObject* child : root->children)
		{
			S.push(child);
		}

		while (!S.empty())
		{
			GameObject* go = S.front();
			go->Update(dt);
			S.pop();
			for (GameObject* child : go->children)
			{
				S.push(child);
			}
		}
		cameraGame->GetComponent<ComponentCamera>()->PostUpdate(dt);
	}
	else
	{
		LOG("The camera doesn't exist");
	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent) {

	GameObject* temp = new GameObject();
	if (parent)
		parent->AttachChild(temp);
	else
		root->AttachChild(temp);
	return temp;
}
GameObject* ModuleScene::CreateGameObject(const std::string name, GameObject* parent)
{
	GameObject* temp = new GameObject(name);
	if (parent)
		parent->AttachChild(temp);
	else
		root->AttachChild(temp);
	return temp;
}
