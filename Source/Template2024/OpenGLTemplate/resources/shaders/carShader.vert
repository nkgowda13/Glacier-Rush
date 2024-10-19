#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

uniform LightInfo light1;

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec3 vColorPass;
out vec2 vTexCoordPass;

void main()
{	
	vec3 p = inPosition;
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(p, 1.0);

	// Calculate diffuse light
    vec3 lightDir = normalize(vec3(light1.position));
    float diff = max(dot(inNormal, lightDir), 0.0);
    vec3 diffuse = diff * light1.Ld;

    // Calculate ambient light
    vec3 ambient = 0.3 * light1.La;

	// Combine diffuse, ambient, and specular lighting
    vColorPass = ambient + diffuse + light1.Ls;
	vTexCoordPass = inCoord;
} 
