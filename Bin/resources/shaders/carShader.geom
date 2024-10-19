#version 400

in vec2 vTexCoordPass[];
in vec3 vColorPass[];


// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

out vec2 vTexCoord;
out vec3 vColor;

uniform bool bExplodeObject;
uniform float explodeFactor;

uniform bool bJoinObject;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main() 
{
	float localExplode = bExplodeObject? explodeFactor: 0.0;

	// Get triangle normal
	vec3 e1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 e2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 n = -normalize(cross(e1, e2));

	for(int i = 0; i < 3; i++) { 

		if(bExplodeObject)
		{
			//vec4 explodedPos = gl_in[i].gl_Position + localExplode * vec4(n, 0);
			gl_Position = (explodeFactor * vec4(n,1) * 2) + gl_in[i].gl_Position;
		}
		else if(bJoinObject)
		{
			gl_Position = gl_in[i].gl_Position - (explodeFactor * vec4(n,1) * 2);
		}
		else
		{
			gl_Position = gl_in[i].gl_Position;
		}
		
		vTexCoord = vTexCoordPass[i];
		vColor = vColorPass[i];

		EmitVertex();
	}
	EndPrimitive();
}