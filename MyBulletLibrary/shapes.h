#ifndef _Shapes_HG_
#define _Shapes_HG_
#include <iShape.h>
#include <bullet\btBulletDynamicsCommon.h>

namespace nPhysics {

	class cBulletSphereShape : public iShape
	{
	public:
		cBulletSphereShape(float radius);
		virtual ~cBulletSphereShape();
		virtual bool GetSphereRadius(float& radiusOut);

	private:
		cBulletSphereShape();
		cBulletSphereShape(const cBulletSphereShape& other);
		cBulletSphereShape& operator=(const cBulletSphereShape& other);

		float mRadius;
	};

	class cBulletPlaneShape : public iShape
	{
	public:
		cBulletPlaneShape(const glm::vec3& normal, float planeConst);

		virtual ~cBulletPlaneShape();

		virtual bool GetPlaneNormal(glm::vec3& normalOut);
		virtual bool GetPlaneConst(float& planeConstOut);

	private:
		cBulletPlaneShape();
		cBulletPlaneShape(const cBulletPlaneShape& other);
		cBulletPlaneShape& operator=(const cBulletPlaneShape& other);

		glm::vec3 mNormal;
		float mPlaneConst;
	};
}


#endif
