#include "Common.h"
#include "Tree.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


CTree::CTree()
{}

CTree::~CTree()
{}


// Create the plane, including its geometry, texture mapping, normal, and colour
void CTree::Create(string directory, string filename)
{
    // Load the texture
    m_texture.Load(directory + filename, true);

    m_directory = directory;
    m_filename = filename;

    // Set parameters for texturing using sampler object
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Use VAO to store state associated with vertices
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create a VBO
    m_vbo.Create();
    m_vbo.Bind();

    auto vertices = GetVertices();
    auto indices = GetIndices();
    auto normals = GetNormals(vertices, indices);
    auto texCoords = GetTexCoords();

    for (int i = 0; i < vertices.size(); i++)
    {
        m_vbo.AddVertexData(&vertices.at(i), sizeof(glm::vec3));
        m_vbo.AddVertexData(&texCoords[i], sizeof(glm::vec2));
        m_vbo.AddVertexData(&normals[i], sizeof(glm::vec3));
    }

    for (int i = 0; i < indices.size(); i++)
    {
        m_vbo.AddIndexData(&indices[i][0], sizeof(unsigned int));
        m_vbo.AddIndexData(&indices[i][1], sizeof(unsigned int));
        m_vbo.AddIndexData(&indices[i][2], sizeof(unsigned int));
        m_numTriangles++;
    }

    // Upload the VBO to the GPU
    m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

    GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

std::vector<std::vector<int>> CTree::GetIndices()
{
    std::vector<std::vector<int>> indices{
        std::vector<int>{0, 1, 2}, //base
            std::vector<int>{0, 2, 3},  //base
            std::vector<int>{0, 4, 5}, //Bark Left 
            std::vector<int>{0, 5, 1}, //Bark Left 
            std::vector<int>{2, 1, 5}, //Bark Back
            std::vector<int>{2, 5, 6}, //Bark Back
            std::vector<int>{3, 2, 6}, //Bark Right 
            std::vector<int>{3, 6, 7}, //Bark Right 
            std::vector<int>{0, 3, 7}, //Bark Front 
            std::vector<int>{0, 7, 4}, //Bark Front 
            std::vector<int>{4, 8, 5},
            std::vector<int>{5, 8, 9},
            std::vector<int>{5, 9, 6},
            std::vector<int>{6, 9, 10},
            std::vector<int>{6, 10, 7},
            std::vector<int>{7, 10, 11},
            std::vector<int>{7, 11, 4},
            std::vector<int>{4, 11, 8},
            std::vector<int>{12, 9, 8}, // Tree Right
            std::vector<int>{12, 10, 9}, // Tree Front
            std::vector<int>{12, 11, 10}, // Tree Left
            std::vector<int>{12, 8, 11} // Tree Back
    };
    return indices;
}

std::vector < glm::vec2 > CTree::GetTexCoords()
{
    std::vector<glm::vec2> texCoords{
            // Bark vertices
            glm::vec2(0.0f, 0.05f), // Bottom left
            glm::vec2(0.25f, 0.05f), // Top left
            glm::vec2(0.5f, 0.05f), // Top right
            glm::vec2(0.75f, 0.05f), // Bottom right

            // Upper bark vertices
            glm::vec2(0.0f, 0.25f), // Bottom left
            glm::vec2(0.25f, 0.25f), // Top left
            glm::vec2(0.5f, 0.25f), // Top right
            glm::vec2(0.75f, 0.25f), // Bottom right

            // Lower tree vertices
            glm::vec2(0.0f, 0.25f), // Bottom left
            glm::vec2(0.0f, 1.f), // Top left
            glm::vec2(1.f, 1.f), // Top right
            glm::vec2(1.f, 0.25f), // Bottom right

            // Upper tree vertice
            glm::vec2(0.5f, 0.625f)  // Center
    };

    return texCoords;
}

std::vector<glm::vec3> CTree::GetVertices()
{
    float barkSize = 0.5;
    int barkLength = 4;
    int treeSize = 4;
    int treeHeight = 20;

    std::vector<glm::vec3> vertices {

            glm::vec3(-barkSize, 0, barkSize),
            glm::vec3(-barkSize, 0.f, -barkSize),
            glm::vec3(barkSize, 0.f, -barkSize),
            glm::vec3(barkSize, 0.f, barkSize),

            glm::vec3(-barkSize, barkLength, barkSize),
            glm::vec3(-barkSize, barkLength, -barkSize),
            glm::vec3(barkSize, barkLength, -barkSize),
            glm::vec3(barkSize, barkLength, barkSize),

            glm::vec3(-treeSize, barkLength, treeSize),
            glm::vec3(-treeSize, barkLength, -treeSize),
            glm::vec3(treeSize, barkLength, -treeSize),
            glm::vec3(treeSize, barkLength, treeSize),

            glm::vec3(0, treeHeight, 0)

    };

    return vertices;
}

std::vector<glm::vec3> CTree::GetNormals(const std::vector<glm::vec3>& vertices, const std::vector<std::vector<int>>& indices) {
    std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));
    std::vector<int> count(vertices.size(), 0);

    for (auto triangle : indices) {
        glm::vec3 v0 = vertices[triangle[0]];
        glm::vec3 v1 = vertices[triangle[1]];
        glm::vec3 v2 = vertices[triangle[2]];

        // Get the normal for this triangle
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        for (int i = 0; i < 3; i++) {
            normals[triangle[i]] += normal;
            count[triangle[i]]++;
        }
    }

    // Average the normals
    for (size_t i = 0; i < normals.size(); ++i) {
        normals[i] /= static_cast<float>(count[i]);
        normals[i] = glm::normalize(normals[i]);
    }

    return normals;
}

void CTree::Render()
{
    glBindVertexArray(m_vao);
    m_texture.Bind();
    glDrawElements(GL_TRIANGLES, m_numTriangles * 3, GL_UNSIGNED_INT, 0);
}

// Release resources
void CTree::Release()
{
    m_texture.Release();
    glDeleteVertexArrays(1, &m_vao);
    m_vbo.Release();
}