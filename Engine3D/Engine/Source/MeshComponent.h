#pragma once

#include "Component.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "MathGeoLib/src/MathGeoLib.h"
#include "MaterialComponent.h"

#include <vector>

typedef unsigned int GLuint;
typedef unsigned char GLubyte;

class TransformComponent;
class MaterialComponent;
class Mesh;
class Resource;

class MeshComponent : public Component
{
public:
	MeshComponent(GameObject* own, TransformComponent* trans);
	MeshComponent(MeshComponent* meshComponent, TransformComponent* trans);
	~MeshComponent();

	uint32 SetDefaultShader();

	void Draw() override;
	void DrawScene() override;
	void DrawOutline() override;
	void OnEditor() override;

	bool OnLoad(JsonParsing& node) override;
	bool OnSave(JsonParsing& node, JSON_Array* array) override;

	void SetMesh(std::shared_ptr<Resource> m);

	inline void SetTransform(TransformComponent* trans) { transform = trans; }
	inline void SetMaterial(MaterialComponent* mat) { material = mat; }
	MaterialComponent* GetMaterial() { return material; }

	void EditorShader();

	inline Shader* const GetShader() { return shader; }
	uint32 const GetShaderID() { return shader->parameters.shaderID; }

	inline AABB GetLocalAABB() { return localBoundingBox; }
	const std::shared_ptr<Mesh> GetMesh() const { return mesh; }

	std::string GetNamefromPath(std::string path);

	void LoadShader(std::string path);

public:

	Shader* shadertoRecompily;

	TextEditor editor;
	std::string fileToEdit;
	bool showShaderEditor;

private:
	TransformComponent* transform;
	MaterialComponent* material;

	float normalLength;
	float3 colorNormal;
	
	bool faceNormals;
	bool verticesNormals;

	Shader* shader;

	std::shared_ptr<Mesh> mesh;

	AABB localBoundingBox;
	float a = 0;

	bool showMeshMenu;
};