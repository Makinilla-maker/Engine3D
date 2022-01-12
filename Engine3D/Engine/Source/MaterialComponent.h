#pragma once

#include "Component.h"
#include "Shader.h"
#include "Color.h"
#include <string>
#include <memory>
#include <fstream>

#include "Imgui/TextEditor/TextEditor.h"

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

	void ShowUniforms();

	void EditorShader(MaterialComponent* material);

	bool OnLoad(JsonParsing& node) override;
	bool OnSave(JsonParsing& node, JSON_Array* array) override;

	void BindTexture();
	void UnbindTexture();

	void SetTexture(std::shared_ptr<Resource> tex);
	//inline void SetColor(Color color) { this->color = color; }
	//inline Color GetColor() const { return color; }
	inline Shader* const GetShader() { return shader; }
	uint32 const GetShaderID() { return shader->parameters.shaderID; }
	std::shared_ptr<Texture> getTexture() { return diff; }

	void LoadShader(std::string path);
private:
	bool checker;
	bool showTexMenu;
	bool showShaderMenu;
	bool showShaderEditor;

	std::shared_ptr<Texture> diff;
	Shader* shader;

	Shader* shadertoRecompily;

	TextEditor editor;
	std::string fileToEdit;
};