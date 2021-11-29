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
	//Do getForward del gameobject
	frustrum.front = owner->transform->GetForward();
	frustrum.up = owner->transform->GetUp();
	frustrum.verticalFov = 60.0f * DEGTORAD;
	frustrum.horizontalFov = 2.0f * atanf(tanf(frustrum.verticalFov / 2) * (16.f / 9.f));

}

ComponentCamera::~ComponentCamera()
{
}

bool ComponentCamera::Update(float dt)
{	
	//Maybe dont update every frame?
	frustrum.pos = owner->transform->GetPosition();
	frustrum.front = owner->transform->GetForward();
	frustrum.up = owner->transform->GetUp();

	//Render de lineas de camera mas adelante


	return true;

}

bool ComponentCamera::PreUpdate(float dt)
{
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(frustrum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix.Transposed().ptr());
	return false;
}
