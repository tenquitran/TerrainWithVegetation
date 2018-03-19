#version 440 core
#pragma debug(on)
#pragma optimize(off)

in vec2 texCoord2;

// Presence of the textures (in percent).
in float firstTexturePresence2;
in float secondTexturePresence2;
in float thirdTexturePresence2;

layout(binding=0) uniform sampler2D firstTexture;
layout(binding=1) uniform sampler2D secondTexture;
layout(binding=2) uniform sampler2D thirdTexture;

out vec4 outColor;

void main()
{
	// Multitexturing (using fixed texture weights).

	vec4 firstTexColor  = texture(firstTexture,  texCoord2);
	vec4 secondTexColor = texture(secondTexture, texCoord2);
	vec4 thirdTexColor  = texture(thirdTexture,  texCoord2);

	vec4 texColor;

	if (firstTexturePresence2 <= 0.0)
	{
		texColor = mix(thirdTexColor, secondTexColor, secondTexturePresence2);
	}
	else if (secondTexturePresence2 <= 0.0)
	{
		texColor = mix(firstTexColor, thirdTexColor, thirdTexturePresence2);
	}
	else if (thirdTexturePresence2 <= 0.0)
	{
		texColor = mix(firstTexColor, secondTexColor, secondTexturePresence2);
	}

	outColor = texColor;

	// TODO: hard-coded color
	//outColor = vec4(0.0f, 1.0f, 0.5f, 1.0f);
}
