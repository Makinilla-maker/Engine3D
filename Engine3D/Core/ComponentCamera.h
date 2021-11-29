#pragma once

#include "Component.h"
#include "Geometry/Frustum.h"

struct TextureObject;

class ComponentCamera : public Component {

public:

	ComponentCamera(GameObject* parent);

	

private:

	Frustum frustrum;
};