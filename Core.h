#ifndef _CORE_H_
#define _CORE_H_

#include <SDL.h>

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
#include <math.h>

#include "Logger.h"
#include "KeyBuffer.h"
#include "MouseBuffer.h"
#include "ShaderGL.h"
#include "BitmapFontGL.h"
#include "TextEditor.h"
#include "KeyAnalyzer.h"
#include "AudioAnalyzer.h"

namespace LiveCoder {
	
const int DefaultWidth = 800;
const int DefaultHeight = 600;

const int EffectNum = 12;


class Core {
private:
	int width;
	int height;
	int bpp;
	bool end;

	bool editMode;
	bool nowCompiled;
	bool errorHighlight;

	int nowEffect;

	std::string title;

	int ProcessSDLEvents();

	KeyBuffer keyBuffer;
	MouseBuffer mouseBuffer;
	ShaderGL shaderGL[EffectNum];
//	ShaderGL postEffect;
	TextEditor textEditor;
	KeyAnalyzer keyAnalyzer;
	AudioAnalyzer *audioAnalyzer;

	std::string nowSource;

	Uint32 baseTime;

	float* audioBuffer;
	GLuint audioTexture;

	// FBOÇ∆Ç©
	GLuint frameBuffer;
	GLuint renderBuffer;
	GLuint renderTexture;

	// é©ï™Ç≈çDÇ´Ç»ÇƒÇ≠Ç∑ÇøÇ·ÇégÇ¶ÇÈÇ∆Ç§ÇÍÇµÇ¢ÇÀ
	GLuint optionTexture;
public:

	int Initialize(std::string title_, int width_, int height_, int SDLflags);
	int MainLoop();
	void Render();

	Core();
	virtual ~Core();
};

};

#endif