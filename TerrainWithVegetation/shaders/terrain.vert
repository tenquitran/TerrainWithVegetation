#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

// Presence of the textures (in percent).
layout(location = 3) in float firstTexturePresence;
layout(location = 4) in float secondTexturePresence;
layout(location = 5) in float thirdTexturePresence;

layout (location = 0) uniform mat4 MVP;

out vec2 texCoord2;

// Presence of the textures (in percent).
out float firstTexturePresence2;
out float secondTexturePresence2;
out float thirdTexturePresence2;

void main(void) 
{
	texCoord2 = texCoord;

	firstTexturePresence2  = firstTexturePresence;
	secondTexturePresence2 = secondTexturePresence;
	thirdTexturePresence2  = thirdTexturePresence;

    gl_Position = MVP * vec4(position, 1.0);
}
