#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>			// C++ cin, cout, etc.
//#include "linmath.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <random>
#include <time.h>

#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>		
#include <string>

#include <vector>	
#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "cShader.h"
#include "cShaderManager.h" 

#include "cGameObject.h"
#include "cVAOMeshManager.h"

#include "Physics.h"	// Physics collision detection functions
#include "cLightManager.h"
#include "Keyboard.h"
#include "DebugRenderer.h"

#include"CommandManager.h"
#include "OrientToTime.h"

#include "CTextureManager.h"
#include <iPhysicsFactory.h>
#include <eBodyType.h>
#include <enums.h>

//nPhysics::iPhysicsWorld* theBulletWorld;
//nPhysics::iPhysicsFactory* theBulletFactory;
nPhysics::iPhysicsWorld* theCustomWorld;
nPhysics::iPhysicsFactory* theCustomFactory;
bool isBullet = true;

void DrawObject(cGameObject* pTheGO,GLint shaderId);
void PhysicsStep(double& deltaTime);
void ResolveCollisions(std::vector<cGameObject*> theGameObjects);
void CalculateReflection(cGameObject* theObject);
bool Load3DModelsFromModelFile(int shaderID, cVAOMeshManager* pVAOManager, std::string &error);
bool LoadSceneFromFile(std::string& fileName);
void getUniformLocations(int& mvp_location, int &currentProgID);
bool LoadLightsAttributesFromFile(std::string& fileName, std::vector<cLight>& theLights);
glm::vec3 getTriangleCentroid(cPhysTriangle* theTriangle);

std::string modelAndLightFile = "modelsandscene.txt";

cVAOMeshManager*	g_pVAOManager = 0;		// or NULL, or nullptr
cShaderManager*		g_pShaderManager;		// Heap, new (and delete)
cLightManager*		g_pLightManager;
cGameObject*		g_pTheDebugSphere;
cGameObject*		g_theBomb;
cGameObject*		g_theExplosion;
cMesh*				g_GameTerrain;
DebugRenderer*		g_pTheDebugrender;
CTextureManager*	g_pTextureManager;

bool g_bombExploded = false;

std::vector< cGameObject* >  g_vecGameObjects;
std::vector< cGameObject* >  g_vecGameSpeeders;
std::vector<cMesh*>* g_vecMeshTerrain;


glm::vec3 g_cameraXYZ = glm::vec3(0.0f, 0.0f, 0.0f);	// 5 units "down" z
glm::vec3 g_cameraTarget_XYZ = glm::vec3(-0.0f, 20.8f, 0.2f);

bool g_bDrawDebugLightSpheres = true;

// Other uniforms:
GLint uniLoc_materialDiffuse = -1;
GLint uniLoc_materialAmbient = -1;
GLint uniLoc_ambientToDiffuseRatio = -1; 	// Maybe	// 0.2 or 0.3
GLint uniLoc_materialSpecular = -1;  // rgb = colour of HIGHLIGHT only
GLint uniLoc_bIsDebugWireFrameObject = -1;
GLint uniLoc_eyePosition = -1;	// Camera position
GLint uniLoc_mModel = -1;
GLint uniLoc_mView = -1;
GLint uniLoc_mProjection = -1;

int g_GameObjectIndex = 0;
cGameObject *g_curGameObject;
const float g_sizeOfExplosion = 10.0f;

std::random_device randd; // obtain a random number from hardware
std::mt19937 engine(rand()); // seed the generator
std::uniform_int_distribution<> distribution(1, 40);

double g_lastTimeStep = 0.0;

CommandManager* theParallelCommands;
CommandManager* theSerialCommands;

bool drawDebugInfo = true;
extern int objectIndex;

static void error_callback(int error, const char* description){
	fprintf(stderr, "Error: %s\n", description);
}

int main(void){

	InitPhysics();
	theCustomWorld = theCustomFactory->CreateWorld();
	theCustomWorld->setIntegrationType(nPhysics::IntegrationType::RK4);

	GLFWwindow* window;
	g_vecMeshTerrain = new std::vector<cMesh*>();
	g_curGameObject = new cGameObject();
	//    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location;	// , vpos_location, vcol_location;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	int height = 480;	/* default */
	int width = 640;	// default
	std::string title = "OpenGL Rocks";

	loadConfigFile(width, height, title);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// C++ string
	// C no strings. Sorry. char    char name[7] = "Michael\0";
	window = glfwCreateWindow(width, height,
		title.c_str(),
		NULL, NULL);

	if (!window){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//static key_callback is from Keyboard.h
	glfwSetKeyCallback(window, Keyboard::key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	std::cout << glGetString(GL_VENDOR) << " "
		<< glGetString(GL_RENDERER) << ", "
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	::g_pShaderManager = new cShaderManager();
	cShader vertShader;
	cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";
	::g_pShaderManager->setBasePath("assets//shaders//");

	// Shader objects are passed by reference so that
	//	we can look at the results if we wanted to. 
	if (!::g_pShaderManager->createProgramFromFile(
		"mySexyShader", vertShader, fragShader)){
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;
		//		exit(
	}
	std::cout << "The shaders compiled and linked OK" << std::endl;

	::g_pVAOManager = new cVAOMeshManager();
	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	//Load models 
	std::string error;
	if (!Load3DModelsFromModelFile(sexyShaderID, ::g_pVAOManager, error)){
		std::cout << "Not all models were loaded..." << std::endl;
		std::cout << error << std::endl;
	}
	LoadSceneFromFile(modelAndLightFile);

	//init the debug renderer
	::g_pTheDebugrender = new DebugRenderer();
	if (!::g_pTheDebugrender->initialize(error)){
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
	}

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
	getUniformLocations(mvp_location, currentProgID);

	//load lights
	::g_pLightManager = new cLightManager();
	::g_pLightManager->CreateLights(20);
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);
	LoadLightsAttributesFromFile(modelAndLightFile, ::g_pLightManager->vecLights);

	//do the texture loading
	g_pTextureManager = new CTextureManager();
	g_pTextureManager->setBasePath("assets//textures");

	//TODO: find out why screen is black in one of these functions
	LoadTexturesFromFile(modelAndLightFile);

	::g_pTextureManager->setBasePath("assets/textures/skybox");
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles(
		"space",
		"SpaceBox_right1_posX.bmp",
		"SpaceBox_left2_negX.bmp",
		"SpaceBox_top3_posY.bmp",
		"SpaceBox_bottom4_negY.bmp",
		"SpaceBox_front5_posZ.bmp",
		"SpaceBox_back6_negZ.bmp", true, true))
	{
		std::cout << "Didn't load skybox" << std::endl;
	}

	//map the textures
	MapTexturesToProperObjects();

	glEnable(GL_DEPTH);
	glCullFace(GL_BACK);
	// Gets the "current" time "tick" or "step"
	g_lastTimeStep = glfwGetTime();

	// Main game or application loop
	while (!glfwWindowShouldClose(window)){

		float ratio;
		int width, height;
		//g_cameraTarget_XYZ = g_vecGameObjects[0]->position;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear colour AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		::g_pShaderManager->useShaderProgram("mySexyShader");
		GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

		// Update all the light uniforms...
		// (for the whole scene)
		::g_pLightManager->CopyLightInformationToCurrentShader();

		// Projection and view don't change per scene (maybe)
		glm::mat4x4 p = glm::perspective(0.6f,			// FOV
			ratio,		// Aspect ratio
			0.1f,			// Near (as big as possible)
			1000.0f);	// Far (as small as possible)

		// View or "camera" matrix
		glm::mat4 v = glm::mat4(1.0f);	// identity

		g_cameraTarget_XYZ = g_vecGameObjects[objectIndex]->position;

		v = glm::lookAt(g_cameraXYZ,		// "eye" or "camera" position
			g_cameraTarget_XYZ,				// "At" or "target" 
			glm::vec3(0.0f, 1.0f, 0.0f));	// "up" vector

		glUniformMatrix4fv(uniLoc_mView, 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(v));
		glUniformMatrix4fv(uniLoc_mProjection, 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(p));

		// Draw the scene
		unsigned int sizeOfVector = ::g_vecGameObjects.size();	//*****//
		for (int index = 0; index != sizeOfVector; index++){

			cGameObject* pTheGO = ::g_vecGameObjects[index];

			if (::g_vecGameObjects[index]->theBody)	{
				if(::g_vecGameObjects[index]->typeOfObject == eTypeOfObject::SOFT_BODY){ 
					updateSoftBodyMeshFromPhysics();
				}
				else {
					::g_vecGameObjects[index]->synchronize();
				}
			}

			DrawObject(pTheGO, shaderID);
		}//for ( int index = 0...

		std::stringstream ssTitle;
		ssTitle << "Camera (xyz): "
			<< g_cameraXYZ.x << ", "
			<< g_cameraXYZ.y << ", "
			<< g_cameraXYZ.z;
		glfwSetWindowTitle(window, ssTitle.str().c_str());

		double curTime = glfwGetTime();
		double deltaTime = curTime - g_lastTimeStep;

		//draw the bounding boxes
		::g_pTheDebugrender->RenderDebugObjects(v, p, deltaTime);

		//physics
		theCustomWorld->TimeStep(curTime, (float)deltaTime);

		g_lastTimeStep = curTime;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}// while ( ! glfwWindowShouldClose(window) )

	glfwDestroyWindow(window);
	glfwTerminate();
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;
	delete ::g_pLightManager;
	delete ::g_pTheDebugSphere;
	delete ::g_theBomb;
	delete ::g_theExplosion;
	delete ::g_GameTerrain;
	return 0;
}

// Update the world 1 "step" in time
void PhysicsStep(double& deltaTime)
{
	//gravity is off
	const glm::vec3 GRAVITY = glm::vec3(0.0f, 0.0f, 0.0f);
	// Identical to the 'render' (drawing) loop
	for (int index = 0; index != ::g_vecGameObjects.size(); index++)
	{
		cGameObject* pCurGO = ::g_vecGameObjects[index];

		// Is this object to be updated?
		if (!pCurGO->bIsUpdatedInPhysics)
		{	// DON'T update this
			continue;		// Skip everything else in the for
		}

		// Explicit Euler  (RK4) 

		if (g_vecGameObjects[index]->typeOfObject == eTypeOfObject::SPHERE)
		{
			// New position is based on velocity over time
			glm::vec3 deltaPosition = (float)deltaTime * pCurGO->vel;
			pCurGO->position += deltaPosition;

			// New velocity is based on acceleration over time
			glm::vec3 deltaVelocity = ((float)deltaTime * pCurGO->accel)
				+ ((float)deltaTime * GRAVITY);

			pCurGO->vel -= deltaVelocity;
		}
		

		//if (g_vecGameObjects[index]->typeOfObject == eTypeOfObject::UNKNOWN)
		//{
		//	glm::vec3 closestPtToObject(0.0f);
		//	std::pair<float,AABB_Center_Radius*> closestAABB;
		//	std::list<std::pair<float, AABB_Center_Radius*>> theClosestAABBs;
		//	std::list<std::pair<float, cPhysTriangle*>> theDistancesAndTheirTriangles;
		//	std::vector<glm::vec3> theCollisions;
		//
		//	//test inner AABB's other AABB's
		//	for (int i = 0; i < pCurGO->theBoundingBox->theInnerBoxes.size(); i++)
		//	{
		//
		//		for (int AABBIndex = 2; AABBIndex < g_vecGameObjects.size(); AABBIndex++)
		//		{
		//			if (g_vecGameObjects[AABBIndex]->bHasAABB)
		//			{
		//				float distance = glm::distance(pCurGO->theBoundingBox->theInnerBoxes[i]->center, g_vecGameObjects[AABBIndex]->theBoundingBox->center);
		//
		//				if (distance < g_vecGameObjects[AABBIndex]->theBoundingBox->getAverageRadius())
		//				{
		//					theClosestAABBs.push_back(std::pair<float, AABB_Center_Radius*>(distance, g_vecGameObjects[AABBIndex]->theBoundingBox));
		//				}
		//			}
		//
		//		}
		//
		//		//make sure the vector has something in it
		//		if (theClosestAABBs.size() > 0)
		//		{
		//			theClosestAABBs.sort();
		//			closestAABB = theClosestAABBs.front();
		//
		//			//int TestAABBAABB(AABB_Center_Radius* a, AABB_Center_Radius* b)
		//			int collision = TestAABBAABB(pCurGO->theBoundingBox->theInnerBoxes[i], closestAABB.second);
		//			if (collision > 0)
		//			{
		//				glm::vec3 tempAdditive(0.0f);
		//				if (pCurGO->position.x < pCurGO->previousPosition.x)
		//				{
		//					tempAdditive.x += 0.5f;
		//				}
		//
		//				if (pCurGO->position.x > pCurGO->previousPosition.x)
		//				{
		//					tempAdditive.x -= 0.5f;
		//				}
		//
		//				if (pCurGO->position.y < pCurGO->previousPosition.y)
		//				{
		//					tempAdditive.y += 0.5f;
		//				}
		//
		//				if (pCurGO->position.x > pCurGO->previousPosition.x)
		//				{
		//					tempAdditive.y -= 0.5f;
		//				}
		//
		//				if (pCurGO->position.z < pCurGO->previousPosition.z)
		//				{
		//					tempAdditive.z += 0.5f;
		//				}
		//
		//				if (pCurGO->position.z > pCurGO->previousPosition.z)
		//				{
		//					tempAdditive.z -= 0.5f;
		//				}
		//
		//				pCurGO->position = pCurGO->previousPosition + tempAdditive;
		//				pCurGO->theBoundingBox->drawCollisionBox(pCurGO->theBoundingBox->theInnerBoxes[i]->center, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		//				pCurGO->vel = glm::vec3(0.0f);
		//			}
		//		}
		//	}
		//
		//
		//	//test against terrain mesh
		//	for (int i = 0; i < g_GameTerrain->vecPhysTris.size(); i++)
		//	{
		//		//check each triangle 
		//		cPhysTriangle& curTriangle = g_GameTerrain->vecPhysTris[i];
		//
		//		for (int AABBIndex = 0; AABBIndex < g_vecGameObjects[index]->theBoundingBox->theInnerBoxes.size(); AABBIndex++)
		//		{
		//			
		//			glm::vec3 theClosestPoint = curTriangle.ClosestPtPointTriangle(&g_vecGameObjects[index]->theBoundingBox->theInnerBoxes[AABBIndex]->center);
		//
		//			float distance = glm::distance(theClosestPoint, g_vecGameObjects[index]->theBoundingBox->theInnerBoxes[AABBIndex]->center);
		//
		//			//if the trianlge is closer than the radius of the box away, add it to the list
		//			if (distance < g_vecGameObjects[index]->theBoundingBox->theInnerBoxes[AABBIndex]->radius.x)
		//			{
		//				theDistancesAndTheirTriangles.push_back(std::pair<float, cPhysTriangle*>(distance, &curTriangle));
		//			}
		//
		//			//if there are any triangles within 10 units
		//			if (theDistancesAndTheirTriangles.size() > 0)
		//			{
		//				//iterate through the list and check for collisions
		//				std::pair<float, cPhysTriangle*> closest;
		//				theDistancesAndTheirTriangles.sort();
		//				closest = theDistancesAndTheirTriangles.front();
		//
		//				/*int collision = TestTriangleAABB(closest.second->vertex[0], closest.second->vertex[1], closest.second->vertex[2], *g_vecGameObjects[index]->theBoundingBox->theInnerBoxes[AABBIndex]);
		//				if (collision > 0 )
		//				{*/
		//					//add the collision
		//					closestPtToObject = getTriangleCentroid(closest.second);
		//					//set the object to its previous position when it wasnt colliding
		//					
		//					g_vecGameObjects[index]->position = g_vecGameObjects[index]->previousPosition + closest.second->normals[1];
		//					//g_vecGameObjects[index]->position.y = closestPtToObject.y + 0.1f;
		//					pCurGO->theBoundingBox->drawCollisionBox(closestPtToObject, 0.75f, glm::vec3(1.0f, 0.0f, 0.0f));
		//
		//					//theCollisions.push_back(closestPtToObject);
		//					theDistancesAndTheirTriangles.clear();
		//					pCurGO->vel = glm::vec3(0.0f);
		//				//}
		//			}			
		//		}
		//	}
		//
		//	//set previous position
		//	g_vecGameObjects[index]->previousPosition = g_vecGameObjects[index]->position;
		//}

	}//for ( int index...

	return;
}

// Draw a single object
void DrawObject(cGameObject* pTheGO,GLint curShaderProgramID)
{
	// Is there a game object? 
	if (pTheGO == 0)	//if ( ::g_GameObjects[index] == 0 )
	{	// Nothing to draw
		return;		// Skip all for loop code and go to next
	}

	if (drawDebugInfo){
		////build the bounding boxes 
		//if (pTheGO->bHasAABB)
		//{
		//	cMesh* tempMesh = new cMesh();
		//	g_pVAOManager->lookupMeshFromName(pTheGO->meshName, *tempMesh);
		//	pTheGO->theBoundingBox = new AABB_Center_Radius(tempMesh, pTheGO->position, pTheGO->scale);
		//	pTheGO->theBoundingBox->buildBoundingBox(pTheGO->scale);
		//}
	}	

	std::string meshToDraw = pTheGO->meshName;		//::g_GameObjects[index]->meshName;

	sVAOInfo VAODrawInfo;
	if (::g_pVAOManager->lookupVAOFromName(meshToDraw, VAODrawInfo) == false)
	{	// Didn't find mesh
		return;
	}

	glm::mat4x4 mModel = glm::mat4x4(1.0f);	//		mat4x4_identity(m);

	glm::mat4 matRreRotZ = glm::mat4x4(1.0f);
	matRreRotZ = glm::rotate(matRreRotZ, pTheGO->orientation.z,
		glm::vec3(0.0f, 0.0f, 1.0f));
	mModel = mModel * matRreRotZ;

	glm::mat4 trans = glm::mat4x4(1.0f);
	trans = glm::translate(trans,
		pTheGO->position);
	mModel = mModel * trans;

	glm::mat4 matPostRotZ = glm::mat4x4(1.0f);
	matPostRotZ = glm::rotate(matPostRotZ, pTheGO->orientation2.z,
		glm::vec3(0.0f, 0.0f, 1.0f));
	mModel = mModel * matPostRotZ;


	glm::mat4 matPostRotY = glm::mat4x4(1.0f);
	matPostRotY = glm::rotate(matPostRotY, pTheGO->orientation2.y,
		glm::vec3(0.0f, 1.0f, 0.0f));
	mModel = mModel * matPostRotY;


	glm::mat4 matPostRotX = glm::mat4x4(1.0f);
	matPostRotX = glm::rotate(matPostRotX, pTheGO->orientation2.x,
		glm::vec3(1.0f, 0.0f, 0.0f));
	mModel = mModel * matPostRotX;
	// TODO: add the other rotation matrix (i.e. duplicate code above)
	float finalScale = pTheGO->scale;

	glm::mat4 matScale = glm::mat4x4(1.0f);
	matScale = glm::scale(matScale,
		glm::vec3(finalScale,
			finalScale,
			finalScale));
	mModel = mModel * matScale;


	glUniformMatrix4fv(uniLoc_mModel, 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(mModel));


	glm::mat4 mWorldInTranpose = glm::inverse(glm::transpose(mModel));

	glUniform4f(uniLoc_materialDiffuse,
		pTheGO->diffuseColour.r,
		pTheGO->diffuseColour.g,
		pTheGO->diffuseColour.b,
		pTheGO->diffuseColour.a);

	///////////////////////////////////////

	//Set ALL the samplers to something (so they don't point to GL_TEXTURE0)
	QnDTexureSamplerUtility::LoadUniformLocationsIfNeeded(curShaderProgramID);
	// There's no way we have 999 texture units...
	QnDTexureSamplerUtility::setAllSamplerUnitsToInvalidNumber(999);
	QnDTexureSamplerUtility::clearAllBlendValuesToZero();

	// Now set our samplers, and blend function to something
	// Basic texture binding setup (assign all loaded textures to samplers)
	g_pTextureManager->UpdateTextureBindingsByTextureNameSimple();
	std::map<std::string, CTexUnitInfoBrief> mapTexNameToTexUnit;

	g_pTextureManager->GetBoundTextureUnitsByTextureNames(mapTexNameToTexUnit);

	//// Now look up what textures our object is using and set the samplers
	QnDTexureSamplerUtility::SetSamplersForMeshTextures(*pTheGO, mapTexNameToTexUnit);

	GLint texHeightMap_UniLoc = glGetUniformLocation(curShaderProgramID, "texHeightMap");
	GLint bIsHeightMap_UniLoc = glGetUniformLocation(curShaderProgramID, "bIsHeightMap");

	{
		glUniform1f(bIsHeightMap_UniLoc, GL_FALSE);
	}

	GLint uniLoc_bIsSkyBoxObject = glGetUniformLocation(curShaderProgramID, "isASkyBox");
	if (pTheGO->bIsSkyBoxObject)
	{
		glUniform1f(uniLoc_bIsSkyBoxObject, GL_TRUE);
	}
	else
	{
		glUniform1f(uniLoc_bIsSkyBoxObject, GL_FALSE);
	}

	GLint uniLoc_bIsDebugWireFrameObject = glGetUniformLocation(curShaderProgramID, "bIsDebugWireFrameObject");

	// Wireframe?
	if (pTheGO->bIsWireFrame)
	{
		glUniform1f(uniLoc_bIsDebugWireFrameObject, 1.0f);	// TRUE
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Default
	}
	else
	{
		glUniform1f(uniLoc_bIsDebugWireFrameObject, 0.0f);	// FALSE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Default
	}
	///////////////////////////////////////////

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glBindVertexArray(VAODrawInfo.VAO_ID);

	glDrawElements(GL_TRIANGLES,
		VAODrawInfo.numberOfIndices,		// testMesh.numberOfTriangles * 3,	// How many vertex indices
		GL_UNSIGNED_INT,					// 32 bit int 
		0);
	// Unbind that VAO
	glBindVertexArray(0);

	return;
}

void getUniformLocations(int& mvp_location, int &currentProgID) {
	// Get the uniform locations for this shader
	mvp_location = glGetUniformLocation(currentProgID, "MVP");		// program, "MVP");
	uniLoc_materialDiffuse = glGetUniformLocation(currentProgID, "materialDiffuse");
	uniLoc_materialAmbient = glGetUniformLocation(currentProgID, "materialAmbient");
	uniLoc_ambientToDiffuseRatio = glGetUniformLocation(currentProgID, "ambientToDiffuseRatio");
	uniLoc_materialSpecular = glGetUniformLocation(currentProgID, "materialSpecular");
	uniLoc_bIsDebugWireFrameObject = glGetUniformLocation(currentProgID, "bIsDebugWireFrameObject");
	uniLoc_eyePosition = glGetUniformLocation(currentProgID, "eyePosition");
	uniLoc_mModel = glGetUniformLocation(currentProgID, "mModel");
	uniLoc_mView = glGetUniformLocation(currentProgID, "mView");
	uniLoc_mProjection = glGetUniformLocation(currentProgID, "mProjection");
}


void populateTextures() {
	::g_pTextureManager->setBasePath("assets/textures");
	if (!::g_pTextureManager->Create2DTextureFromBMPFile("Utah_Teapot_xyz_n_uv_Checkerboard.bmp", true))
	{
		std::cout << "Didn't load the texture. Oh no!" << std::endl;
	}
	else
	{
		std::cout << "Texture is loaded! Hazzah!" << std::endl;
	}
	::g_pTextureManager->Create2DTextureFromBMPFile("Utah_Teapot_xyz_n_uv_Enterprise.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GuysOnSharkUnicorn.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GuysOnSharkUnicorn.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("barberton_etm_2001121_lrg.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("height_map_norway-height-map-aster-30m.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("A_height_map_norway-height-map-aster-30m.bmp", true);

	::g_pTextureManager->setBasePath("assets/textures/skybox");
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles(
		"space",
		"SpaceBox_right1_posX.bmp",
		"SpaceBox_left2_negX.bmp",
		"SpaceBox_top3_posY.bmp",
		"SpaceBox_bottom4_negY.bmp",
		"SpaceBox_front5_posZ.bmp",
		"SpaceBox_back6_negZ.bmp", true, true))
	{
		std::cout << "Didn't load skybox" << std::endl;
	}
}
