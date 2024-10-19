/*
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting,
 different camera controls, different shaders, etc.

 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk)

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 version 6.1a 13/02/2022 - Sorted out Release mode and a few small compiler warnings
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "FrameBufferObject.h"
#include <chrono>


// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pTVCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pFtFont = NULL;
	m_pCarMesh = NULL;
	m_pIceMesh = NULL;
	m_pSignMesh = NULL;
	m_pTunnelMesh = NULL;
	m_pBarricadeMesh = NULL;
	m_pSnowmanMesh = NULL;
	m_pIceBergMesh = NULL;
	m_pStreetLightMesh = NULL;
	m_pTree = NULL;
	m_pSnow = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pHeightmapTerrain = NULL;
	m_pCatmullRom = NULL;
	m_pCatmullRomRight = NULL;
	m_pCatmullRomLeft = NULL;
	m_pPlane = NULL;
	m_pPlaneFBO = NULL;
	m_pSpeedometerImage = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_shaderElapsedTime = 0.0f;

	m_totalTime = 0.0f;
	m_cameraSpeed = 3000;
	m_cameraRadius = 50;
	m_currentDistance = 0;
}

// Destructor
Game::~Game()
{
	//game objects
	delete m_pCamera;
	delete m_pTVCamera;
	delete m_pSkybox;
	delete m_pFtFont;
	delete m_pCarMesh;
	delete m_pIceMesh;
	delete m_pSnow;
	delete m_pSignMesh;
	delete m_pTunnelMesh;
	delete m_pBarricadeMesh;
	delete m_pIceBergMesh;
	delete m_pStreetLightMesh;
	delete m_pTree;
	delete m_pCubeTree;
	delete m_pAudio;
	delete m_pHeightmapTerrain;
	delete m_pCatmullRom;
	delete m_pCatmullRomLeft;
	delete m_pCatmullRomRight;
	delete m_pPlane;
	delete m_pPlaneFBO;
	delete m_pSpeedometerImage;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise()
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	// Create objects
	m_pCamera = new CCamera;
	m_pTVCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram*>;
	m_pFtFont = new CFreeTypeFont;
	m_pCarMesh = new COpenAssetImportMesh;
	m_pCarMesh1 = new COpenAssetImportMesh;
	m_pCarMesh2 = new COpenAssetImportMesh;
	m_pSignMesh = new COpenAssetImportMesh;
	m_pTunnelMesh = new COpenAssetImportMesh;
	m_pBarricadeMesh = new COpenAssetImportMesh;
	m_pIceMesh = new COpenAssetImportMesh;
	m_pIceBergMesh = new COpenAssetImportMesh;
	m_pSnowmanMesh = new COpenAssetImportMesh;
	m_pStreetLightMesh = new COpenAssetImportMesh;
	m_pTree = new CTree;
	m_pSnow = new CSnow;
	m_pCubeTree = new CCubeTree;
	m_pAudio = new CAudio;
	m_pHeightmapTerrain = new CHeightMapTerrain;
	m_pCatmullRom = new CCatmullRom;
	m_pCatmullRomLeft = new CCatmullRom;
	m_pCatmullRomRight = new CCatmullRom;
	m_cameraType = Third;
	m_movePlayer = None;
	m_gameMode = Light;
	m_playerSpeed = 0;
	m_increaseSpeed = false;
	m_health = 20;
	m_collisionDistance = 0;
	m_gameOver = false;
	m_explodeFactor = 0;
	m_pGameLoopTimer = new CHighResolutionTimer;
	m_pPlane = new CPlane;
	m_pPlaneFBO = new CFrameBufferObject;
	m_pSpeedometerImage = new CTexture;

	m_resetCar = false;
	m_lives = 3;

	RECT dimensions = m_gameWindow.GetDimensions();

	width = dimensions.right - dimensions.left;
	height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height);
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float)width / (float)height, 0.5f, 5000.0f);
	m_pTVCamera->SetOrthographicProjectionMatrix(width, height);
	m_pTVCamera->SetPerspectiveProjectionMatrix(45.0f, (float)width / (float)height, 0.5f, 5000.0f);

	LoadShaders();

	// Load Textures
	m_pSpeedometerImage->Load("resources\\textures\\grass.jpg");

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);

	// Create the heightmap terrain
	string terrainMap = "resources\\textures\\terrain.bmp";
	string terrainTex = "resources\\textures\\Ice.jpg";
	m_pHeightmapTerrain->Create(&terrainMap[0], &terrainTex[0], glm::vec3(0, 1, 0), 7000.0f, 7000.0f, 175.f);

	// Load some meshes
	m_pCarMesh->Load("resources\\models\\Car\\maincar.fbx");
	m_pCarMesh1->Load("resources\\models\\Car\\car1.fbx");
	m_pCarMesh2->Load("resources\\models\\Car\\car2.fbx");
	m_pSignMesh->Load("resources\\models\\TunnelSign\\objSign.obj");
	m_pTunnelMesh->Load("resources\\models\\Tunnel\\tunnel.obj");
	m_pBarricadeMesh->Load("resources\\models\\Barricade\\Barricade1.fbx");
	m_pIceMesh->Load("resources\\models\\Iceberg\\gg.fbx");
	m_pStreetLightMesh->Load("resources\\models\\StreetLight\\streetlight.obj");
	m_pIceBergMesh->Load("resources\\models\\Iceberg\\ice.obj");
	m_pSnowmanMesh->Load("resources\\models\\Iceberg\\snowman.obj");

	// Create the plane for the tv
	m_pPlane->Create("resources\\textures\\", "ice.jpg", 40.0f, 30.0f, 1.0f);

	// Load Font
	m_pFtFont->LoadFont("resources\\fonts\\fluffy.ttf", 32);

	// Create a tree
	m_pTree->Create("resources\\textures\\", "TreeTex1.png");
	glEnable(GL_CULL_FACE);

	// Create Snow
	m_pSnow->Create();

	// Create a Cube Tree
	m_pCubeTree->Create("resources\\textures\\", "TreeTex2.png");

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

	// Create the road
	std::vector<glm::vec3> controlPoints;
	controlPoints.push_back(glm::vec3(1000, 10, 0));
	controlPoints.push_back(glm::vec3(-1000, 100, 0));
	controlPoints.push_back(glm::vec3(-1500, 100, 500));
	controlPoints.push_back(glm::vec3(-1000, 10, 1000));
	controlPoints.push_back(glm::vec3(-500, 50, 1000));
	controlPoints.push_back(glm::vec3(0, 50, 1500));
	controlPoints.push_back(glm::vec3(0, 10, 2000));
	controlPoints.push_back(glm::vec3(500, 10, 2500));
	controlPoints.push_back(glm::vec3(1500, 10, 500));

	m_pCatmullRom->CreateCentreline(controlPoints);
	m_pCatmullRom->CreateOffsetCurves(40);
	m_pCatmullRom->CreateTrack("resources\\textures\\", "road1.jpg");

	//Create the edge for the road
	auto centreLinePoints = m_pCatmullRom->m_centrelinePoints;
	auto rightOffsetPoints = m_pCatmullRom->m_rightOffsetPoints;
	auto leftOffsetPoints = m_pCatmullRom->m_leftOffsetPoints;
	for (int i = 0; i < rightOffsetPoints.size(); i++)
	{
		rightOffsetPoints[i] += ((centreLinePoints[i] - rightOffsetPoints[i]) * 2.05f);
		leftOffsetPoints[i] += ((centreLinePoints[i] - leftOffsetPoints[i]) * 2.05f);


		//Set the tree positions
		if (i < 50 || (i > 250 && i < 300))
		{
			if (i % 2 == 0)
			{
				m_tree_positions.push_back(rightOffsetPoints[i] + (centreLinePoints[i] - rightOffsetPoints[i]) * 3.f);
			}
			else
			{
				m_tree_positions.push_back(leftOffsetPoints[i] + (centreLinePoints[i] - leftOffsetPoints[i]) * 3.f);
			}
		}

		//Set the barricade positions
		if (i % 50 == 0)
		{
			if (i % 150 == 0)
			{
				m_barricade_positions.push_back(centreLinePoints[i]);
				m_collidables.push_back(centreLinePoints[i]);
			}
			else if (i % 100 == 0)
			{
				m_barricade_positions.push_back(rightOffsetPoints[i] + glm::normalize(centreLinePoints[i] - rightOffsetPoints[i]) * 4.f);
				m_collidables.push_back(rightOffsetPoints[i] + glm::normalize(centreLinePoints[i] - rightOffsetPoints[i]) * 4.f);
			}
			else
			{
				m_barricade_positions.push_back(leftOffsetPoints[i] + glm::normalize(centreLinePoints[i] - leftOffsetPoints[i]) * 4.f);
				m_collidables.push_back(leftOffsetPoints[i] + glm::normalize(centreLinePoints[i] - leftOffsetPoints[i]) * 4.f);
			}
		}

		//Set the streetlight positions
		if (i % 100 == 0)
		{
			m_streetlight_positions.push_back(rightOffsetPoints[i]);
		}
	}
	m_barricade_positions[0].z = -10;
	m_barricade_positions[0].x += 30;
	m_collidables[0] = m_barricade_positions[0];
	m_collidables[0].z = -10;
	m_pCatmullRomLeft->CreateCentreline(rightOffsetPoints);
	m_pCatmullRomLeft->CreateOffsetCurves(2);
	m_pCatmullRomLeft->CreateTrack("resources\\textures\\", "yellow.jpg");
	m_pCatmullRomRight->CreateCentreline(leftOffsetPoints);
	m_pCatmullRomRight->CreateOffsetCurves(2);
	m_pCatmullRomRight->CreateTrack("resources\\textures\\", "yellow.jpg");

	m_pPlaneFBO->Create(width, height);
}

void Game::LoadShaders()
{
	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("treeShader.vert");
	sShaderFileNames.push_back("treeShader.frag");
	sShaderFileNames.push_back("carShader.vert");
	sShaderFileNames.push_back("carShader.geom");
	sShaderFileNames.push_back("carShader.frag");

	for (int i = 0; i < (int)sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int)sShaderFileNames[i].size() - 4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\" + sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram* pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram* pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create the tree shader program
	CShaderProgram* pTreeProgram = new CShaderProgram;
	pTreeProgram->CreateProgram();
	pTreeProgram->AddShaderToProgram(&shShaders[4]);
	pTreeProgram->AddShaderToProgram(&shShaders[5]);
	pTreeProgram->LinkProgram();
	m_pShaderPrograms->push_back(pTreeProgram);

	// Create the car shader program
	CShaderProgram* pCarProgram = new CShaderProgram;
	pCarProgram->CreateProgram();
	pCarProgram->AddShaderToProgram(&shShaders[6]);
	pCarProgram->AddShaderToProgram(&shShaders[7]);
	pCarProgram->AddShaderToProgram(&shShaders[8]);
	pCarProgram->LinkProgram();
	m_pShaderPrograms->push_back(pCarProgram);


	pCarProgram->SetUniform("bExplodeObject", false);
	pCarProgram->SetUniform("explodeFactor", 0);

	m_pFtFont->SetShaderProgram(pFontProgram);
}

void Game::RestartGame()
{
	lap1 = 0;
	lap2 = 0;
	lap3 = 0;
	m_currentDistance = 0;
	m_lives = 3;
	m_resetCar = true;
	m_explodeFactor = 0;
	m_playerSpeed = 0;
	m_health = 20;
	m_gameOver = false;
}

void Game::Revive()
{
	m_resetCar = true;
	m_explodeFactor = 0;
	m_playerSpeed = 0;
	m_health = 20;
	m_gameOver = false;
	moveDist = 0;
}


// Render method runs repeatedly in a loop
void Game::Render(int pass)
{
	CCamera* currCamera;
	if (pass == 1)
		currCamera = m_pTVCamera;
	else
		currCamera = m_pCamera;
	

	// Clear the buffers and enable depth testing (z-buffering)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	//Light properties for Light and Dark Modes
	float la, ld, ls, ma;
	if (m_gameMode == Light || pass == 1)
	{
		la = 0.5f;
		ld = 0.9f;
		ls = 0.6f;
		ma = 1;
	}
	else
	{
		la = 0.5f;
		ld = 0.3f;
		ls = 0.3f;
		ma = 0.5f;
	}

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(currCamera->GetPosition(), currCamera->GetView(), currCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = currCamera->ComputeNormalMatrix(viewMatrix);

	// Use the main shader program 
	CShaderProgram* pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	if (m_gameMode == Dark)
	{
		pMainProgram->SetUniform("bUseStreetlight", true);
		if (!m_gameOver)
			pMainProgram->SetUniform("bUseSpotlight", true);
	}
	else if (m_gameMode == Light)
	{
		pMainProgram->SetUniform("bUseStreetlight", false);
		pMainProgram->SetUniform("bUseSpotlight", false);
	}
	if(m_gameOver)
		pMainProgram->SetUniform("bUseSpotlight", false);
	pMainProgram->SetUniform("bUsePhongModel", true);
	pMainProgram->SetUniform("bExplodeObject", false);

	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", currCamera->GetPerspectiveProjectionMatrix());

	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(100, 30, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(la));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(ld));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(ls));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(ma));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	pMainProgram->SetUniform("spotlight1.position", viewMatrix * glm::vec4(m_playerPos - m_playerT - m_playerN * 2.f, 1));
	pMainProgram->SetUniform("spotlight1.La", glm::vec3(0.0f, 0.0f, 0.50f));
	pMainProgram->SetUniform("spotlight1.Ld", glm::vec3(0.0f, 0.0f, 0.1f));
	pMainProgram->SetUniform("spotlight1.Ls", glm::vec3(0.0f, 0.0f, 1.90f));
	pMainProgram->SetUniform("spotlight1.direction", glm::normalize(viewNormalMatrix * glm::normalize(m_playerT)));
	pMainProgram->SetUniform("spotlight1.exponent", 0.0f);
	pMainProgram->SetUniform("spotlight1.cutoff", 10.0f);

	pMainProgram->SetUniform("spotlight2.position", viewMatrix * glm::vec4(m_playerPos - m_playerT + m_playerN * 2.f, 1));
	pMainProgram->SetUniform("spotlight2.La", glm::vec3(0.0f, 0.0f, 0.50f));
	pMainProgram->SetUniform("spotlight2.Ld", glm::vec3(0.0f, 0.0f, 0.1f));
	pMainProgram->SetUniform("spotlight2.Ls", glm::vec3(0.0f, 0.0f, 1.90f));
	pMainProgram->SetUniform("spotlight2.direction", glm::normalize(viewNormalMatrix * glm::normalize(m_playerT)));
	pMainProgram->SetUniform("spotlight2.exponent", 0.0f);
	pMainProgram->SetUniform("spotlight2.cutoff", 10.0f);

	for (int i = 0; i < 4; i++)
	{
		string spotlightVar = "spotlights[" + std::to_string(i) + "]";
		if (i < 2)
		{
			float multiplier = (i == 1) ? -1 : 1;
			pMainProgram->SetUniform(spotlightVar + ".position", viewMatrix * glm::vec4(m_car1Pos - m_car1T + m_car1N * 2.f * multiplier, 1));
			pMainProgram->SetUniform(spotlightVar + ".direction", glm::normalize(viewNormalMatrix * glm::normalize(m_car1T)));
		}
		else 
		{
			float multiplier = (i == 3) ? -1 : 1;
			pMainProgram->SetUniform(spotlightVar + ".position", viewMatrix * glm::vec4(m_car2Pos - m_car2T + m_car2N * 2.f * multiplier, 1));
			pMainProgram->SetUniform(spotlightVar + ".direction", glm::normalize(viewNormalMatrix * glm::normalize(m_car2T)));
		}
		pMainProgram->SetUniform(spotlightVar+".La", glm::vec3(0.0f, 0.0f, 0.50f));
		pMainProgram->SetUniform(spotlightVar+".Ld", glm::vec3(0.0f, 0.0f, 0.01f));
		pMainProgram->SetUniform(spotlightVar+".Ls", glm::vec3(0.0f, 0.0f, 0.90f));
		pMainProgram->SetUniform(spotlightVar+".exponent", 0.01f);
		pMainProgram->SetUniform(spotlightVar+".cutoff", 5.0f);
	}

	pMainProgram->SetUniform("spotmaterial1.Ma", glm::vec3(0.0f, 0.0f, 0.05f));
	pMainProgram->SetUniform("spotmaterial1.Md", glm::vec3(0.0f, 0.0f, 0.0001f));
	pMainProgram->SetUniform("spotmaterial1.Ms", glm::vec3(0.0f, 0.0f, 0.1f));
	pMainProgram->SetUniform("spotmaterial1.shininess", 0.0f);

	pMainProgram->SetUniform("streetmaterial1.Ma", glm::vec3(0.001f));	
	pMainProgram->SetUniform("streetmaterial1.Md", glm::vec3(0.25f, 0.25f, 0));
	pMainProgram->SetUniform("streetmaterial1.Ms", glm::vec3(0.25f, 0.25f, 0.0f));
	pMainProgram->SetUniform("streetmaterial1.shininess", 0.1f);	


	vector<float> streetlight_x = {
		0,0,0,10, -10
	};
	vector<float> streetlight_z = {
		10, 10, -10, 0, 0
	};
	for (int i = 0; i < m_streetlight_positions.size(); i++)
	{
		glm::vec3 pos = m_streetlight_positions[i];
		pos.x -= streetlight_x[i];
		pos.y -= -30;
		pos.z -= streetlight_z[i];
		string streetlightVar = "streetlights[" + std::to_string(i) + "]";
		pMainProgram->SetUniform(streetlightVar + ".position", viewMatrix * glm::vec4(pos, 1));
		pMainProgram->SetUniform(streetlightVar + ".La", glm::vec3(1.0f, 1.0f, 0.0f));
		pMainProgram->SetUniform(streetlightVar + ".Ld", glm::vec3(1.0f, 1.0f, 0.0f));
		pMainProgram->SetUniform(streetlightVar + ".Ls", glm::vec3(1.0f, 1.0f, 0.0f));
		pMainProgram->SetUniform(streetlightVar + ".direction", glm::normalize(viewNormalMatrix * glm::vec3(0,-1,0)));
		pMainProgram->SetUniform(streetlightVar + ".exponent", 2.0f);
		pMainProgram->SetUniform(streetlightVar + ".cutoff", 250.5f);
	}

	if (pass == 0) {
		// Render the plane for the TV
		// Back face actually places the horse the right way round
		glDisable(GL_CULL_FACE);
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 100.0f, 0.0f));
		modelViewMatrixStack.RotateRadians(glm::vec3(1, 0, 0), glm::radians(90.0f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), 180.0);
		modelViewMatrixStack.Scale(-1.0);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlaneFBO->BindTexture(0);
		m_pPlane->Render(false);
		modelViewMatrixStack.Pop();
		glEnable(GL_CULL_FACE);
	}

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	glm::vec3 vEye = currCamera->GetPosition();
	modelViewMatrixStack.Translate(vEye);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render(cubeMapTextureUnit);
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	//Render the HeightMap
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pHeightmapTerrain->Render();
	modelViewMatrixStack.Pop();

	// Render the Track
	m_pCatmullRom->RenderTrack();
	m_pCatmullRomLeft->RenderTrack();
	m_pCatmullRomRight->RenderTrack();

	// Render the Tunnel 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(831, 0, 2000));
	modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 0), glm::radians(-29.0f));
	modelViewMatrixStack.Scale(7.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pTunnelMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the Icebergs
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-300, 0, 500));
	modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 1), glm::radians(-29.0f));
	modelViewMatrixStack.Scale(50.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-250, 30, 1500));
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 1, 0), glm::radians(-75.0f));
	modelViewMatrixStack.Scale(70.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(1050, 30, 200));
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 1, 0), glm::radians(-75.0f));
	modelViewMatrixStack.Scale(70.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-750, 30, -700));
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 1, 0), glm::radians(-75.0f));
	modelViewMatrixStack.Scale(70.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-1250, 50, -300));
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 1, 0), glm::radians(-15.0f));
	modelViewMatrixStack.Scale(70.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-1350, 50, 500));
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 1, 0), glm::radians(-1845.0f));
	modelViewMatrixStack.Scale(70.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(350, 50, 300));
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 1, 0), glm::radians(-185.0f));
	modelViewMatrixStack.Scale(70.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(550, 50, 1900));
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 1, 0), glm::radians(-145.0f));
	modelViewMatrixStack.Scale(70.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(1550, -10, 1200));
	modelViewMatrixStack.Scale(10.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceBergMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-550, -10, 200));
	modelViewMatrixStack.Scale(7.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceBergMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-2050, -10, 200));
	modelViewMatrixStack.Scale(7.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pIceBergMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the Sign Board
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(550, 0, 2500));
	modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 0), glm::radians(90.0f));
	modelViewMatrixStack.Scale(10.5f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSignMesh->Render();
	modelViewMatrixStack.Pop();

	vector<float> streetlight_rotations = {
		0, 0, 180, 90, -90
	};
	// Render the Street Lights
	for (int i = 0; i < m_streetlight_positions.size(); i++)
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_streetlight_positions[i]);
		modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 0), glm::radians(90.0f));
		modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 0), glm::radians(streetlight_rotations[i]));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pStreetLightMesh->Render();
		modelViewMatrixStack.Pop();
	}

	// Render the Snowman
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-1500,0,1000));
	modelViewMatrixStack.Scale(3000.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSnowmanMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-100, 0, -300));
	modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 0), glm::radians(-150.0));
	modelViewMatrixStack.Scale(5000.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSnowmanMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(1000, 0, 1400));
	modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 0), glm::radians(-150.0));
	modelViewMatrixStack.Scale(1000.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSnowmanMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the Barricade
	vector<float> barricade_rotations = {
		0, 0, 0, 90, 0, 130, 70, 90, 90, 90
	};
	for (int i = 0; i < m_barricade_positions.size(); i++)
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_barricade_positions[i]);
		modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 0), glm::radians(-90.0));
		modelViewMatrixStack.RotateRadians(glm::vec3(0, 1, 0), glm::radians(barricade_rotations[i]));
		modelViewMatrixStack.Scale(0.04);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarricadeMesh->Render();
		modelViewMatrixStack.Pop();
	}

	// Render the Car

	// Use the Car Program
	CShaderProgram* pCarProgram = (*m_pShaderPrograms)[3];
	pCarProgram->UseProgram();

	pCarProgram->SetUniform("sampler0", 0);

	pCarProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pCarProgram->SetUniform("light1.La", glm::vec3(la));		// Ambient colour of light
	pCarProgram->SetUniform("light1.Ld", glm::vec3(ld));		// Diffuse colour of light
	pCarProgram->SetUniform("light1.Ls", glm::vec3(ls));		// Specular colour of light
	pCarProgram->SetUniform("material1.Ma", glm::vec3(ma));	// Ambient material reflectance
	pCarProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pCarProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pCarProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	// Set the projection matrix
	pCarProgram->SetUniform("matrices.projMatrix", currCamera->GetPerspectiveProjectionMatrix());

	if (m_explodeFactor <= 3.5 || m_resetCar)
	{
		if (m_gameOver && m_health <= 0)
		{
			if (m_explodeFactor == 0 && m_lives > 0)
				m_lives -= 1;

			pCarProgram->SetUniform("bExplodeObject", true);
			pCarProgram->SetUniform("bJoinObject", false); 
			pCarProgram->SetUniform("explodeFactor", m_explodeFactor);
			m_explodeFactor += 0.09f;
			m_resetCar = false;
		}
		if (m_resetCar && m_explodeFactor >= 0)
		{
			pCarProgram->SetUniform("bExplodeObject", false); 
			pCarProgram->SetUniform("bJoinObject", true);
			pCarProgram->SetUniform("explodeFactor", m_explodeFactor);
			m_explodeFactor -= 0.09f;
		}
		if (m_explodeFactor == 0)
			m_resetCar = false;
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_playerPos);
		modelViewMatrixStack *= m_playerAngle;
		modelViewMatrixStack.RotateRadians(glm::vec3(1, 0, 0), glm::radians(-90.0f));
		modelViewMatrixStack.RotateRadians(glm::vec3(0, 0, 1), glm::radians(90.0f));
		modelViewMatrixStack.RotateRadians(glm::vec3(0, 0, 1), glm::radians(m_rotateAngle));
		modelViewMatrixStack.Scale(3.5f - m_explodeFactor);
		pCarProgram->SetUniform("matrices.projMatrix", currCamera->GetPerspectiveProjectionMatrix());
		pCarProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pCarProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCarMesh->Render();
		modelViewMatrixStack.Pop();
	}

	pCarProgram->SetUniform("bExplodeObject", false);
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(m_car1Pos);
	modelViewMatrixStack *= m_car1Angle;
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 0, 0), glm::radians(-90.0f));
	modelViewMatrixStack.RotateRadians(glm::vec3(0, 0, 1), glm::radians(90.0f));
	modelViewMatrixStack.Scale(3.5f);
	pCarProgram->SetUniform("matrices.projMatrix", currCamera->GetPerspectiveProjectionMatrix());
	pCarProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pCarProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCarMesh1->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(m_car2Pos);
	modelViewMatrixStack *= m_car2Angle;
	modelViewMatrixStack.RotateRadians(glm::vec3(1, 0, 0), glm::radians(-90.0f));
	modelViewMatrixStack.RotateRadians(glm::vec3(0, 0, 1), glm::radians(90.0f));
	modelViewMatrixStack.Scale(3.5f);
	pCarProgram->SetUniform("matrices.projMatrix", currCamera->GetPerspectiveProjectionMatrix());
	pCarProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pCarProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCarMesh2->Render();
	modelViewMatrixStack.Pop();

	if (pass == 0)
	{
		// Render the trees

		pMainProgram->UseProgram();

		pMainProgram->SetUniform("bUsePhongModel", false);
		pMainProgram->SetUniform("bExplodeObject", false);

		for (int i = 0; i < m_tree_positions.size(); i++)
		{
			modelViewMatrixStack.Push();
			m_tree_positions[i].y = 0;
			modelViewMatrixStack.Translate(m_tree_positions[i]);
			modelViewMatrixStack.Scale(2);
			pMainProgram->SetUniform("matrices.projMatrix", currCamera->GetPerspectiveProjectionMatrix());
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", currCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			if (i < 50)
			{
				m_pTree->Render();
			}
			else
			{
				m_pCubeTree->Render();
			}
			modelViewMatrixStack.Pop();
		}
	}

	if (pass == 0)
	{
		// Draw the 2D graphics after the 3D graphics
		DisplayFrameRate();
		DisplayLaps();
		DisplayHealthAndLapTimes();
		DisplayControls();
		RenderSpeedTexture();
		if (m_gameOver)
		{
			int lap = m_pCatmullRom->CurrentLap(m_currentDistance);
			if (lap >= 3)
			{
				DisplayGameOverText();
			}
			else
			{
				DisplayDeathText();
			}
		}
	}

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());
}

void Game::RenderSpeedTexture()
{

	m_pSpeedometerImage->Bind();

	glBegin(GL_QUADS);
	glTexCoord2f(10.0f, 10.0f); glVertex3f(-10.5f, -10.5f, 0.0f);
	glTexCoord2f(11.0f, 10.0f); glVertex3f(10.5f, -10.5f, 0.0f);
	glTexCoord2f(11.0f, 11.0f); glVertex3f(10.5f, 10.5f, 0.0f);
	glTexCoord2f(10.0f, 11.0f); glVertex3f(-10.5f, 10.5f, 0.0f);
	glEnd();
}

// Update method runs repeatedly with the Render method
void Game::Update()
{
	if (m_collidables.size() > m_barricade_positions.size())
	{
		int index = m_barricade_positions.size();
		m_collidables[index] = m_car1Pos;
		m_collidables[index + 1] = m_car2Pos;
	}
	else
	{
		m_collidables.push_back(m_car1Pos);
		m_collidables.push_back(m_car2Pos);
	}

	if (!m_gameOver)
	{
		if (m_increaseSpeed)
		{
			if (m_playerSpeed < 0.5)
				m_playerSpeed += 0.001f;
		}
		else if (m_decreaseSpeed)
		{
			if (m_playerSpeed > 0)
				m_playerSpeed -= 0.005f;
			else
				m_playerSpeed = 0;
		}

		// Camera and Player to Follow Path
		m_currentDistance += m_dt * m_playerSpeed;
		glm::vec3 y = glm::vec3(0, 1, 0);
		glm::vec3 pNext;
		m_pCatmullRom->Sample(m_currentDistance + 1, pNext);
		glm::vec3 p;
		m_pCatmullRom->Sample(m_currentDistance, p);

		m_playerT = glm::normalize(pNext - p);
		m_playerN = glm::normalize(glm::cross(m_playerT, y));
		m_playerB = glm::normalize(glm::cross(m_playerN, m_playerT));

		p.y += 5;
		float dist = 0.7f;
		switch (m_movePlayer)
		{
		case Left:
			if (moveDist >= -17.4)
			{
				moveDist += -dist;
			}
			m_movePlayer = None;
			break;
		case Right:
			if (moveDist <= 17.4)
			{
				moveDist += dist;
			}
			m_movePlayer = None;
			break;
		}
		m_playerPos = p - glm::vec3(0, 3, 0) + m_playerN * moveDist;

		glm::vec3 camPos;
		glm::vec3 camViewPos;
		switch (m_cameraType)
		{
		case First:
			camPos = m_playerPos + (m_playerT * 10.0f) + glm::vec3(0, 5, 0);
			camViewPos = m_playerPos + (m_playerT * 50.0f);
			m_pCamera->Set(camPos, camViewPos, y);
			break;
		case Third:
			camPos = m_playerPos - (m_playerT * 50.0f) + glm::vec3(0, 20, 0);
			camViewPos = m_playerPos + (m_playerT * 30.0f);
			m_pCamera->Set(camPos, camViewPos, y);
			break;
		case Top:
			camPos = m_playerPos + y * 100.f + m_playerT * 30.f;
			camViewPos = m_playerPos + m_playerT * 30.f;
			m_pCamera->Set(camPos, camViewPos, m_playerT);
			break;
		case FreeLook:
			m_pCamera->Update(m_dt);
			break;
		}
		camPos = m_playerPos - (m_playerT * 50.0f) + glm::vec3(0, 20, 0);
		camViewPos = m_playerPos + (m_playerT * 30.0f);
		m_pTVCamera->Set(camPos, camViewPos, y);

		m_playerAngle = glm::mat4(glm::mat3(m_playerT, m_playerB, m_playerN));

		// Car 1
		m_car1Distance += m_dt * 0.43f;
		m_pCatmullRom->Sample(m_car1Distance + 1, pNext);
		m_pCatmullRom->Sample(m_car1Distance, p);

		m_car1T = glm::normalize(pNext - p);
		m_car1N = glm::normalize(glm::cross(m_car1T, y));
		m_car1B = glm::normalize(glm::cross(m_car1N, m_car1T));
		
		m_car1Pos = p + glm::vec3(0, 2, 0) - m_car1N * 10.f;

		m_car1Angle = glm::mat4(glm::mat3(m_car1T, m_car1B, m_car1N));


		// Car 2
		m_car2Distance += m_dt * 0.38f;
		m_pCatmullRom->Sample(m_car2Distance + 1, pNext);
		m_pCatmullRom->Sample(m_car2Distance, p);

		m_car2T = glm::normalize(pNext - p);
		m_car2N = glm::normalize(glm::cross(m_car2T, y));
		m_car2B = glm::normalize(glm::cross(m_car2N, m_car2T));

		m_car2Pos = p + glm::vec3(0, 2, 0) + m_car2N * 10.f;

		m_car2Angle = glm::mat4(glm::mat3(m_car2T, m_car2B, m_car2N));


		// Check if player collides with collidables
		for (int i = 0; i < m_collidables.size(); i++)
		{
			float dist = CalculateDistance(m_playerPos, m_collidables[i]);
			if (dist < 4.2)
			{
				m_playerSpeed = 0.1;
				m_health -= 20;
				m_gameOver = true;
				m_currentDistance += 20;
			}
		}

		//GameOver Conditions
		if (m_pCatmullRom->CurrentLap(m_currentDistance) == 3 || m_health <= 0)
		{
			m_playerSpeed = 0;
			m_gameOver = true;
		}
	}

	// Play Audio
	m_pAudio->Update();
}

void Game::DisplayFrameRate()
{
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_totalTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
	{
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
	}

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

void Game::DisplayLaps()
{
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Use the font shader program and render the text
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	int noLaps = m_pCatmullRom->CurrentLap(m_currentDistance) + 1;
	if (noLaps > 3) {
		noLaps = 3;
	}
	m_pFtFont->Render(width / 2 - 100, height - 50, 40, "Laps: %d/3", noLaps);
}

void Game::DisplayHealthAndLapTimes()
{
	int currLap = m_pCatmullRom->CurrentLap(m_currentDistance);
	if (currLap == 0)
		lap1 += m_dt;
	else if (currLap == 1)
		lap2 += m_dt;
	else if (currLap == 2)
		lap3 += m_dt;

	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	Time time1;
	long totalSeconds = lap1 / 1000;
	time1.minutes = totalSeconds / 60;
	time1.seconds = totalSeconds % 60;

	Time time2;
	totalSeconds = lap2 / 1000;
	time2.minutes = totalSeconds / 60;
	time2.seconds = totalSeconds % 60;

	Time time3;
	totalSeconds = lap3 / 1000;
	time3.minutes = totalSeconds / 60;
	time3.seconds = totalSeconds % 60;

	auto text = "Lives left : " + std::to_string(m_lives) + " \n Lap 1 : " + std::to_string(time1.minutes) + ":" + std::to_string(time1.seconds) + 
		"\n Lap 2 : " + std::to_string(time2.minutes) + ":" + std::to_string(time2.seconds) + 
		"\n Lap 3 : " + std::to_string(time3.minutes) + ":" + std::to_string(time3.seconds);

	// Use the font shader program and render the text
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//m_pFtFont->Render(width - 175, height - 20, 20, "Car Health: %d%% \n Lap 1 : %a%% \n Lap 2 : %a%%\n Lap 3 : %a%%", m_health, lap1, lap2, lap3);
	m_pFtFont->Render(width - 175, height - 20, 20, text.c_str());
}

void Game::DisplayDeathText()
{
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Use the font shader program and render the text
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	string m_deathText;

	if (m_lives <= 0)
	{
		m_deathText = "Press R to restart the game";
		m_pFtFont->Render(width / 2 - 300, height - height / 2, 40, m_deathText.c_str());
	}
	else
	{
		m_deathText = "Press R to respawn";
		m_pFtFont->Render(width / 2 - 200, height - height / 2, 40, m_deathText.c_str());
	}
}

void Game::DisplayGameOverText()
{
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Use the font shader program and render the text
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	string gameText = "GAME OVER";
	m_pFtFont->Render(width / 2 - 120, height - height / 2, 40, gameText.c_str());

	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	float bestLap = 0;
	if (lap1 >= lap2 && lap2 >= lap3)
		bestLap = lap3;
	else if (lap2 >= lap1 && lap3 >= lap1)
		bestLap = lap2;
	else
		bestLap = lap1;

	Time time1;
	long totalSeconds = bestLap / 1000;
	time1.minutes = totalSeconds / 60;
	time1.seconds = totalSeconds % 60;

	gameText = "Best Lap : " + std::to_string(time1.minutes) + " min " + std::to_string(time1.seconds) + " seconds";
	m_pFtFont->Render(width / 2 - 140, height - height / 2 - 30, 20, gameText.c_str());

	gameText = "Press R to restart";
	m_pFtFont->Render(width / 2 - 100, height - height / 2 - 170, 20, gameText.c_str());
}

void Game::DisplayControls()
{
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Use the font shader program and render the text
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 0.7f));

	string gameText = "Controls";
	m_pFtFont->Render(100, 180, 20, gameText.c_str());

	gameText = "W A S D - Movement \n N - Toggle Night Mode \n C - Switch Camera \n F - Toggle Freelook";
	m_pFtFont->Render(60, 150, 20, gameText.c_str());
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	// Variable timer
	m_pGameLoopTimer->Start();

	m_pPlaneFBO->Bind();
	Render(1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Update();
	Render(0);


	m_dt = m_pGameLoopTimer->Elapsed();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());
}

float Game::CalculateDistance(glm::vec3 p1, glm::vec3 p2)
{
	float dist = sqrt(pow(p1.x - p2.x, 2) + pow(p1.z - p2.z, 2));
	return dist;
}

WPARAM Game::Execute()
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if (!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();
	constexpr float requiredFPS = 60;
	constexpr double desiredFrameTime = 1.0 / requiredFPS;

	MSG msg;

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (m_appActive) {
			GameLoop();
		}
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch (LOWORD(w_param))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			m_appActive = true;
			m_pHighResolutionTimer->Start();
			break;
		case WA_INACTIVE:
			m_appActive = false;
			break;
		}
		break;
	}

	case WM_SIZE:
		RECT dimensions;
		GetClientRect(window, &dimensions);
		m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch (w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case 'A':
			if (m_cameraType != FreeLook) 
			{
				m_movePlayer = Left;
				m_rotateAngle = 5;
			}
			break;
		case 'D':
			if (m_cameraType != FreeLook)
			{
				m_movePlayer = Right;
				m_rotateAngle = -5;
			}
			break;
		case 'W':
			if (m_cameraType != FreeLook)
				m_increaseSpeed = true;
			break;
		case 'S':
			if (m_cameraType != FreeLook)
				m_decreaseSpeed = true;
			break;
		case 'C':
			if (m_cameraType == First)
			{
				m_cameraType = Third;
			}
			else if (m_cameraType == Third)
			{
				m_cameraType = Top;
			}
			else if (m_cameraType == Top)
			{
				m_cameraType = First;
			}
			break;
		case 'F':
			if (m_cameraType == FreeLook)
				m_cameraType = Third;
			else
			{
				m_cameraType = FreeLook;
			}
			break;
		case 'N':
			if (m_gameMode == Light)
				m_gameMode = Dark;
			else
				m_gameMode = Light;
			break;
		case 'R':
			if (m_gameOver)
			{
				if (m_lives <= 0 || m_pCatmullRom->CurrentLap(m_currentDistance) >= 3)
					RestartGame();
				else
					Revive();
			}
			break;
		}
		break;

	case WM_KEYUP:
		switch (w_param) {
		case 'W':
			m_increaseSpeed = false;
			break;
		case 'S':
			m_decreaseSpeed = false;
			break;
		case 'A':
			m_rotateAngle = 0;
			break;
		case 'D':
			m_rotateAngle = 0;
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance()
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance)
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int)
{
	Game& game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return int(game.Execute());
}
