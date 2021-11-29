#pragma once

#include "Component.h"
#include "Geometry/Frustum.h"
#include "Math/float4x4.h"

struct TextureObject;

class ComponentCamera : public Component {

public:

	ComponentCamera(GameObject* parent);

	virtual ~ComponentCamera();

	bool Update(float dt) override;
	bool PreUpdate(float dt);

	float4x4 ViewMatrixOpenGL() const;
	float4x4 ProjectionMatrixOpenGL() const;

	void SetAspectRatio(float aspectRatio);

	void StartDraw();
	void EndDraw();

	void ReGenerateBuffer(int w, int h);

	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;	

private:

	Frustum frustrum;
	float4x4 viewMatrix;
};