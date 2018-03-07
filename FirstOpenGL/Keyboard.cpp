#include "Keyboard.h"
#include "cGameObject.h"
#include <iostream>
#include <vector>
#include <glm\vec3.hpp>
#include "cLightManager.h"
#include "LuaBrain.h"
#include <iPhysicsWorld.h>
#include <iRigidBody.h>
#include "ModelUtilities.h"

extern glm::vec3 g_cameraXYZ;
extern glm::vec3 g_cameraTarget_XYZ;
extern cGameObject* g_curGameObject;
extern std::vector<cGameObject*> g_vecGameObjects;
extern cLightManager*		g_pLightManager;

int objectIndex = 0;
int lightIndex = 0;

LuaBrain* theBrain = new LuaBrain();
glm::vec3 vel;

extern bool drawDebugInfo;

extern nPhysics::iPhysicsWorld* theBulletWorld;
extern nPhysics::iPhysicsWorld* theCustomWorld;
extern bool isBullet;

nPhysics::iRigidBody* theBody;

glm::vec3 forward(0.0f);
glm::vec3 right(0.0f);
//TODO:: 
// set all bools to false 
//update the index when it is pressed 
//Then in the game loop go through and update the camera "stuff"
bool keyIsPressed[256];

void Keyboard::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//vector from camera to target
	forward = g_cameraTarget_XYZ - g_cameraXYZ;
	forward.y = 0; 
	forward = glm::normalize(forward);

	right = glm::cross(forward, glm::vec3(0.f, 1.0f, 0.0f));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (drawDebugInfo)
			drawDebugInfo = false;
		else
			drawDebugInfo = true;
	}
	

	const float CAMERASPEED = 2.0f;
	const float ballSpeed = 15.f;

	switch (key)
	{
	case GLFW_KEY_LEFT:		// Left
		g_cameraXYZ.x -= CAMERASPEED;
		break;
	case GLFW_KEY_RIGHT:		// Right
		g_cameraXYZ.x += CAMERASPEED;
		break;
	case GLFW_KEY_RIGHT_SHIFT:		// Forward (along z)
		g_cameraXYZ.z += CAMERASPEED;
		break;
	case GLFW_KEY_KP_1:		// Backwards (along z)
		g_cameraXYZ.z -= CAMERASPEED;
		break;
	case GLFW_KEY_UP:		// "Down" (along y axis)
		g_cameraXYZ.y -= CAMERASPEED;
		break;
	case GLFW_KEY_DOWN:		// "Up" (along y axis)
		g_cameraXYZ.y += CAMERASPEED;
		break;

	case GLFW_KEY_A:
		//TODO: change this to apply force 
		//use the forwar or right vector to control what direction the spheres move
		theBody = dynamic_cast<nPhysics::iRigidBody*>(g_vecGameObjects[objectIndex]->theBody);
		theBody->ApplyImpulse(right * -ballSpeed);
		break;
	case GLFW_KEY_D:
		theBody = dynamic_cast<nPhysics::iRigidBody*>(g_vecGameObjects[objectIndex]->theBody);
		theBody->ApplyImpulse(right * ballSpeed);
		break;
	case GLFW_KEY_W:
		theBody = dynamic_cast<nPhysics::iRigidBody*>(g_vecGameObjects[objectIndex]->theBody);
		theBody->ApplyImpulse(forward * ballSpeed);
		break;
	case GLFW_KEY_S:
		theBody = dynamic_cast<nPhysics::iRigidBody*>(g_vecGameObjects[objectIndex]->theBody);
		theBody->ApplyImpulse(-forward * ballSpeed);
		break;
	case GLFW_KEY_Q:
		theBody = dynamic_cast<nPhysics::iRigidBody*>(g_vecGameObjects[objectIndex]->theBody);
		theBody->ApplyImpulse(glm::vec3(0.0f,1.0f,0.0f) * ballSpeed);
		break;
	//case GLFW_KEY_E:
	//	vel = g_vecGameObjects[objectIndex]->vel;
	//	vel.y -= ballSpeed;
	//	g_vecGameObjects[objectIndex]->theBody->SetVelocity(vel);
	//	break;
	}


	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		if (objectIndex == 10)
			objectIndex = 0;
		else
			objectIndex++;

		if (objectIndex > 0)
			g_vecGameObjects[objectIndex - 1]->diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		else
			g_vecGameObjects[g_vecGameObjects.size() - 1]->diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		g_vecGameObjects[objectIndex]->diffuseColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		g_cameraTarget_XYZ = g_vecGameObjects[objectIndex]->position;
	}

	return;
}