#pragma once
#include "Globals.h"

#include <string>

class ModuleViewportFrameBuffer
{
public:
	ModuleViewportFrameBuffer();
	~ModuleViewportFrameBuffer();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:

	uint frameBuffer = 0;
	uint renderBufferoutput = 0;
	uint texture = 0;
	bool show_viewport_window = true;

};