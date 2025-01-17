#pragma once

#include "Component.h"
#include "MathGeoLib/src/Geometry/Frustum.h"

class TransformComponent;
class IndexBuffer;
class VertexBuffer;
class CameraComponent : public Component
{
public:
	CameraComponent(GameObject* own, TransformComponent* trans);
	~CameraComponent();

	void OnEditor() override;
	bool Update(float dt) override;
	void Draw() override;

	void SetPlanes();
	void CalculateVerticalFov(float horizontalFovRadians, float width, float height);
	void UpdateFovAndScreen(float width, float height);
	void UpdateFov();
	
	void CompileBuffers();
	bool CompareRotations(Quat& quat1, Quat& quat2);

	bool OnLoad(JsonParsing& node) override;
	bool OnSave(JsonParsing& node, JSON_Array* array) override;

	int CameraComponent::ContainsAaBox(const AABB& boundingBox);

	float4x4 matrixViewFrustum;
	float4x4 matrixProjectionFrustum;

	vec GetCameraPos() 
	{
		return camera.Pos();
	}

	float* GetRawViewMatrix()
	{
		static float4x4 m;

		m = camera.ViewMatrix();
		m.Transpose();

		return (float*)m.v;
	}

	float* GetProjectionMatrix()
	{
		static float4x4 m;

		m = camera.ProjectionMatrix();
		m.Transpose();

		return (float*)m.v;
	}

private:

	Frustum camera;
	TransformComponent* transform;
	Quat currentRotation;
	float3 currentPos;

	float nearPlane;
	float farPlane;
	float verticalFov;
	float horizontalFov;
	float currentScreenWidth;
	float currentScreenHeight;

	IndexBuffer* ebo;
	VertexBuffer* vbo;
};