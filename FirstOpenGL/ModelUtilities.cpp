#include "ModelUtilities.h" 
#include "cVAOMeshManager.h"
#include "cShaderManager.h"
#include "cMesh.h"
#include <sstream>
#include "cGameObject.h"
#include "cLightManager.h"
#include "CTextureManager.h"
#include <glm\vec3.hpp>
#include <glm\glm.hpp>
#include "globalOpenGL_GLFW.h"
#include <iPhysicsFactory.h>
#include <iRigidBody.h>
#include <iSoftBody.h>
#include <eBodyType.h>
#include "cVAOMeshManager.h"

extern glm::vec3 g_cameraXYZ;
extern std::vector<cGameObject*> g_vecGameObjects;


extern nPhysics::iPhysicsWorld*		theCustomWorld;
extern nPhysics::iPhysicsFactory*	theCustomFactory; 
extern cShaderManager*				g_pShaderManager;		// Heap, new (and delete)
extern cVAOMeshManager*				g_pVAOManager;

void ReadFileToToken(std::ifstream &file, std::string token)
{
	bool bKeepReading = true;
	std::string garbage;
	do
	{
		file >> garbage;		// Title_End??
		if (garbage == token)
		{
			return;
		}
	} while (bKeepReading);
	return;
}


// Takes a file name, loads a mesh
bool LoadPlyFileIntoMesh(std::string filename, cMesh &theMesh)
{
	// Load the vertices
	// c_str() changes a string to a "c style char* string"
	std::ifstream plyFile(filename.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return
		return false;
	}
	// File is open, let's read it

	ReadFileToToken(plyFile, "vertex");
	//	int numVertices = 0;
	plyFile >> theMesh.numberOfVertices;

	ReadFileToToken(plyFile, "face");
	//	int numTriangles = 0;
	plyFile >> theMesh.numberOfTriangles;

	ReadFileToToken(plyFile, "end_header");

	// Allocate the appropriate sized array (+a little bit)
	//theMesh.pVertices = new cVertex_xyz_rgb_n[theMesh.numberOfVertices];
	theMesh.pVertices = new sVertex_xyz_rgba_n_uv2_bt[theMesh.numberOfVertices];
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Read vertices
	for (int index = 0; index < theMesh.numberOfVertices; index++)
	{
		//end_header
		//-0.0312216 0.126304 0.00514924 0.850855 0.5 		
		float x, y, z;//, confidence, intensity;

		plyFile >> x;
		plyFile >> y;
		plyFile >> z;
		//		plyFile >> confidence;
		//		plyFile >> intensity;

		theMesh.pVertices[index].x = x;	// vertices[index].x = x;
		theMesh.pVertices[index].y = y;	// vertices[index].y = y;
		theMesh.pVertices[index].z = z;
		theMesh.pVertices[index].r = 1.0f;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].g = 1.0f;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].b = 1.0f;	// vertices[index].r = 1.0f;
	}

	// Load the triangle (or face) information, too
	for (int count = 0; count < theMesh.numberOfTriangles; count++)
	{
		// 3 164 94 98 
		int discard = 0;
		plyFile >> discard;									// 3
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;	// 164
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;	// 94
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;	// 98
	}

	//theMesh.CalculateNormals();

	return true;
}

// Takes a file name, loads a mesh
bool LoadPlyFileIntoMeshWithNormals(std::string filename, cMesh &theMesh)
{
	// Load the vertices
	// c_str() changes a string to a "c style char* string"
	std::string file = "assets\\models\\" + filename;
	std::ifstream plyFile(file.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return
		return false;
	}
	// File is open, let's read it

	ReadFileToToken(plyFile, "vertex");
	//	int numVertices = 0;
	plyFile >> theMesh.numberOfVertices;

	ReadFileToToken(plyFile, "face");
	//	int numTriangles = 0;
	plyFile >> theMesh.numberOfTriangles;

	ReadFileToToken(plyFile, "end_header");

	// Allocate the appropriate sized array (+a little bit)
	//theMesh.pVertices = new cVertex_xyz_rgb_n[theMesh.numberOfVertices];
	theMesh.pVertices = new sVertex_xyz_rgba_n_uv2_bt[theMesh.numberOfVertices];
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Read vertices
	for (int index = 0; index < theMesh.numberOfVertices; index++)
	{
		//end_header
		//-0.0312216 0.126304 0.00514924 0.850855 0.5 		
		float x, y, z, nx, ny, nz;//, confidence, intensity;

		plyFile >> x;
		plyFile >> y;
		plyFile >> z;
		plyFile >> nx >> ny >> nz;
		//		plyFile >> confidence;
		//		plyFile >> intensity;

		theMesh.pVertices[index].x = x;	// vertices[index].x = x;
		theMesh.pVertices[index].y = y;	// vertices[index].y = y;
		theMesh.pVertices[index].z = z;
		theMesh.pVertices[index].r = 1.0f;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].g = 1.0f;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].b = 1.0f;	// vertices[index].r = 1.0f;
		theMesh.pVertices[index].nx = nx;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].ny = ny;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].nz = nz;	// vertices[index].r = 1.0f;
	}

	// Load the triangle (or face) information, too
	for (int count = 0; count < theMesh.numberOfTriangles; count++)
	{
		// 3 164 94 98 
		int discard = 0;
		plyFile >> discard;									// 3
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;	// 164
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;	// 94
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;	// 98
	}

	//	theMesh.CalculateNormals();

	return true;
}

// Takes a file name, loads a mesh
bool LoadPlyFileIntoMeshWith_Normals_and_UV(std::string filename, cMesh &theMesh)
{
	// Load the vertices
	std::string file = "assets\\models\\" + filename;
	// c_str() changes a string to a "c style char* string"
	std::ifstream plyFile(file.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return
		return false;
	}
	// File is open, let's read it

	ReadFileToToken(plyFile, "vertex");
	//	int numVertices = 0;
	plyFile >> theMesh.numberOfVertices;

	ReadFileToToken(plyFile, "face");
	//	int numTriangles = 0;
	plyFile >> theMesh.numberOfTriangles;

	ReadFileToToken(plyFile, "end_header");

	// Allocate the appropriate sized array (+a little bit)
	//theMesh.pVertices = new cVertex_xyz_rgb_n[theMesh.numberOfVertices];
	theMesh.pVertices = new sVertex_xyz_rgba_n_uv2_bt[theMesh.numberOfVertices];
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Read vertices
	for (int index = 0; index < theMesh.numberOfVertices; index++)
	{
		//end_header
		//-0.0312216 0.126304 0.00514924 0.850855 0.5 		
		float x, y, z, nx, ny, nz;
		// Added
		float u, v;		// Model now has texture coordinate

						// Typical vertex is now... 
						// 29.3068 -5e-006 24.3079 -0.949597 0.1875 -0.251216 0.684492 0.5

		plyFile >> x >> y >> z;
		plyFile >> nx >> ny >> nz;
		// 
		plyFile >> u >> v;			// ADDED

		//add the vertices to the vector for the physics soft body
		theMesh.theVertices.push_back(glm::vec3(x,y,z));

		theMesh.pVertices[index].x = x;	// vertices[index].x = x;
		theMesh.pVertices[index].y = y;	// vertices[index].y = y;
		theMesh.pVertices[index].z = z;
		theMesh.pVertices[index].r = 1.0f;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].g = 1.0f;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].b = 1.0f;	// vertices[index].r = 1.0f;
		theMesh.pVertices[index].nx = nx;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].ny = ny;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].nz = nz;	// vertices[index].r = 1.0f;

											// Only store the 1st UV.
		theMesh.pVertices[index].u1 = u;
		theMesh.pVertices[index].v1 = v;
	}

	// Load the triangle (or face) information, too
	for (int count = 0; count < theMesh.numberOfTriangles; count++)
	{
		// 3 164 94 98 
		int discard = 0;
		plyFile >> discard;									// 3
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;	// 164
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;	// 94
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;	// 98
		theMesh.theTriangles.push_back(&theMesh.pTriangles[count]);
	}

	//	theMesh.CalculateNormals();

	return true;
}

int count = 0;

void updateSoftBodyMeshFromPhysics() {
	if (count == 1)
	{
		int breakp = 0;
	}
	cGameObject* theSoftBodyGameObject = g_vecGameObjects[g_vecGameObjects.size() - 1];

	cMesh theSoftBodyMesh;
	g_pVAOManager->lookupMeshFromName(theSoftBodyGameObject->meshName, theSoftBodyMesh);

	nPhysics::iSoftBody* theSoftBody = dynamic_cast<nPhysics::iSoftBody*>(theSoftBodyGameObject->theBody);
	int softBodyNodes = theSoftBody->NumNodes();
	//if there arent any nodes return (shouldn't happen)
	if (softBodyNodes == 0) {
		return;
	}

	//loop through the verts and update the values
	for (int i = 0; i < softBodyNodes; i++){
		//update the vertex info
		theSoftBody->GetNodePosition(i, theSoftBodyMesh.theVertices[i]);
	}

	//update the mesh verts
	for (int i = 0; i < theSoftBodyMesh.numberOfVertices; i++) {
		theSoftBodyMesh.pVertices[i].x = theSoftBodyMesh.theVertices[i].x;
		theSoftBodyMesh.pVertices[i].y = theSoftBodyMesh.theVertices[i].y;
		theSoftBodyMesh.pVertices[i].z = theSoftBodyMesh.theVertices[i].z;
	}

	//get the shader program id
	int id = g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	//update the mesh in the vao
	g_pVAOManager->loadMeshIntoVAO(theSoftBodyMesh, id);
	count++;
}


//
//bool Load3DModelsIntoMeshManager(int shaderID, cVAOMeshManager* pVAOManager, std::string &error)
//{
//	std::stringstream ssError;
//	bool bAllGood = true;
//
//	{
//		cMesh testMesh;
//		testMesh.name = "mountain";
//		if (!LoadPlyFileIntoMeshWith_Normals_and_UV("mount.blend.ply", testMesh))
//		{
//			//std::cout << "Didn't load model" << std::endl;
//			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
//			bAllGood = false;
//		}
//		if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID))
//		{
//			//std::cout << "Could not load mesh into VAO" << std::endl;
//			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
//			bAllGood = false;
//		}
//	}
//	//birch_tree1.ply
//
//	{
//		cMesh testMesh;
//		testMesh.name = "house";
//		if (!LoadPlyFileIntoMeshWith_Normals_and_UV("Big_Old_House.ply", testMesh))
//		{
//			//std::cout << "Didn't load model" << std::endl;
//			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
//			bAllGood = false;
//		}
//		if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID))
//		{
//			//std::cout << "Could not load mesh into VAO" << std::endl;
//			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
//			bAllGood = false;
//		}
//	}
//
//	{
//		cMesh testMesh;
//		testMesh.name = "birch";
//		if (!LoadPlyFileIntoMeshWith_Normals_and_UV("birch_tree1.ply", testMesh))
//		{
//			//std::cout << "Didn't load model" << std::endl;
//			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
//			bAllGood = false;
//		}
//		if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID))
//		{
//			//std::cout << "Could not load mesh into VAO" << std::endl;
//			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
//			bAllGood = false;
//		}
//	}
//
//	{
//		cMesh testMesh;
//		testMesh.name = "house1";
//		if (!LoadPlyFileIntoMeshWith_Normals_and_UV("houseA.ply", testMesh))
//		{
//			//std::cout << "Didn't load model" << std::endl;
//			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
//			bAllGood = false;
//		}
//		if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID))
//		{
//			//std::cout << "Could not load mesh into VAO" << std::endl;
//			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
//			bAllGood = false;
//		}
//	}
//
//	{
//		cMesh testMesh;
//		testMesh.name = "lamp";
//		if (!LoadPlyFileIntoMeshWith_Normals_and_UV("Exterior_lamp.ply", testMesh))
//		{
//			//std::cout << "Didn't load model" << std::endl;
//			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
//			bAllGood = false;
//		}
//		if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID))
//		{
//			//std::cout << "Could not load mesh into VAO" << std::endl;
//			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
//			bAllGood = false;
//		}
//	}
//
//	{
//		cMesh testMesh;
//		testMesh.name = "lamp";
//		if (!LoadPlyFileIntoMeshWith_Normals_and_UV("Exterior_lamp.ply", testMesh))
//		{
//			//std::cout << "Didn't load model" << std::endl;
//			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
//			bAllGood = false;
//		}
//		if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID))
//		{
//			//std::cout << "Could not load mesh into VAO" << std::endl;
//			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
//			bAllGood = false;
//		}
//	}
//
//
//	{
//		cMesh testMesh;
//		testMesh.name = "SmoothSphereRadius1InvertedNormals";
//		if (!LoadPlyFileIntoMeshWith_Normals_and_UV("SmoothSphere_Inverted_Normals_xyz_n_uv.ply", testMesh))
//		{
//			//std::cout << "Didn't load model" << std::endl;
//			ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
//			bAllGood = false;
//		}
//		if (!pVAOManager->loadMeshIntoVAO(testMesh, shaderID))
//		{
//			//std::cout << "Could not load mesh into VAO" << std::endl;
//			ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
//			bAllGood = false;
//		}
//	}
//	if (!bAllGood)
//	{
//		// Copy the error string stream into the error string that
//		//	gets "returned" (through pass by reference)
//		error = ssError.str();
//	}
//
//	return bAllGood;
//}

bool LoadLightsAttributesFromFile(std::string& fileName, std::vector<cLight>& theLights) {
	//open the file
	std::ifstream modelAndSceneFile(fileName.c_str());
	cMesh* testMesh;
	int numLights = 0;

	if (!modelAndSceneFile.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(modelAndSceneFile, "NUM_LIGHTS_TO_LOAD");
	modelAndSceneFile >> numLights;

	ReadFileToToken(modelAndSceneFile, "LIGHTS_BEGIN");

	std::string tempType;
	for (int i = 0; i < numLights; i++)
	{

		modelAndSceneFile >> theLights[i].position.x;
		modelAndSceneFile >> theLights[i].position.y;
		modelAndSceneFile >> theLights[i].position.z;
		modelAndSceneFile >> theLights[i].attenuation.x;
		modelAndSceneFile >> theLights[i].attenuation.y;
		modelAndSceneFile >> theLights[i].attenuation.z;
		modelAndSceneFile >> theLights[i].diffuse.r;
		modelAndSceneFile >> theLights[i].diffuse.g;
		modelAndSceneFile >> theLights[i].diffuse.b;
		modelAndSceneFile >> tempType;
		if (tempType == "DIRECTIONAL")
			theLights[i].setLightParamType(cLight::DIRECTIONAL);
		else if (tempType == "POINT")
			theLights[i].setLightParamType(cLight::POINT);
		else if (tempType == "SPOT")
		{
			theLights[i].setLightParamType(cLight::SPOT);
			theLights[i].direction = glm::vec3(0.0f, -1.0f, 0.0f);
			theLights[i].setLightParamSpotPrenumAngleInner(glm::radians(15.0f));
			theLights[i].setLightParamSpotPrenumAngleOuter(glm::radians(25.0f));
		}

	}

	//close the file stream
	modelAndSceneFile.close();
	return 0;
}

bool Load3DModelsFromModelFile(int shaderID, cVAOMeshManager* pVAOManager, std::string &error)
{
	std::string fileName = "modelsAndScene.txt";
	std::ifstream modelNameFile(fileName.c_str());
	//cMesh* testMesh;

	std::stringstream ssError;
	bool bAnyErrors = false;
	std::string tempString;
	int numModels = 0;
	int numModelsUV;

	if (!modelNameFile.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(modelNameFile, "NUM_MODELS_UV");
	modelNameFile >> numModelsUV;
	ReadFileToToken(modelNameFile, "MODEL_NAME_UV_START");

	for (int i = 0; i < numModelsUV; i++)
	{
		modelNameFile >> tempString;
		cMesh* testMesh;
		testMesh = new cMesh();
		testMesh->name = tempString;

		if (testMesh->name == "SmoothSphere_Inverted_Normals_xyz_n.ply" || testMesh->name == "Sphereply_xyz_n.ply")
		{
			if (!LoadPlyFileIntoMeshWithNormals(tempString, *testMesh))
			{
				//std::cout << "Didn't load model" << std::endl;
				ssError << "Didn't load model >" << testMesh->name << "<" << std::endl;
				bAnyErrors = true;
			}

		}
		else
		{
			if (!LoadPlyFileIntoMeshWith_Normals_and_UV(tempString, *testMesh))
			{
				//std::cout << "Didn't load model" << std::endl;
				ssError << "Didn't load model >" << testMesh->name << "<" << std::endl;
				bAnyErrors = true;
			}
		}

		if (!pVAOManager->loadMeshIntoVAO(*testMesh, shaderID,true))
		{
			//std::cout << "Could not load mesh into VAO" << std::endl;
			ssError << "Could not load mesh >" << testMesh->name << "< into VAO" << std::endl;
			bAnyErrors = true;
		}

		tempString = "";
	}

	if (!bAnyErrors)
	{
		error = ssError.str();
	}

	return bAnyErrors;
}

bool LoadTexturesFromFile(std::string& fileName) {
	std::ifstream modelAndSceneFile(fileName.c_str());
	int numTextures = 0;
	if (!modelAndSceneFile.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(modelAndSceneFile, "NUM_TEXTURES");
	modelAndSceneFile >> numTextures;

	ReadFileToToken(modelAndSceneFile, "TEXTURE_NAMES");
	for (int i = 0; i < numTextures; i++)
	{
		std::string temp = "";
		modelAndSceneFile >> temp;
		//load the textures into the texture manager
		g_pTextureManager->Create2DTextureFromBMPFile(temp, true);
		temp = "";
	}
	return true;
}

bool MapTexturesToProperObjects() {

	//MeshLabTerrain_FLAT_xyz_n_uv.ply
	//MeshLabTerrain_FLAT_xyz_n_uv_back.ply
	//MeshLabTerrain_FLAT_xyz_n_uv_front.ply
	//MeshLabTerrain_FLAT_xyz_n_uv_left.ply
	//MeshLabTerrain_FLAT_xyz_n_uv_right.ply
	//Sphere_xyz_n_uv.ply


	for (int i = 0; i < g_vecGameObjects.size(); i++)
	{
		if (g_vecGameObjects[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv.ply")
		{
			g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("terrain.bmp", 0.5f));
		}
		else if (g_vecGameObjects[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv_back.ply")
		{
			g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("rust.bmp", 1.0f));
		}
		else if (g_vecGameObjects[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv_front.ply")
		{
			g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("treeTrunk.bmp", 0.6f));
		}
		else if (g_vecGameObjects[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv_left.ply")
		{
			g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("houseA.bmp", 1.0f));
		}
		else if (g_vecGameObjects[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv_right.ply")
		{
			g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("Big_Old_House_C.bmp", 0.25f));
			g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("Big_Old_House_S.bmp", 0.75f));
		}
		else if (g_vecGameObjects[i]->meshName == "Sphere_xyz_n_uv.ply")
		{
			g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("ground_grass.bmp", 1.0f));
		}
		else if (g_vecGameObjects[i]->meshName == "SmoothSphere_Inverted_Normals_xyz_n.ply")
		{
			g_vecGameObjects[i]->vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
			g_vecGameObjects[i]->bIsSkyBoxObject = true;
		}
		else if (g_vecGameObjects[i]->meshName == "plane.ply")
		{
			g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("ground_grass.bmp", 1.0f));
		}
		//plane.ply

	}

	//for (int i = 0; i < g_vecGameObjects.size(); i++)
	//{
	//	if (g_vecGameObjects[i]->meshName == "MeshLabTerrain_FLAT_xyz_n_uv.ply")
	//	{
	//		//vecMehs2DTextures.push_back(sTextureBindBlendInfo("barberton_etm_2001121_lrg.bmp", 1.0f));
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("ground_grass.bmp", 1.0f));
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("terrain.bmp", 0.5f));
	//	}
	//	else if (g_vecGameObjects[i]->meshName == "Exterior_lamp.ply")
	//	{
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("rust.bmp", 1.0f));
	//	}
	//	else if (g_vecGameObjects[i]->meshName == "Ship_Pack_WIP_mod-gas-harvester_xyz_n_uv.ply")
	//	{
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("rust.bmp", 1.0f));
	//	}
	//	else if (g_vecGameObjects[i]->meshName == "birch_tree.ply")
	//	{
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("leaves.bmp", 0.40f));
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("treeTrunk.bmp", 0.6f));
	//	}
	//	else if (g_vecGameObjects[i]->meshName == "houseA.ply")
	//	{
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("houseA.bmp", 1.0f));
	//	}
	//	else if (g_vecGameObjects[i]->meshName == "Big_Old_House.ply")
	//	{
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("Big_Old_House_C.bmp", 0.25f));
	//		g_vecGameObjects[i]->vecMehs2DTextures.push_back(sTextureBindBlendInfo("Big_Old_House_S.bmp", 0.75f));

	//		//Big_Old_House_S.bmp
	//	}
	//	else if (g_vecGameObjects[i]->meshName == "SmoothSphere_Inverted_Normals_xyz_n.ply")
	//	{
	//		g_vecGameObjects[i]->vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
	//		g_vecGameObjects[i]->bIsSkyBoxObject = true;
	//	}

	//}

	return true;
}

bool LoadSceneFromFile(std::string& fileName) {
	//open the file
	std::ifstream modelAndSceneFile(fileName.c_str());
	int numModels = 0;
	int numModelsUV = 0;
	if (!modelAndSceneFile.is_open())
	{	// Didn't open file, so return
		return false;
	}
	ReadFileToToken(modelAndSceneFile, "CAMERA_POSITION");
	modelAndSceneFile >> g_cameraXYZ.x;
	modelAndSceneFile >> g_cameraXYZ.y;
	modelAndSceneFile >> g_cameraXYZ.z;

	//models with normal and uv
	ReadFileToToken(modelAndSceneFile, "NUMBER_OF_MODELS_WITH_UV");
	modelAndSceneFile >> numModelsUV;
	ReadFileToToken(modelAndSceneFile, "MODELS_WITH_UV_FOR_DRAWING");
	cGameObject* pTempGO;
	for (int i = 0; i < numModelsUV; i++)
	{
		std::string tempString = "";
		pTempGO = new cGameObject();
		float tempFloat = 0.0f;
		float mass = 0.0f;
		modelAndSceneFile >> pTempGO->meshName;
		modelAndSceneFile >> pTempGO->position.x;
		modelAndSceneFile >> pTempGO->position.y;
		modelAndSceneFile >> pTempGO->position.z;
		modelAndSceneFile >> pTempGO->scale;
		modelAndSceneFile >> pTempGO->vel.x;
		modelAndSceneFile >> pTempGO->vel.y;
		modelAndSceneFile >> pTempGO->vel.z;
		modelAndSceneFile >> pTempGO->radius;
		modelAndSceneFile >> pTempGO->mass;
		modelAndSceneFile >> pTempGO->diffuseColour.x;
		modelAndSceneFile >> pTempGO->diffuseColour.y;
		modelAndSceneFile >> pTempGO->diffuseColour.z;
		modelAndSceneFile >> pTempGO->diffuseColour.a;
		modelAndSceneFile >> pTempGO->bIsWireFrame;
		modelAndSceneFile >> pTempGO->orientation.x;
		modelAndSceneFile >> pTempGO->orientation.y;
		modelAndSceneFile >> pTempGO->orientation.z;
		modelAndSceneFile >> tempFloat;
		pTempGO->orientation2.x = glm::radians(tempFloat);
		modelAndSceneFile >> tempFloat;
		pTempGO->orientation2.y = glm::radians(tempFloat);
		modelAndSceneFile >> tempFloat;
		pTempGO->orientation2.z = glm::radians(tempFloat);
		modelAndSceneFile >> pTempGO->bIsUpdatedInPhysics;
		modelAndSceneFile >> tempString;
		modelAndSceneFile >> pTempGO->bHasAABB;
		modelAndSceneFile >> pTempGO->hasTexture;

		nPhysics::sRigidBodyDesc theDesc;
		theDesc.Position = pTempGO->position;
		theDesc.Mass = pTempGO->mass;
		theDesc.Velocity = pTempGO->vel;
		theDesc.Rotation = pTempGO->orientation2;

		nPhysics::sSoftBodyDesc theSoftBodyDesc;

		nPhysics::iShape* shape;

		cMesh theMesh;
		g_pVAOManager->lookupMeshFromName(pTempGO->meshName, theMesh);


		if (tempString == "UNKNOWN")
		{
			pTempGO->typeOfObject = eTypeOfObject::UNKNOWN;
			g_vecGameObjects.push_back(pTempGO);
			continue;
		}
		else if (tempString == "SPHERE"){
			pTempGO->typeOfObject = eTypeOfObject::SPHERE;
			shape = theCustomFactory->CreateSphere(pTempGO->radius);
		}
		else if (tempString == "PLANE")	{
			glm::vec3 normal(theMesh.pVertices[0].nx, theMesh.pVertices[0].ny, theMesh.pVertices[0].nz);
			normal = glm::normalize(normal);
			pTempGO->typeOfObject = eTypeOfObject::PLANE;
			float d = glm::dot(normal, pTempGO->position);
			shape = theCustomFactory->CreatePlane(normal, d);
		}
		else if (tempString == "SOFT_BODY") {
			//get the soft body desc
			pTempGO->typeOfObject = eTypeOfObject::SOFT_BODY;
			populateSoftBodyDesc(theSoftBodyDesc, *pTempGO);
			pTempGO->theBody = theCustomFactory->CreateSoftBody(theSoftBodyDesc);

			theCustomWorld->AddCollisionBody(pTempGO->theBody);
			g_vecGameObjects.push_back(pTempGO);
			continue;
		}

		//CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape)
		//set the body in the physics
		pTempGO->theBody = theCustomFactory->CreateRigidBody(theDesc, shape);

		theCustomWorld->AddCollisionBody(pTempGO->theBody);
		g_vecGameObjects.push_back(pTempGO);
	}


	////models with normals
	//ReadFileToToken(modelAndSceneFile, "NUMBER_OF_MODELS_TO_DRAW");
	//modelAndSceneFile >> numModels;

	//ReadFileToToken(modelAndSceneFile, "MODELS_FOR_DRAWING");
	//for (int i = 0; i < numModels; i++)
	//{
	//	std::string tempString = "";
	//	pTempGO = new cGameObject();
	//	modelAndSceneFile >> pTempGO->meshName;
	//	modelAndSceneFile >> pTempGO->position.x;
	//	modelAndSceneFile >> pTempGO->position.y;
	//	modelAndSceneFile >> pTempGO->position.z;
	//	modelAndSceneFile >> pTempGO->scale;
	//	modelAndSceneFile >> pTempGO->vel.x;
	//	modelAndSceneFile >> pTempGO->vel.y;
	//	modelAndSceneFile >> pTempGO->vel.z;
	//	modelAndSceneFile >> pTempGO->radius;
	//	modelAndSceneFile >> pTempGO->diffuseColour.x;
	//	modelAndSceneFile >> pTempGO->diffuseColour.y;
	//	modelAndSceneFile >> pTempGO->diffuseColour.z;
	//	modelAndSceneFile >> pTempGO->bIsWireFrame;
	//	modelAndSceneFile >> pTempGO->orientation.x;
	//	modelAndSceneFile >> pTempGO->orientation.y;
	//	modelAndSceneFile >> pTempGO->orientation.z;
	//	modelAndSceneFile >> pTempGO->orientation2.x;
	//	modelAndSceneFile >> pTempGO->orientation2.y;
	//	modelAndSceneFile >> pTempGO->orientation2.z;
	//	modelAndSceneFile >> pTempGO->bIsUpdatedInPhysics;
	//	modelAndSceneFile >> tempString;
	//	modelAndSceneFile >> pTempGO->bHasAABB;

	//	g_vecGameObjects.push_back(pTempGO);

	//	if (tempString == "UNKNOWN")
	//	{
	//		pTempGO->typeOfObject = eTypeOfObject::UNKNOWN;
	//	}
	//	else if (tempString == "SPHERE")
	//	{
	//		pTempGO->typeOfObject = eTypeOfObject::SPHERE;
	//		//g_theBomb = pTempGO;
	//	}
	//	else
	//	{
	//		pTempGO->typeOfObject = eTypeOfObject::PLANE;
	//	}

	//}

	//close the file stream
	modelAndSceneFile.close();
	return 0;
}


void populateSoftBodyDesc(nPhysics::sSoftBodyDesc& theDesc,const cGameObject& theObject)
{
	//TODO:: get the mesh info 
	//populate the desc with the required info
	cMesh theSoftBodyMesh;
	g_pVAOManager->lookupMeshFromName(theObject.meshName, theSoftBodyMesh);

	//have the mesh now popualate the desc triangulated triangles
	theDesc.TriangulatedIndices = theSoftBodyMesh.theTriangles;

	//populate vertices
	theDesc.Vertices = theSoftBodyMesh.theVertices;


	//theDesc.StaticIndices.push_back(0);
	//theDesc.StaticIndices.push_back(1);
	//theDesc.StaticIndices.push_back(2);
	//theDesc.StaticIndices.push_back(14);
	//theDesc.StaticIndices.push_back(15);
	//theDesc.StaticIndices.push_back(16);

	//theDesc.StaticIndices.push_back(17);
	//theDesc.StaticIndices.push_back(18);
	//theDesc.StaticIndices.push_back(19);
	//theDesc.StaticIndices.push_back(31);
	//theDesc.StaticIndices.push_back(32);
	//theDesc.StaticIndices.push_back(33);

	//set the top of the mesh to not be effected by gravity
	for (int i = 288; i > 254; i--) {
		if (i == 281 || i == 280 || i == 279 || i == 264 || i == 263 || i == 262) {
			continue;
		}
		theDesc.StaticIndices.push_back(i);
	}


	//1 2 3 
	//theDesc.StaticIndices.push_back(288);
	//theDesc.StaticIndices.push_back(287);
	//theDesc.StaticIndices.push_back(286);
	////15 16 17 
	//theDesc.StaticIndices.push_back(274);
	//theDesc.StaticIndices.push_back(273);
	//theDesc.StaticIndices.push_back(272);
	////18 19 20
	//theDesc.StaticIndices.push_back(271);
	//theDesc.StaticIndices.push_back(270);
	//theDesc.StaticIndices.push_back(269);
	//////32 33 34
	//theDesc.StaticIndices.push_back(255);
	//theDesc.StaticIndices.push_back(254);
	//theDesc.StaticIndices.push_back(253);
}
