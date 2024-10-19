#pragma once

#include "Texture.h"
#include "VertexBufferObjectIndexed.h"
#include "VertexBufferObject.h"

// Class for generating a unit sphere
class CSnow
{
public:
	CSnow();
	~CSnow();
	void Create();
	void Render();
	void Release();
private:
	UINT m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	int m_numTriangles;
};
