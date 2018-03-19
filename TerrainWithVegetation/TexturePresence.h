#pragma once


namespace TerrainWithVegetation
{
	// Data about texture presence according to the terrain height.
	class TexturePresence
	{
	public:
		// Parameters: lowest - lowest height at which the texture is present;
		//             optimal - optimal height at which the texture is present;
		//             highest - highest height at which the texture is present.
		TexturePresence(int lowest, int optimal, int highest);

		virtual ~TexturePresence();

		// Get the amount of texture presence at the specified height, in "percent" (values from 0.0 through 1.0).
		// Parameters: height - height value (from 0 through 255).
		GLfloat getTexturePresencePercent(unsigned char height) const;

	private:
		// Lowest height at which the texture is present (texture presence is 0%).
		int m_lowestHeight;

		// Optimal height at which the texture is present (texture presence is 100%).
		int m_optimalHeight;

		// Highest height at which the texture is present (texture presence is 0%).
		int m_highestHeight;
	};
}
