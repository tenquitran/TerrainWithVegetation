#pragma once


namespace TerrainWithVegetation
{
	class Terrain
	{
	public:
		// Parameters: scaleFactor - scale factor for the terrain heightmap.
		explicit Terrain(GLfloat scaleFactor);

		virtual ~Terrain();

		// Throws: std::bad_alloc
		bool initialize();

		// Update Model-View-Projection (MVP) and other matrices in the GLSL program.
		void updateViewMatrices(const std::unique_ptr<CommonLibOgl::Camera>& spCamera) const;

		void render() const;

	private:
		// Generate data for the terrain.
		// Parameters: vertices - vertex coordinates;
		//             indices - vertex indices;
		//             normals - vertex normals.
		// Returns: true on success, false otherwise.
		// Throws: std::bad_alloc
		bool generateTerrainData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, std::vector<glm::vec3>& normals);

		// Initialize texture data for the terrain.
		// Parameters: vertices - vertex coordinates.
		void generateTextureData(const std::vector<GLfloat>& vertices);

	private:
		// Terrain heightmap.
		Heightmap m_heightmap;

		std::unique_ptr<CommonLibOgl::ProgramGLSL> m_spProgram;    // GLSL program wrapper

		GLuint m_vao;
		GLuint m_vbo;

		GLuint m_index;          // index buffer
		GLsizei m_indexCount;    // number of indices

		GLuint m_normal;    // normal buffer

		// Minimum and maximum scaled height of the terrain.
		GLfloat m_minHeightScaled;
		GLfloat m_maxHeightScaled;

		// Buffer for texture presence data (one for all textures).
		GLuint m_texturePresence;

		// Tiled textures for the terrain.
		std::vector< std::unique_ptr<TiledTexture> > m_tiledTextures;

		// Number of different tiled textures.
		static const int TileCount = 3;
	};
}
