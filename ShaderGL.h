#ifndef _SHADER_H_
#define _SHADER_H_

#ifdef	__WIN32__
#include <windows.h>
#endif	__WIN32__

#ifdef	__APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <string>

#include "Logger.h"

namespace LiveCoder {

class ShaderGL {
private:
	bool OK;
	GLuint shaderProgram;
public:
	ShaderGL();
	virtual ~ShaderGL();

	GLuint Compile(const std::string& fsshader);

	GLuint CompileFromFile(const std::string& filename);

	bool Valid();
	void Bind();
	void Unbind();
	void SetUniform(const GLchar* name, int i);
	void SetUniform(const GLchar* name, float v);
	void SetUniform(const GLchar* name, float* fv, int size);
	void SetUniform(const GLchar* name, float x, float y);
	
};

};

#endif