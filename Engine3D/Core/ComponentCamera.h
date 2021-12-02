#pragma once

#include "Component.h"
#include "Geometry/Frustum.h"
#include "Math/float4x4.h"
#include "ModuleViewportFrameBuffer.h"

struct TextureObject;

class ComponentCamera : public Component {

public:

	ComponentCamera(GameObject* parent);

	virtual ~ComponentCamera();

	bool Start();
	
	bool PreUpdate(float dt);
	bool Update(float dt) override;
	bool PostUpdate(float dt);
	void RecalculateProjection();
	void LookAt(const float3& point);
	Frustum GetCamera()
	{
		return frustrum;
	}
	void SetAspectRadio(float asprat)
	{
		aspectRatio = asprat;
	}

	void OnGui() override;

	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;	
	ModuleViewportFrameBuffer viewPortGame;
	bool movedCamera;

private:

	Frustum frustrum;
	float4x4 viewMatrix;
	float FOV = 60.f;
	float aspectRatio = 1.f;
};