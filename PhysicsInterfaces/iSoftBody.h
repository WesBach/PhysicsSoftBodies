#ifndef _iSoftBody_HG_
#define _iSoftBody_HG_
#include <glm\vec3.hpp>
#include "iCollisionBody.h"

namespace nPhysics {
	class iSoftBody :public iCollisionBody {
	public:
		virtual ~iSoftBody() {};
		virtual void GetNodePosition(size_t index, glm::vec3& nodePositionOnMesh) = 0;
		virtual size_t NumNodes() = 0;

	protected:
		iSoftBody() :iCollisionBody(SOFT_BODY) {}
		//don't use
		iSoftBody(const iSoftBody& other):iCollisionBody(other){}
		iSoftBody& operator=(const iSoftBody& other) { return *this; }
	};
}

#endif // !_iSoftBody_HG_
