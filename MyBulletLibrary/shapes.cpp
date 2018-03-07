#include "Shapes.h"

namespace nPhysics {

	cBulletSphereShape::cBulletSphereShape(float radius)
		: iShape(SHAPE_TYPE_SPHERE)
		, mRadius(radius)
	{

	}
	cBulletSphereShape::cBulletSphereShape()
		: iShape(SHAPE_TYPE_SPHERE)
	{

	}

	cBulletSphereShape::cBulletSphereShape(const cBulletSphereShape& other)
		: iShape(SHAPE_TYPE_SPHERE)
	{

	}

	cBulletSphereShape& cBulletSphereShape::operator=(const cBulletSphereShape& other)
	{
		return *this;
	}

	cBulletSphereShape::~cBulletSphereShape()
	{

	}

	bool cBulletSphereShape::GetSphereRadius(float& radiusOut)
	{
		radiusOut = mRadius;
		return true;
	}

	cBulletPlaneShape::cBulletPlaneShape(const glm::vec3& normal, float planeConst)
		: iShape(SHAPE_TYPE_PLANE)
		, mNormal(normal)
		, mPlaneConst(planeConst)
	{

	}

	cBulletPlaneShape::cBulletPlaneShape()
		: iShape(SHAPE_TYPE_PLANE)
	{

	}

	cBulletPlaneShape::cBulletPlaneShape(const cBulletPlaneShape& other)
		: iShape(SHAPE_TYPE_PLANE)
	{

	}
	cBulletPlaneShape& cBulletPlaneShape::operator=(const cBulletPlaneShape& other)
	{
		return *this;
	}

	cBulletPlaneShape::~cBulletPlaneShape()
	{

	}

	bool cBulletPlaneShape::GetPlaneNormal(glm::vec3& normalOut)
	{
		normalOut = mNormal;
		return true;
	}

	bool cBulletPlaneShape::GetPlaneConst(float& planeConstOut)
	{
		planeConstOut = mPlaneConst;
		return true;
	}
}