#ifndef _cShader_HG_
#define _cShader_HG_
#include <string>
#include <vector>

class cShader {
public:
	cShader();
	~cShader();
	enum eShaderType
	{
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		GEOMETRY_SHADER,
		TESSELATION_SHADER,
		UNKNOWN
	};

	eShaderType shaderType;
	std::string getShaderTypeString(void);

	unsigned int ID;	// or "name" from OpenGL
	std::vector<std::string> vecSource;
	void parseStringIntoMultiLine(std::string singleLineSouce);
	bool bSourceIsMultiLine;
	std::string fileName;
};

#endif // !_cShader_HG_

