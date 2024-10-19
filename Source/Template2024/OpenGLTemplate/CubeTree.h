#pragma once

#include "Texture.h"
#include "VertexBufferObjectIndexed.h"
#include "VertexBufferObject.h"

// Class for generating a unit sphere
class CCubeTree
{
public:
	CCubeTree();
	~CCubeTree();
	void Create(string sDirectory, string sFilename);
	void Render();
	void Release();
	std::vector<glm::vec3> GetVertices();
	std::vector<std::vector<int>> GetIndices();
	std::vector<glm::vec3> GetNormals(const std::vector<glm::vec3>&, const std::vector<std::vector<int>>&);
	std::vector<glm::vec2> GetTexCoords();
private:
	UINT m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	int m_numTriangles;
};