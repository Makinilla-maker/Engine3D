#pragma once
#include "Component.h"
#include "Globals.h"
#include <string.h>
#include "Math/float3.h"
#include "Math/float2.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "par_shapes.h"

class ComponentMesh : public Component {

public:
	
	enum class Shape
	{
		CUBE,
		SPHERE,
		CYLINDER
	};

	ComponentMesh(GameObject* parent);
	ComponentMesh(GameObject* parent, Shape shape);
	~ComponentMesh();

	void CopyParMesh(par_shapes_mesh* parMesh);

	void GenerateBuffers();
	void ComputeNormals();
	void GenerateBounds();
	bool IsInsideFrustum(Frustum* camFrustum);
	void DrawNormals() const;
	float3 GetCenterPointInWorldCoords() const;
	inline float GetSphereRadius() const { return radius; }

	bool Update(float dt) override;
	void OnGui() override;

	uint vertexBufferId = 0, indexBufferId = 0, textureBufferId = 0;
	std::string texturePath;
	
	uint numVertices = 0;
	std::vector<float3> vertices;

	uint numNormalFaces = 0;
	std::vector<float3> normals;
	std::vector<float3> faceNormals;
	std::vector<float3> faceCenters;

	std::vector<float2> texCoords;

	uint numIndices = 0;
	std::vector<uint> indices;

	bool drawWireframe = false;
	bool drawVertexNormals = false;
	bool drawFaceNormals = false;
	bool showBoxAABB = true;
	float normalScale = 1.f;

	AABB globalAABB;
	OBB globalOBB;
	
private:

	//Bounding sphere
	float3 centerPoint = float3::zero;
	float radius;

	//Local coords AABB
	AABB localAABB;
	
};