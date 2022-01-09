#pragma once

#include "Component.h"
#include "Shader.h"
#include "Color.h"
#include <string>
#include <memory>

typedef unsigned int GLuint;
typedef unsigned char GLubyte;

class Texture;
class Resource;

class MaterialComponent : public Component
{
public:
	MaterialComponent(GameObject* own);
	MaterialComponent(MaterialComponent* mat);
	~MaterialComponent();

	void OnEditor() override;

	bool OnLoad(JsonParsing& node) override;
	bool OnSave(JsonParsing& node, JSON_Array* array) override;

	void BindTexture();
	void UnbindTexture();

	void SetTexture(std::shared_ptr<Resource> tex);
	inline Color GetColor() const { return color; }
	inline Shader* const GetShader() { return shader; }
	uint32 const GetShaderID() { return shader->parameters.shaderID; }
	//inline void SetShader(std::shared_ptr<Resource> _shader) { this->shader = _shader; }

	void LoadShader(std::string path);
private:
	bool checker;
	bool showTexMenu;
	bool showShaderMenu;

	std::shared_ptr<Texture> diff;
	Shader* shader;
	Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
};