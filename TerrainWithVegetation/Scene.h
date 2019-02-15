#pragma once


namespace TerrainWithVegetation
{
	// OpenGL scene.
	class Scene
	{
	public:
		// Parameters: scaleFactor - scale factor for the terrain heightmap.
		explicit Scene(GLfloat terrainScaleFactor);

		virtual ~Scene();

		// Parameters: openGlInfo - OpenGL settings.
		bool initialize(const CommonLibOgl::OpenGLInfo& openGlInfo);

		// Update Model-View-Projection (MVP) and other matrices in the GLSL program.
		void updateViewMatrices() const;

		void resize(GLfloat aspectRatio);

		void render() const;

		//////////////////////////////////////////////////////////////////////////
		// Camera control.
		//////////////////////////////////////////////////////////////////////////

        void translateCamera(const glm::vec3& diff);

        void rotateCamera(const glm::vec3& degrees);

		GLfloat getCameraScale() const;

		void scaleCamera(GLfloat amount);

	private:
		std::unique_ptr<CommonLibOgl::ProgramGLSL> m_spProgram;

		// OpenGL camera.
		std::unique_ptr<CommonLibOgl::Camera> m_spCamera;

		Terrain m_terrain;
	};
}
