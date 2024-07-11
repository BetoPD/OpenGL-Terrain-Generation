#version 400

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;

uniform mat4 modelview;
uniform float mountainHeight;
uniform float waterHeight;
uniform float snowHeight;
uniform mat4 model;
out float Height;
out float WaterHeight;
out float SnowHeight;
flat out vec3 Normal;
out vec3 fragPos;

void main()
{
   Height = iPos.y * mountainHeight;
   vec4 oPos = modelview * vec4(iPos.x, Height, iPos.z, 1.0);
   gl_Position = oPos;
   WaterHeight = waterHeight;
   SnowHeight = snowHeight;
   Normal = mat3(transpose(inverse(model))) * iNormal;
   fragPos = (model * vec4(iPos, 1.0)).xyz;
}