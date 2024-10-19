#include "Common.h"
#include "Snow.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

CSnow::CSnow()
{}

CSnow::~CSnow()
{}

void CSnow::Create()
{
    // Define the vertices of the triangle
    std::vector<glm::vec3> triangleVertices = {
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.5f, 0.0f)
    };

    // Use VAO to store state associated with vertices
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create a VBO and bind it
    m_vbo.Create();
    m_vbo.Bind();

    // Upload vertex data to VBO
    m_vbo.AddVertexData(triangleVertices.data(), triangleVertices.size() * sizeof(glm::vec3));

    // Set up vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
}


void CSnow::Render()
{
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void CSnow::Release()
{
    // Unbind VAO and VBO
    glBindVertexArray(0);
    m_vbo.Release();
}
