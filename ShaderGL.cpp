#include "ShaderGL.h"

namespace LiveCoder {

enum ShaderGLType {
	GLSL_VS,
	GLSL_FS
};
	
void getErrorLog(GLuint shader, std::set<int>* errorLines = NULL)
{
	GLsizei bufSize = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		GLchar *infoLog;
		GLsizei length;

		infoLog = new GLchar[bufSize];

		glGetShaderInfoLog(shader, bufSize, &length, infoLog);
		Logger::Instance()->OutputString("Compile Status: " + std::string(infoLog));

		// 適当な解析
		if (errorLines != NULL) {
			std::string tmpStr;
			for (int i = 0; i <  bufSize; i ++) {
				if (infoLog[i] == '\n') {
					int num = 0;
					int numcnt = 0;
					bool inNum = false;
					for (int j = 0; j < tmpStr.length(); j ++) {
						if (inNum) {
							if (isdigit(tmpStr[j])) {
								num = num * 10 + (tmpStr[j] - '0');
							} else {
								inNum = false;
								numcnt ++;

								// 二番目の数字
								if (numcnt == 2) {
									errorLines->insert(num);
								}
							}
						} else {
							if (isdigit(tmpStr[j])) {
								inNum = true;
								num = num * 10 + (tmpStr[j] - '0');
							}
						}
					}
					tmpStr = "";
				} else {
					tmpStr += infoLog[i];
				}
			}
		}

		delete[] infoLog;
	}
}

GLuint CompileShader(ShaderGLType type, const GLchar* source, std::set<int>* errorLines = NULL)
{
	GLint status;
	unsigned int prog = 0;
	switch (type) {
	case GLSL_VS:
		{
 			prog = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(prog, 1, &source, 0);
			glCompileShader(prog);
			glGetShaderiv(prog, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE) {
				getErrorLog(prog, errorLines);
				Logger::Instance()->OutputString("Compile error in vertex shader.");
				glDeleteShader(prog);
				prog = 0;
			}

			return prog;
		}break;
	case GLSL_FS:
		{
			prog = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(prog, 1, &source, 0);
			glCompileShader(prog);
			glGetShaderiv(prog, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE) {
				getErrorLog(prog, errorLines);
				Logger::Instance()->OutputString("Compile error in fragment shader.");
				glDeleteShader(prog);
				prog = 0;
			}

			return prog;
		}break;
	}
	return 0;
}


GLuint LinkShader(GLuint vsh, GLuint fsh)
{
	GLuint program = 0;
	if (vsh != 0 && fsh != 0) {
		program = glCreateProgram();
		glAttachShader(program, vsh);
		glAttachShader(program, fsh);
		// リンク
		glLinkProgram(program);
		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			Logger::Instance()->OutputString("Link Error.");
			glDeleteProgram(program);
			program = 0;
		}
	}

	return program;
}

ShaderGL::ShaderGL() {
	OK = false;
	shaderProgram = 0;
}

GLuint ShaderGL::CompileFromFile(const std::string& filename) {
	FILE* fp = fopen(filename.c_str(), "rt");
	if (fp != NULL) {
		std::string fsshader;
		char buf[1024];
		while (fgets(buf, 1024, fp) != NULL) {
			fsshader += buf;
		}

		fclose(fp);

		return Compile(fsshader);

	}
	return 0;
}

GLuint ShaderGL::Compile(const std::string& fsshader) {
	// 頂点シェーダー（固定）
	static const std::string vsshader = "attribute vec2 pos;void main(){gl_Position=vec4(pos.xy,0,1);}";

	errorLinesVS.clear();
	GLuint vsh = CompileShader(GLSL_VS, vsshader.c_str(), &errorLinesVS); 
	if (vsh == 0) {
		Logger::Instance()->OutputString("Vertex Shader Error.");
		return 0;
	}

	errorLinesFS.clear();
	GLuint fsh = CompileShader(GLSL_FS, fsshader.c_str(), &errorLinesFS);
	if (fsh == 0) {
		Logger::Instance()->OutputString("Fragment Shader Error.");
		glDeleteShader(vsh);
		return 0;
	}

	GLuint program = LinkShader(vsh, fsh);
	if (program != 0) {
		OK = true;
		if (shaderProgram != 0)
			glDeleteProgram(shaderProgram);
		shaderProgram = program;

		glDeleteShader(vsh);
		glDeleteShader(fsh);
	} else
		return 0;

	// 新しくシェーダープログラムセットされたらここにくる
	return shaderProgram;
}

ShaderGL::~ShaderGL() {
	if (shaderProgram != 0) {
		glDeleteProgram(shaderProgram);
	}
}

bool ShaderGL::Valid() {
	return OK;
}

void ShaderGL::Bind() {
	glUseProgram(shaderProgram);
}

void ShaderGL::Unbind() {
	glUseProgram(0);
}

void ShaderGL::SetUniform(const GLchar* name, int i) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform1i(id, i);
	}
}

void ShaderGL:: SetUniform(const GLchar* name, float v) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform1f(id, v);
	}
}

void ShaderGL:: SetUniform(const GLchar* name, float* fv, int size) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform1fv(id, size, fv);
	}
}

void ShaderGL:: SetUniform(const GLchar* name, float x, float y) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform2f(id, x, y);
	}
}

};