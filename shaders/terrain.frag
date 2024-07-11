#version 400
/*	(C) 2022
	Bedrich Benes 
	Purdue University
	bbenes@purdue.edu
*/	

in float Height;
in float WaterHeight;
in float SnowHeight;
flat in vec3 Normal;
in vec3 fragPos;
out vec4 col;

struct DirectionLight
{
	vec3 color;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform DirectionLight directionalLight;
uniform Material material;
uniform vec3 cameraPosition;

void main()
{
	// calculate the color based on the height

	col = vec4(0.0, 1.0, 0.0, 1.0);

	if (Height >= SnowHeight)
	{
		col = vec4(mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0), Height), 1.0);
	}

	if (Height <= WaterHeight) 
	{
		col = vec4(mix(vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), Height), 1.0);
	}
	
	vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;

	float diffuseFactor = max(dot(Normal, normalize(directionalLight.direction)), 0.0);
	vec4 diffuseColor = vec4(directionalLight.color, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

	vec4 specularColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	if (diffuseFactor > 0.0)
	{
		vec3 fragPosToCamera = normalize(cameraPosition - fragPos);
		vec3 reflectedVertex = normalize(reflect(directionalLight.direction, normalize(Normal)));
		float specularFactor = dot(fragPosToCamera, reflectedVertex);

		if (specularFactor > 0.0)
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(directionalLight.color * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	col = col * (ambientColor + diffuseColor + specularColor);
}
