#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace TerrainWithVegetation;

//////////////////////////////////////////////////////////////////////////


Heightmap::Heightmap(GLfloat scaleFactor)
	: ScaleFactor(scaleFactor), m_pData(nullptr), m_size{}, m_width{}, m_height{}
{
}

Heightmap::~Heightmap()
{
	if (m_pData)
	{
		SOIL_free_image_data(m_pData);
	}
}

bool Heightmap::isEmpty() const
{
	return (nullptr == m_pData);
}

int Heightmap::getWidth() const
{
	return m_width;
}

int Heightmap::getHeight() const
{
	return m_height;
}

bool Heightmap::loadFromFile(const std::string& filePath)
{
	// Release the previous heightmap data.
	if (m_pData)
	{
		SOIL_free_image_data(m_pData);
		m_pData = nullptr;

		m_width = {};
		m_height = {};
		m_size = {};
	}

	// The SOIL_LOAD_L flag loads the heightmap as a greyscale image.
	m_pData = SOIL_load_image(filePath.c_str(), &m_width, &m_height, 0, SOIL_LOAD_L);
	if (!m_pData)
	{
		std::wcerr << L"Failed to load heightmap \"" << filePath.c_str() << "\": " << SOIL_last_result() << '\n';
		assert(false); return false;
	}

	m_size = m_width * m_height;

	return true;
}

unsigned char Heightmap::getActualHeightAtPoint(int x, int z) const
{
	return (*this)[z * m_width + x];
}

GLfloat Heightmap::getScaledHeightAtPoint(int x, int z) const
{
	return (ScaleFactor * (GLfloat)(*this)[z * m_width + x]);
}

unsigned char Heightmap::operator[](int index) const
{
	if (   index < 0
		|| index >= m_size)
	{
		throw EXCEPTION_FMT(L"Index %d out of bounds", index);
	}

	return m_pData[index];
}
