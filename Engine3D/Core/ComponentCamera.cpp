#include "Application.h"
#include "ModuleTextures.h"
#include "ImGui/imgui.h"
#include "ComponentCamera.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent)
{
	frustrum.type = FrustumType::PerspectiveFrustum;
	frustrum.nearPlaneDistance = 0.2f;
	frustrum.farPlaneDistance = 500.f;
	//Do getForward del gameobject
	frustrum.front = float3::unitZ;
	frustrum.up = float3::unitY;
	frustrum.verticalFov = 60.0f * DEGTORAD;
	frustrum.horizontalFov = 2.0f * atanf(tanf(frustrum.verticalFov / 2) * (16.f / 9.f));

}

bool ComponentCamera::Update(float dt)
{
	/*
	//Maybe dont update every frame?
	frustrum.pos = owner->transform->GetPosition();
	frustrum.front = owner->transform->GetForward();
	frustrum.up = owner->transform->GetUp();


	//glVertex3f(gameObject->transform->position.x, gameObject->transform->position.y, gameObject->transform->position.z);
	//glVertex3f(gameObject->transform->position.x + forward.x, gameObject->transform->position.y + forward.y, gameObject->transform->position.z + forward.z);
	float3 points[8];
	frustrum.GetCornerPoints(points);

	ModuleRenderer3D::DrawBox(points, float3(0.180f, 1.f, 0.937f));
	*/
	return true;

}
