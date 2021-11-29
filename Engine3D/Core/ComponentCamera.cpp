#include "Application.h"
#include "ModuleTextures.h"
#include "ImGui/imgui.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "SDL/include/SDL_opengl.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent)
{
	frustrum.type = FrustumType::PerspectiveFrustum;
	frustrum.nearPlaneDistance = 0.2f;
	frustrum.farPlaneDistance = 500.f;
	frustrum.front = owner->transform->GetForward();
	frustrum.up = owner->transform->GetUp();
	frustrum.verticalFov = 60.0f * DEGTORAD;
	frustrum.horizontalFov = 2.0f * atanf(tanf(frustrum.verticalFov / 2) * (16.f / 9.f));
}

ComponentCamera::~ComponentCamera()
{
	viewPortGame.CleanUp();
}

bool ComponentCamera::Start()
{
	viewPortGame.Start();

	return true;
}

bool ComponentCamera::Update(float dt)
{	
	if (movedCamera)
	{
		frustrum.pos = owner->transform->GetPosition();
		frustrum.front = owner->transform->GetForward();
		frustrum.up = owner->transform->GetUp();
		viewMatrix = frustrum.ViewMatrix();
		movedCamera = false;
	}

	//Render de lineas de camera mas adelante

	return true;
}

bool ComponentCamera::PreUpdate(float dt)
{
	viewPortGame.PreUpdate(dt);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(frustrum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix.Transposed().ptr());

	return true;
}

bool ComponentCamera::PostUpdate(float dt)
{
	viewPortGame.PostUpdate(dt);
	return true;
}

void ComponentCamera::OnGui()
{
	ImGui::CollapsingHeader("Camera");
}

void ComponentCamera::RecalculateProjection()
{
	frustrum.type = FrustumType::PerspectiveFrustum;
	frustrum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
	frustrum.horizontalFov = 2.f * atanf(tanf(frustrum.verticalFov * 0.5f) * aspectRatio);
}
void ComponentCamera::LookAt(const float3& point)
{
	float3 right;
	frustrum.front = (point - frustrum.pos).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(frustrum.front).Normalized();
	frustrum.up = frustrum.front.Cross(right);

	movedCamera = true;
}

