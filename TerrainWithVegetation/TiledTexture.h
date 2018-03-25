#pragma once


namespace TerrainWithVegetation
{
	class TiledTexture
	{
	public:
		// Parameters: program - GLSL program object;
		//             fileName - name or relative path of the texture file;
		//             textureSamplerName - name of the texture sampler uniform in the fragment shader;
		//             surfaceWidth  - width of the surface to texture;
		//             surfaceHeight - height of the surface to texture;
		//             vertices - vector coordinates of the surface;
		//             texturePresence - data to calculate the texture presence depending on terrain height.
		// Throws: std::bad_alloc
		TiledTexture(const GLuint& program, const std::string& fileName, const std::string& textureSamplerName,
			int surfaceWidth, int surfaceHeight, const std::vector<glm::vec3>& vertices, const TexturePresence& texturePresence);

		virtual ~TiledTexture();

		// Get the amount of texture presence at the specified height, in "percent" (values from 0.0 through 1.0).
		// Parameters: height - height value (from 0 through 255).
		GLfloat getTexturePresencePercent(unsigned char height) const;

	private:
		TiledTexture(const TiledTexture&) = delete;
		TiledTexture& operator=(const TiledTexture&) = delete;

		// Parameters: program - GLSL program object;
		//             textureSamplerName - name of the texture sampler uniform in the fragment shader;
		//             vertices - vector coordinates of the surface.
		// Throws: std::bad_alloc
		bool initialize(const GLuint& program, const std::string& textureSamplerName, const std::vector<glm::vec3>& vertices);

		// TODO: temp
	public:
		// Texture object.
		GLuint m_texture;

	private:
		// Name or relative path of the texture file.
		std::string m_fileName;

		// Texture sampler uniform.
		GLint m_uTextureSampler;

		// Sampler value for the next texture.
		static GLint m_nextSampler;

		// Sampler value for the current texture.
		const GLint m_currentSampler;

		// Texture coordinates buffer and its data.
		GLuint m_textureCoordBuffer;

		// Width and height of the texture.
		int m_textureWidth;
		int m_textureHeight;

		// Width and height of the surface to texture.
		int m_surfaceWidth;
		int m_surfaceHeight;

		// Data to calculate the texture presence depending on terrain height.
		TexturePresence m_texturePresence;
	};
}
