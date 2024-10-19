#pragma once

#include "Common.h"
#include "GameWindow.h"
#include "Tree.h"
#include "CatmullRom.h"
#include "CubeTree.h"
#include "HeightMapTerrain.h"
#include "FrameBufferObject.h"
#include "Snow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCircularSpline;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render(int pass);
	void UseSnowShader(glm::mat4 viewMatrix);
	void RenderSpeedTexture();
	void LoadShaders();
	void RestartGame();
	void Revive();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	CCamera* m_pTVCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh* m_pCarMesh;
	COpenAssetImportMesh* m_pCarMesh1;
	COpenAssetImportMesh* m_pCarMesh2;
	COpenAssetImportMesh* m_pTunnelMesh;
	COpenAssetImportMesh* m_pSignMesh;
	COpenAssetImportMesh* m_pBarricadeMesh;
	COpenAssetImportMesh* m_pIceMesh;
	COpenAssetImportMesh* m_pStreetLightMesh;
	COpenAssetImportMesh* m_pIceBergMesh;
	COpenAssetImportMesh* m_pSnowmanMesh;
	CTree *m_pTree;
	CSnow* m_pSnow;
	CCubeTree* m_pCubeTree;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CHighResolutionTimer* m_pGameLoopTimer;
	CAudio *m_pAudio;
	CHeightMapTerrain* m_pHeightmapTerrain;
	CCatmullRom *m_pCatmullRom;
	CCatmullRom* m_pCatmullRomLeft;
	CCatmullRom* m_pCatmullRomRight;
	CPlane* m_pPlane;
	CFrameBufferObject* m_pPlaneFBO;
	CTexture* m_pSpeedometerImage;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	bool m_gameOver;
	float m_currentDistance;
	float m_car1Distance;
	float m_car2Distance;
	float m_collisionDistance;
	int m_health;
	bool m_resetCar;
	bool m_increaseSpeed;
	bool m_decreaseSpeed;
	glm::vec3 m_playerPos;
	glm::vec3 m_car1Pos;
	glm::vec3 m_car2Pos;
	glm::mat4 m_playerAngle;
	glm::mat4 m_car1Angle;
	glm::mat4 m_car2Angle;
	float m_rotateAngle = 0;
	enum CameraType {First, Third, Top, FreeLook};
	enum PlayerMove {None, Left, Right, Front, Back};
	enum GameMode {Light, Dark};
	CameraType m_cameraType;
	PlayerMove m_movePlayer;
	GameMode m_gameMode;
	float moveDist = 0;
	std::vector<glm::vec3> m_tree_positions;
	std::vector<glm::vec3> m_barricade_positions;
	std::vector<glm::vec3> m_streetlight_positions;
	std::vector<glm::vec3> m_collidables;
	glm::vec3 m_playerT;
	glm::vec3 m_playerN;
	glm::vec3 m_playerB;
	glm::vec3 m_car1T;
	glm::vec3 m_car1N;
	glm::vec3 m_car1B;
	glm::vec3 m_car2T;
	glm::vec3 m_car2N;
	glm::vec3 m_car2B;

	const int MINIMAP_WIDTH = 200;
	const int MINIMAP_HEIGHT = 200;
	float width;
	float height;

	float lap1 = 0;
	float lap2 = 0;
	float lap3 = 0;
	int m_lives;

	struct Time {
		int minutes;
		int seconds;
	};

public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void DisplayLaps();
	void DisplayHealthAndLapTimes();
	void DisplayDeathText();
	void DisplayGameOverText();
	void DisplayControls();
	void GameLoop();
	float CalculateDistance(glm::vec3, glm::vec3);
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
	double m_totalTime;
	int m_cameraSpeed;
	float m_playerSpeed;
	int m_cameraRadius;
	float m_explodeFactor;
	float m_shaderElapsedTime;
};
