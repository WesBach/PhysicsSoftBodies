#ifndef _sSoftBodyDesc_HG_
#define _sSoftBodyDesc_HG_
#include <vector>
#include "cTriangle.h"
#include <glm\vec3.hpp>

namespace nPhysics {
	struct sSoftBodyDesc {
		std::vector<glm::vec3> Vertices;
		std::vector<cTriangle*> TriangulatedIndices;
		std::vector<size_t> StaticIndices;
	};
}
#endif // !_sSoftBodyDesc_HG_
