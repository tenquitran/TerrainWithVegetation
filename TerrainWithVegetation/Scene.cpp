#include "stdafx.h"
#include "Scene.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace TerrainWithVegetation;

//////////////////////////////////////////////////////////////////////////


Scene::Scene(GLfloat terrainScaleFactor)
	: m_terrain(terrainScaleFactor)
{
}

Scene::~Scene()
{
}

bool Scene::initialize(const OpenGLInfo& openGlInfo)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_MULTISAMPLE);

	glClearColor(0.8f, 0.93f, 0.96f, 1.0f);    // very light blue

	// Initial scale factor for the camera.
	const GLfloat CameraScaleFactor = 0.02f;
	//const GLfloat CameraScaleFactor = 1.0f;

    glm::vec3 cameraPosition = {0.0f, 0.0f, -5.0f};

    m_spCamera = std::make_unique<Camera>(cameraPosition, CameraScaleFactor,
		openGlInfo.FieldOfView, openGlInfo.FrustumNear, openGlInfo.FrustumFar);

#if 1
	// Get our future terrain in focus.
    m_spCamera->translate({ -0.8f, -0.8f, -1.5f });
#endif

	// Initialize the program wrapper.

	const ShaderCollection shaders = {
		{ GL_VERTEX_SHADER,   "shaders\\terrain.vert" },
		{ GL_FRAGMENT_SHADER, "shaders\\terrain.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	if (!m_terrain.initialize())
	{
		std::wcerr << L"Terrain initialization failed\n";
		assert(false); return false;
	}

	// TODO: set uniforms if required
#if 0
	glUseProgram(m_spProgram->getProgram());

	;

	glUseProgram(0);
#endif

	if (!m_spProgram->validate())
	{
		std::wcerr << L"GLSL program validation failed\n";
		assert(false); return false;
	}

#if 1
	updateViewMatrices();
#endif

	return true;
}

void Scene::updateViewMatrices() const
{
	m_terrain.updateViewMatrices(m_spCamera);
}

void Scene::translateCamera(const glm::vec3& diff)
{
	m_spCamera->translate(diff);

	updateViewMatrices();
}

void Scene::rotateCamera(const glm::vec3& degrees)
{
	m_spCamera->rotate(degrees);

	updateViewMatrices();
}

GLfloat Scene::getCameraScale() const
{
	return m_spCamera->getScale();
}

void Scene::scaleCamera(GLfloat amount)
{
	m_spCamera->scale(amount);

	updateViewMatrices();
}

void Scene::resize(GLfloat aspectRatio)
{
	m_spCamera->resize(aspectRatio);

	updateViewMatrices();
}

void Scene::render() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_terrain.render();
}
