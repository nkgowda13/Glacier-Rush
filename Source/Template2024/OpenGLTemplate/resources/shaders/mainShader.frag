#version 400 core

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUsePhongModel;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
uniform bool bUseSpotlight;    // A flag indicating if spotlight should be shown
uniform bool bUseStreetlight;    // A flag indicating if spotlight should be shown

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Structure holding spotlight information
struct SpotlightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1;
uniform SpotlightInfo spotlight1;
uniform SpotlightInfo spotlight2;
uniform SpotlightInfo spotlights[4];
uniform SpotlightInfo streetlights[5];
uniform MaterialInfo material1; 
uniform MaterialInfo spotmaterial1;
uniform MaterialInfo streetmaterial1;

in vec3 worldPosition;
in vec3 n;
in vec4 p;
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader
in vec3 vNormal;


// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;
}

vec3 BlinnPhongSpotlightModel(SpotlightInfo light, vec4 p, vec3 n, vec4 position, MaterialInfo material)
{
	vec3 s = normalize(vec3(position - p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
	vec3 ambient = light.La * material.Ma;
	if (angle < cutoff) {
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = light.Ld * material.Md * sDotN;
		vec3 specular = vec3(0.0);
		if (sDotN > 0.0)
			specular = light.Ls * material.Ms * pow(max(dot(h, n), 0.0), material.shininess);
		return ambient + spotFactor * (diffuse + specular);
	} else 
		return ambient;
}

// Fog parameters
float rho = 0.001f;
const vec3 fogColour = vec3(0.75f);

void main()
{
	vec3 vColour;
	
	if(bUseSpotlight)
	{
		rho = 0.001f;
		vColour += 2 * BlinnPhongSpotlightModel(spotlight1, p, n, spotlight1.position, spotmaterial1) + 2 * BlinnPhongSpotlightModel(spotlight2, p, n, spotlight2.position, spotmaterial1);

		for(int i = 0; i < 4; i++)
		{
			vColour += BlinnPhongSpotlightModel(spotlights[i], p, n, spotlights[i].position, spotmaterial1);
		}
	}
	if(bUseStreetlight)
	{
		rho = 0.003f;
		for (int i = 0; i < 5; ++i)
		{
			vColour += BlinnPhongSpotlightModel(streetlights[i], p, n, streetlights[i].position, streetmaterial1);
		}
	}
	if(bUsePhongModel)
	{
		// Apply the Phong model to compute the vertex colour
		vColour += PhongModel(p, n);
	}
	else
	{
		// Calculate diffuse light
		vec3 lightDir = normalize(vec3(light1.position));
		float diff = max(dot(vNormal, lightDir), 0.0);
		vec3 diffuse = diff * light1.Ld;
	
		 // Calculate ambient light
		vec3 ambient = 0.3 * light1.La;
	
		// Combine diffuse, ambient, and specular lighting
		vColour = ambient + diffuse + light1.Ls;
	}

	

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);
	}
	else 
	{
		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture)
			vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
		else
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead

	}

	// Compute fog factor
    float d = length(p.xyz);  // Distance from the camera
    float fogFactor = exp(-rho * d);  // Exponential fog factor
    
    // Apply fog by linear interpolation between fog colour and output colour
    vOutputColour.rgb = mix(fogColour, vOutputColour.rgb, fogFactor);
}