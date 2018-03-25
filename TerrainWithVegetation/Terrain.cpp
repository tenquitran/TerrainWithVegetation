#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace TerrainWithVegetation;

//////////////////////////////////////////////////////////////////////////


Terrain::Terrain(GLfloat scaleFactor)
	: m_heightmap(scaleFactor), m_vao{}, m_vbo{}, m_index{}, m_indexCount{}, m_normal{},
	  m_minHeightScaled{}, m_maxHeightScaled{}, m_texturePresence{}
{
}

Terrain::~Terrain()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (0 != m_texturePresence)
	{
		glDeleteBuffers(1, &m_texturePresence);
	}

	if (0 != m_normal)
	{
		glDeleteBuffers(1, &m_normal);
	}

	if (0 != m_index)
	{
		glDeleteBuffers(1, &m_index);
	}

	if (0 != m_vbo)
	{
		glDeleteBuffers(1, &m_vbo);
	}

	if (0 != m_vao)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_vao);
	}
}

bool Terrain::initialize()
{
	const std::string HeightmapPath = "data\\SochiArea1.png";

	if (!m_heightmap.loadFromFile(HeightmapPath))
	{
		return false;    // errors are already logged
	}

	// Initialize the program wrapper.

	const ShaderCollection shaders = {
		{ GL_VERTEX_SHADER,   "shaders\\terrain.vert" },

#if 0
		{ GL_GEOMETRY_SHADER, "shaders\\terrain.geom" },
#endif

		{ GL_FRAGMENT_SHADER, "shaders\\terrain.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	std::vector<glm::vec3> normals;

	if (!generateTerrainData(vertices, indices, normals))
	{
		return false;
	}

	m_indexCount = indices.size();

	glGenBuffers(1, &m_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Generate VBO and fill it with the data.

	// Vertex positions.

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	// Vertex normals.

	glGenBuffers(1, &m_normal);
	glBindBuffer(GL_ARRAY_BUFFER, m_normal);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	generateTextureData(vertices);

	GLuint program = m_spProgram->getProgram();

	glUseProgram(program);

#if 1
	// Set light properties.

	glm::vec3 ambient(0.15f, 0.15f, 0.15f);
	glm::vec3 diffuse(0.6f, 0.6f, 0.6f);
	glm::vec3 specular(0.25f, 0.25f, 0.25f);
	glm::vec3 lightPos(273.0f, 70.0f, 46.0f);
	//glm::vec3 lightPos(531.0f, 70.0f, 21.0f);
	//x=531.000000 y=37.4500008 z=21.0
	//273.000000 y=29.0499992 z=46.0

	glUniform3fv(3, 1, glm::value_ptr(ambient));
	glUniform3fv(4, 1, glm::value_ptr(diffuse));
	glUniform3fv(5, 1, glm::value_ptr(specular));
	glUniform3fv(6, 1, glm::value_ptr(lightPos));
#endif

	glUseProgram(0);

	glBindVertexArray(0);

	return true;
}

bool Terrain::generateTerrainData(std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices, std::vector<glm::vec3>& normals)
{
	// Width and height of the heightmap.
	const int Width  = m_heightmap.getWidth();
	const int Height = m_heightmap.getHeight();

	assert(Width > 0 && Height > 0);

	// Generate vertex coordinates.

	vertices.reserve(Width * Height);

	for (int z = 0; z < Height; ++z)
	{
		for (int x = 0; x < Width; ++x)
		{
			const GLfloat ScaledHeight = m_heightmap.getScaledHeightAtPoint(x, z);

			vertices.push_back(glm::vec3((GLfloat)x, ScaledHeight, (GLfloat)z));

			if (ScaledHeight < m_minHeightScaled)
			{
				m_minHeightScaled = ScaledHeight;
			}

			if (ScaledHeight > m_maxHeightScaled)
			{
				m_maxHeightScaled = ScaledHeight;
			}
		}
	}

	// Generate vertex indices.

	// Step 1. Generate consecutive integers to pick our indices from.

	std::unique_ptr< std::unique_ptr<GLuint[]>[] > spRawIndices =
		std::make_unique< std::unique_ptr<GLuint[]>[] >(Height);

	GLuint rawIndex = {};

	for (int row = 0; row < Height; ++row)
	{
		spRawIndices[row] = std::make_unique<GLuint[]>(Width);

		for (int col = 0; col < Width; ++col)
		{
			spRawIndices[row][col] = rawIndex++;
		}
	}

	// Step 2. Pick indices for vertex rows.
	// Each row of triangle strip uses two adjacent vertex rows (top to bottom), 
	// hence (height - 1) and  (width * 2) in the declaration below.
	// Besides, we need a degenerate triangle after each strip row to "stitch" the strip row with the next one;
	// that requires 2 indices after each row except the last one.

	indices.resize((Height - 1) * (Width * 2) + 2 * (Height - 2));

	int currIndex = {};

	for (int row = 0; row < (Height - 1); ++row)
	{
		for (int column = 0; column < Width; ++column)    // pick indices for the triangle strip row
		{
			indices[currIndex++] = spRawIndices[row][column];
			indices[currIndex++] = spRawIndices[row + 1][column];
		}

		// Add degenerate triangle to "stitch" the strip row with the next one (excluding the last strip row):
		// add the last index of the row we've just created, then the first index of the next row.

		if (row < (Height - 2))
		{
			indices[currIndex] = indices[currIndex - 1];
			++currIndex;
			
			indices[currIndex++] = spRawIndices[row + 1][0];
		}
	}

#if 0    // TODO: temp

	// Calculate normals.
	// NOTE: we should use the number of "raw" indices, as the index buffer contains duplicates.
	normals.reserve(rawIndex);

	for (int row = 0; row < Height; ++row)
	{
		for (int col = 0; col < Width; ++col)
		{
			normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}
#else
	// Calculate normals.
	// NOTE: we should use the number of "raw" indices, as the index buffer contains duplicates.
	//
	// Code from "3D Terrain Programming" by Trent Polack, Andre LaMothe (demo 8_2).
	normals.reserve(rawIndex);

	for (int row = 0; row < Height; ++row)
	{
		for (int col = 0; col < Width; ++col)
		{
			glm::vec3 *pVertex = &vertices[row * Width + col];

			glm::vec3 normal(0.0f, 1.0f, 0.0f);

			// above
			if (0 != row)
			{
				if (0 != col)
				{
					normal.x += -pVertex[-Width - 1][1];
					normal.z += -pVertex[-Width - 1][1];
				}
				else
				{
					normal.x += -pVertex[-Width][1];
					normal.z += -pVertex[-Width][1];
				}

				normal.x += -pVertex[-Width][1] * 2.0f;

				if ((Width - 1) != col)
				{
					normal.x += -pVertex[-Width + 1][1];
					normal.z += pVertex[-Width + 1][1];
				}
				else
				{
					normal.x += -pVertex[-Width][1];
					normal.z += pVertex[-Width][1];
				}
			}
			else
			{
				normal.x += -pVertex[0][1];
				normal.x += -pVertex[0][1] * 2.0f;
				normal.x += -pVertex[0][1];

				normal.z += -pVertex[0][1];
				normal.z += pVertex[0][1];
			}

			// Current line.
			if (0 != col)
			{
				normal.z += -pVertex[-1][1] * 2.0f;
			}
			else
			{
				normal.z += -pVertex[0][1] * 2.0f;
			}

			if (col != (Width - 1))
			{
				normal.z += pVertex[1][1] * 2.0f;
			}
			else
			{
				normal.z += pVertex[0][1] * 2.0f;
			}

			// Below.
			if ((Width - 1) != row)
			{
				if (0 != col)
				{
					normal.x += pVertex[Width - 1][1];
					normal.z += -pVertex[Width - 1][1];
				}
				else
				{
					normal.x += pVertex[Width][1];
					normal.z += -pVertex[Width][1];
				}

				normal.x += pVertex[Width][1] * 2.0f;

				if ((Width - 1) != col)
				{
					normal.x += pVertex[Width + 1][1];
					normal.z += pVertex[Width + 1][1];
				}
				else
				{
					normal.x += pVertex[Width][1];
					normal.z += pVertex[Width][1];
				}
			}
			else
			{
				normal.x += pVertex[0][1];
				normal.x += pVertex[0][1] * 2.0f;
				normal.x += pVertex[0][1];

				normal.z += -pVertex[0][1];
				normal.z += pVertex[0][1];
			}

			// Normalize the normal.
			float tmpf = 1.0f / (float)sqrt(normal.x * normal.x + normal.z * normal.z + 1.0f);
			normal.x *= tmpf;
			normal.y *= tmpf;
			normal.z *= tmpf;

			normals.push_back(normal);
		}
	}
#endif

	return true;
}

void Terrain::generateTextureData(const std::vector<glm::vec3>& vertices)
{
	// Required because the TiledTexture constructor calls glUniform*().
	glUseProgram(m_spProgram->getProgram());

	m_tiledTextures.resize(TileCount);

	const int HeightmapWidth  = m_heightmap.getWidth();
	const int HeightmapHeight = m_heightmap.getHeight();

	// Lowest and highest height values for a heightmap.
	const int LowestHeightInMap  = 0;
	const int HighestHeightInMap = 255;

	const int PresenceHalfWidth = HighestHeightInMap / (TileCount + 1);

	for (int tile = 0; tile < TileCount; ++tile)
	{
		std::string textureFile;
		std::string textureSamplerName;

		// Textures, from lowest to highest.
		switch (tile)
		{
		case 0:
			textureFile = "data/ground1.png";
			textureSamplerName = "firstTexture";
			break;
		case 1:
			textureFile = "data/ground2.png";
			textureSamplerName = "secondTexture";
			break;
		case 2:
			textureFile = "data/rock.png";
			textureSamplerName = "thirdTexture";
			break;
		}

		// Optimal heights of the tiles are arranged uniformly.
		const int optimalHeight = PresenceHalfWidth * (tile + 1);

		// For the first tile, the height should be the lowest possible.
		// For other tiles, the height should make 2/3 of the distance between the neighboring tiles optimal heights.
		const int lowestHeight = (0 == tile) ? LowestHeightInMap : (optimalHeight - 3 * PresenceHalfWidth / 4);

		// For the last tile, the height should be the highest possible.
		// For other tiles, the height should make 2/3 of the distance between the neighboring tiles optimal heights.
		const int highestHeight = (TileCount - 1 == tile) ? HighestHeightInMap : (optimalHeight + 3 * PresenceHalfWidth / 4);

		m_tiledTextures[tile] = std::make_unique<TiledTexture>(
			m_spProgram->getProgram(), textureFile, textureSamplerName, HeightmapWidth, HeightmapHeight, vertices,
			TexturePresence(lowestHeight, optimalHeight, highestHeight));
	}

	// Calculate texture presence data and pass them to the shaders.

	// Texture presence data for all three textures, one after the other.
	std::vector<GLfloat> texturePresence(HeightmapHeight * HeightmapWidth * TileCount);

	size_t indexOffset = {};    // data offset for each texture

	for (int tile = 0; tile < TileCount; ++tile)
	{
		size_t heightIndex = {};

		for (int z = 0; z < HeightmapHeight; ++z)
		{
			for (int x = 0; x < HeightmapWidth; ++x)
			{
				unsigned char height = m_heightmap.getActualHeightAtPoint(x, z);

				GLfloat percentage = m_tiledTextures[tile]->getTexturePresencePercent(height);

				texturePresence[indexOffset + heightIndex++] = percentage;
			}
		}

		indexOffset += (HeightmapHeight * HeightmapWidth);
	}

	glGenBuffers(1, &m_texturePresence);
	glBindBuffer(GL_ARRAY_BUFFER, m_texturePresence);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texturePresence.size(), &texturePresence[0], GL_STATIC_DRAW);

	const GLuint attrTextureCoordinate1 = 3;
	glVertexAttribPointer(attrTextureCoordinate1, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrTextureCoordinate1);

	int offset = HeightmapHeight * HeightmapWidth * sizeof(GLfloat);

	const GLuint attrTextureCoordinate2 = 4;
	glVertexAttribPointer(attrTextureCoordinate2, 1, GL_FLOAT, GL_FALSE, 0, (void *)offset);
	glEnableVertexAttribArray(attrTextureCoordinate2);

	offset += HeightmapHeight * HeightmapWidth * sizeof(GLfloat);

	const GLuint attrTextureCoordinate3 = 5;
	glVertexAttribPointer(attrTextureCoordinate3, 1, GL_FLOAT, GL_FALSE, 0, (void *)offset);
	glEnableVertexAttribArray(attrTextureCoordinate3);

	glUseProgram(0);
}

void Terrain::updateViewMatrices(const std::unique_ptr<Camera>& spCamera) const
{
	assert(m_spProgram);

	// TODO: turn on
#if 0
	assert(-1 != m_unNormal);
#endif

	glUseProgram(m_spProgram->getProgram());

	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(spCamera->getModelViewProjectionMatrix()));

#if 1
	//glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(spCamera->getProjectionMatrix()));

	glm::mat4 modelView = spCamera->getModelViewMatrix();

	// WARNING: we are using the fact that there are no non-uniform scaling. If this will change, use the entire 4x4 matrix.
	glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(modelView)));

	//Normal = mat3(transpose(inverse(model))) * normal;

	glUniformMatrix3fv(1, 1, GL_FALSE, glm::value_ptr(normal));

	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(modelView));
#endif

	glUseProgram(0);
}

void Terrain::render() const
{
	assert(m_spProgram);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(m_spProgram->getProgram());

	glBindVertexArray(m_vao);

	// TODO: uncomment
#if 1
	for (int tile = 0; tile < TileCount; ++tile)
	{
		glActiveTexture(GL_TEXTURE0 + tile);
		glBindTexture(GL_TEXTURE_2D, m_tiledTextures[tile]->m_texture);
	}
#endif

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);

	glDrawElements(GL_TRIANGLE_STRIP, m_indexCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// TODO: uncomment
#if 1
	for (int tile = 0; tile < TileCount; ++tile)
	{
		glActiveTexture(GL_TEXTURE0 + tile);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
#endif

	glBindVertexArray(0);
	glUseProgram(0);
}
