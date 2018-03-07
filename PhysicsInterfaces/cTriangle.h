#ifndef  _sTriangle_HG_
#define _sTriangle_HG_
#include <glm\vec3.hpp>



struct cTriangle {
public:
	cTriangle() {
		vertex_ID_0 = -1;
		vertex_ID_1 = -1;
		vertex_ID_2 = -1;
	};

	int vertex_ID_0;
	int vertex_ID_1;
	int vertex_ID_2;
};

class cPhysTriangle
{
public:
	glm::vec3 vertex[3];
	glm::vec3 normals[3];
	//int TestSphereTriangle(cGameObject* s, glm::vec3* a, glm::vec3* b, glm::vec3* c, glm::vec3 &p);
};

#endif // ! _sTriangle_HG_
