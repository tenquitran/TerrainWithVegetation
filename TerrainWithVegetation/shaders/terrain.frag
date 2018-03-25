#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 3) uniform vec3 lightAmbient;
layout (location = 4) uniform vec3 lightDiffuse;
layout (location = 5) uniform vec3 lightSpecular;
layout (location = 6) uniform vec3 lightPosition;

in vec2 texCoord2;
in vec3 vPos;
in vec3 vNormal;

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

	vec3 n = normalize(vNormal);
	vec3 s = normalize(lightPosition - vPos);
	vec3 v = normalize(vec3(-vPos));
	vec3 r = reflect(-s, n);

	// We suppose that ambient, diffuse and specular material components are represented by the combined texture color.

	const float shininess = 32.0f;    // hard-coded

	vec3 ambient  = lightAmbient  * vec3(texColor);
	vec3 diffuse  = lightDiffuse  * vec3(texColor) * max(dot(s, n), 0.0);
	vec3 specular = lightSpecular * vec3(texColor) * pow(max(dot(r, v), 0.0), shininess);

	outColor = vec4(ambient + diffuse + specular, 1.0);
	//outColor = vec4(diffuse, 1.0);

	// TODO: temp
	/*
	if (0.0 == diffuse.r && 0.0 == diffuse.g && 0.0 == diffuse.b)
	{
		outColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	*/

	// Color of textures (without Phong lighting).
	//outColor = texColor;

	// TODO: hard-coded color
	//outColor = vec4(0.0f, 1.0f, 0.5f, 1.0f);
}
