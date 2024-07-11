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

// Ref: https://www.udemy.com/course/graphics-with-modern-opengl/learn/lecture/10017914#learning-tools

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

	col = col * (ambientColor + diffuseColor);
}
