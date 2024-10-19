#version 400 core

in vec3 vColor;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;


void main()
{
	vec4 vTexColour = texture(sampler0, vTexCoord);	
	vOutputColour = vTexColour*vec4(vColor, 1.0f);	// Combine object colour and texture 
		
}
