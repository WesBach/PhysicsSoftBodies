#ifndef _ModelUtilities_HG_
#define _ModelUtilities_HG_

#include <fstream>
#include <string>
#include "cMesh.h"
#include <sSoftBodyDesc.h>

class cGameObject;

// Only "function signature"
void ReadFileToToken( std::ifstream &file, std::string token );

// Note the & symbol which is "by reference" which means
//	it's NOT a copy, but referring to the original object.
// ALSO note that this is a C++ thing (& isn't a thing in C)
bool LoadPlyFileIntoMesh( std::string filename, cMesh &theMesh );
bool LoadPlyFileIntoMeshWithNormals( std::string filename, cMesh &theMesh );
bool LoadPlyFileIntoMeshWith_Normals_and_UV(std::string filename, cMesh &theMesh);
bool LoadTexturesFromFile(std::string& fileName);
bool MapTexturesToProperObjects();

void updateSoftBodyMeshFromPhysics();
void populateSoftBodyDesc(nPhysics::sSoftBodyDesc& theDesc,const cGameObject& theObject);

#endif
