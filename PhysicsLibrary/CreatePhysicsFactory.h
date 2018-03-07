#pragma once
#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

namespace nPhysics {
	EXTERN_DLL_EXPORT iPhysicsFactory* CreateFactory();
}
