#ifndef  _iCollisionBody_HG_
#define _iCollisionBody_HG_
#include "eBodyType.h"


namespace nPhysics {
	class iCollisionBody {
	public :
		virtual ~iCollisionBody() {}
		inline eBodyType GetBodyType() const { return mType; }

	protected:
		iCollisionBody(eBodyType type) :mType(type) {}
		iCollisionBody(const iCollisionBody& other) {}
		iCollisionBody& operator=(const iCollisionBody& other) { return *this; }

	private:
		eBodyType mType;

	};
}

#endif // ! _iCollisionBody_HG)
