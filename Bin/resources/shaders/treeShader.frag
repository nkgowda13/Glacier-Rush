#version 400 core


in vec3 vColour;
in vec2 vTexCoord;

out vec4 vOutputColour;

uniform sampler2D sampler0;

void main()
{	
	vec4 vTexColour = texture(sampler0, vTexCoord);	
	
    vOutputColour = vTexColour*vec4(vColour, 1.0f);
}
