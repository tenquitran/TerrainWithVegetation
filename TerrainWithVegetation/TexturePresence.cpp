#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace TerrainWithVegetation;

//////////////////////////////////////////////////////////////////////////


TexturePresence::TexturePresence(int lowest, int optimal, int highest)
	: m_lowestHeight(lowest), m_optimalHeight(optimal), m_highestHeight(highest)
{
}

TexturePresence::~TexturePresence()
{
}

GLfloat TexturePresence::getTexturePresencePercent(unsigned char height) const
{
	GLfloat result = {};

	if (   height <= m_lowestHeight
		|| height >= m_highestHeight)
	{
		return result;
	}

	if (height == m_optimalHeight)
	{
		result = 1.0f;
	}
	else if (height < m_optimalHeight)
	{
		// Height is below optimal.

		GLfloat temp1 = (GLfloat)m_lowestHeight - height;
		GLfloat temp2 = (GLfloat)m_optimalHeight - m_lowestHeight;

		result = abs(temp1 / temp2);
	}
	else
	{
		// Height is above optimal.

		GLfloat temp1 = (GLfloat)m_highestHeight - m_optimalHeight;

		result = abs((temp1 - (height - m_optimalHeight)) / temp1);
	}

	assert(result <= 1.0f);
	return result;
}
