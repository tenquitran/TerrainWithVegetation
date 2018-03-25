#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

// Presence of the textures (in percent).
layout(location = 3) in float firstTexturePresence;
layout(location = 4) in float secondTexturePresence;
layout(location = 5) in float thirdTexturePresence;

layout (location = 0) uniform mat4 MVP;
layout (location = 1) uniform mat3 Normal;
layout (location = 2) uniform mat4 ModelView;

out vec2 texCoord2;
out vec3 vPos;
out vec3 vNormal;

// Presence of the textures (in percent).
out float firstTexturePresence2;
out float secondTexturePresence2;
out float thirdTexturePresence2;

/*
out VS_OUT {
	vec3 normalToGeom;
} vs_out;
*/

void main(void) 
{
	vNormal = normalize(Normal * normal);

	vPos = vec3(ModelView * vec4(position, 1.0));

	texCoord2 = texCoord;

	firstTexturePresence2  = firstTexturePresence;
	secondTexturePresence2 = secondTexturePresence;
	thirdTexturePresence2  = thirdTexturePresence;

	//vs_out.normalToGeom = normalize(vec3(Projection * vec4(Normal * normal, 0.0)));

    gl_Position = MVP * vec4(position, 1.0);
}
