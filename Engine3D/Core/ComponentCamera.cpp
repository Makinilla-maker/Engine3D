#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "SDL/include/SDL_opengl.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent)
{
	frustrum.type = FrustumType::PerspectiveFrustum;
	frustrum.nearPlaneDistance = 0.2f;
	frustrum.farPlaneDistance = 50000.f;
	frustrum.front = owner->transform->GetForward();
	frustrum.up = owner->transform->GetUp();
	frustrum.verticalFov = 60.0f * DEGTORAD;
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

	// 
	float3 points[8];
	frustrum.GetCornerPoints(points);
	App->renderer3D->DrawBox(points, float3(1.f, 1.f, 0.f));


	return true;
}

bool ComponentCamera::StartBuffer(float dt)
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
	if(ImGui::CollapsingHeader("Camera"))
	{
		ImGui::DragFloat("Near Distance", &frustrum.nearPlaneDistance,0.2,0.1,500);
		ImGui::DragFloat("Far Distance", &frustrum.farPlaneDistance,0.2,0.1);
		if(ImGui::DragFloat("Vertical FOV", &FOV, 0.2, 0.1)) 
		{
			RecalculateProjection();
		}
	}
}

void ComponentCamera::RecalculateProjection()
{
	frustrum.type = FrustumType::PerspectiveFrustum;
	frustrum.verticalFov = (FOV * 3.141592 / 2) / 180.f;
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

