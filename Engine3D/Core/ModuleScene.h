#pragma once

// -- Tools
#include "Module.h"
#include "Globals.h"
#include "ModuleImport.h"
#include "Geometry/Frustum.h"
#include "GameObject.h"

class File;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);

	bool Init() override;
	bool Start() override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	GameObject* CreateGameObject(GameObject* parent = nullptr);	
	GameObject* CreateGameObject(const std::string name, GameObject* parent = nullptr);	
	
public:
	
	GameObject* root;
	GameObject* cameraGame;
	File* assetFile;
	
};
