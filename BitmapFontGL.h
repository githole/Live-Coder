#ifndef _BITMAP_FONT_GL_
#define _BITMAP_FONT_GL_

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

#include <string.h>

#include "lcfont_bw112x66.h"
#include "inconsolata_224x123.h"
#include "inconsolata_160x90.h"

namespace LiveCoder {

	
#define SyntaxTableNum 38
static const char* syntaxTable[] = {
	"if", "break", "int", "case", "continue", "return", "default", "do",
    "else", "struct", "switch", "float", "for", "unsigned", "goto", "while", "void", "const", "signed",
    "vec2", "vec3", "vec4", "mat2", "mat3", "mat4", "precision", "lowp", "mediump", "highp", "uniform",
	"attribute", "varying", "gl_Vertex", "gl_FragCoord", "gl_FragColor", "sampler2D", "#ifdef", "#endif",
};

class BitmapFontGL
{
	private:
		unsigned int texName;
		BitmapFontGL(){};
		
		bool IsComment;

		int fontWidth;
		int fontHeight;
	public:
		static BitmapFontGL* Instance() {    
			static BitmapFontGL instance;  // 唯一のインスタンス
    		return &instance;
		}

		void SetFontSize(int width, int height) {
			fontWidth = width;
			fontHeight = height;
		}

		void CreateTexture()
		{
			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			glGenTextures(1, &texName);
			glBindTexture(GL_TEXTURE_2D, texName);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, 160, 90,
				0, GL_LUMINANCE, GL_UNSIGNED_BYTE, inconsolata_160x90);
			glPopAttrib();
		}

		void DrawCursor(int col, int row, float aspect, float width) {
			unsigned int ddx = row * fontWidth;
			unsigned int ddy = -col * fontHeight;
			unsigned int w = 2;
			glPushMatrix();
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_1D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			glScalef(0.25f * 8.0f / width,
					 0.25f * 8.0f / width * aspect,
					 0.25f * 8.0f / width);
					glBegin(GL_QUADS);
							glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
							glVertex2i  (ddx              , ddy       );
							glVertex2i  (ddx + w  , ddy       );
							glVertex2i  (ddx + w  , ddy - fontHeight  );
							glVertex2i  (ddx              , ddy - fontHeight  );
					glEnd();	
			glPopAttrib();
			glPopMatrix();
		}

		void DrawSelect(float aspect, float width, int line, int start, int end, float red = 1.0f, float green = 1.0f, float blue = 1.0f) {
			unsigned int ddx = start * fontWidth;
			unsigned int ddy = -line * fontHeight;
			unsigned int w = 4 + (end - start) * fontWidth;
			glPushMatrix();
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_1D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			glScalef(0.25f * 8.0f / width,
					 0.25f * 8.0f / width * aspect,
					 0.25f * 8.0f / width);
					glBegin(GL_QUADS);
							glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
							glVertex2i  (ddx              , ddy       );
							glVertex2i  (ddx + w  , ddy       );
							glVertex2i  (ddx + w  , ddy - fontHeight  );
							glVertex2i  (ddx              , ddy - fontHeight  );
					glEnd();	
			glPopAttrib();
			glPopMatrix();
		}

		void Reset() {
			IsComment = false;
		}

		void ProcessComment(std::string& str) {
			for (int i = 0; i < str.length(); i ++) {
				if (IsComment) {
					int loc = str.find("*/", i);
					if (loc != std::string::npos) {
						IsComment = false;
						i = loc + 2;
					} else break;
				} else {
					int loc = str.find("/*", i);
					if (loc != std::string::npos) {
						IsComment = true;
						i = loc + 2;
					} else break;
				}
			}
		}

		void DrawLine(const char* strbuf, float aspect, float width, int line, float upAlpha, float downAlpha, bool isError = false, float red = 1.0f, float green = 1.0f, float blue = 1.0f) {
			unsigned int ddx = 0;
			unsigned int ddy = -fontHeight * line;
			glPushMatrix();
			glScalef(0.25f * 8.0f / width,
					 0.25f * 8.0f / width * aspect,
					 0.25f * 8.0f / width);
			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			///glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			
			glEnable(GL_COLOR_MATERIAL);

			glBindTexture(GL_TEXTURE_2D, texName);
			unsigned int strend = strlen(strbuf);
			std::string str = strbuf;
			std::vector<int> syntaxColor;

			if (IsComment) {
				syntaxColor.resize(strend, 2);
			} else {
				syntaxColor.resize(strend, 0);
			}

			for (int i = 0; i < str.length(); i ++) {
				if (IsComment) {
					int loc = str.find("*/", i);
					if (loc != std::string::npos) {
						IsComment = false;
						for (int j = loc + 2; j < str.length(); j ++)
							syntaxColor[j] = 0;
						i = loc + 2;
					} else break;
				} else {
					int loc = str.find("/*", i);
					if (loc != std::string::npos) {
						IsComment = true;
						for (int j = loc; j < str.length(); j ++)
							syntaxColor[j] = 2;
						i = loc + 2;
					} else break;
				}
			}

			int loc = str.find("//", 0);
			if (loc != std::string::npos) {
				for (int j = loc; j < str.length(); j ++)
					syntaxColor[j] = 2;
			}
			
			for (int i = 0; i < SyntaxTableNum; i ++) {
				for (int j = 0; j < strend; j ++) {
					int loc = str.find(syntaxTable[i], j);
					if (loc != std::string::npos) {
						bool ok = true;
						int len = strlen(syntaxTable[i]);
						if (loc - 1 >= 0 && isalpha(str[loc - 1])) {
							ok = false;
						}
						if (loc + len < strend && isalpha(str[loc + len])) {
							ok = false;
						}

						if (ok) {
							for (int k = loc; k < loc + len; k ++) {
								if (syntaxColor[k] == 0)
									syntaxColor[k] = 1;
							}
							j = loc + len;
						}
					} else break;
				}
			}

			for(unsigned int strptr = 0; strptr < strend; strptr++)
			{
				if (strbuf[strptr] >= 0x20 && strbuf[strptr] < 0x80)
				{
					unsigned int lowbit = (strbuf[strptr] - 0x20) & 0x0F;
					unsigned int highbit = ((strbuf[strptr] - 0x20) >> 4) & 0x0F;
					float ptx = lowbit / 16.0f;
					float pty = 1.0f - highbit / 6.0f;
					const float dtx = 1.0f / 16.0f;
					const float dty = 1.0f /  6.0f;
					float r = red, g = green, b = blue;

					if (isError) {
						r = 1.0;
						g = 0.05;
						b = 0.05;
					} else {
						switch (syntaxColor[strptr]) {
						case 0:
							break;
						case 1:
							r = 0.6;
							g = 0.6;
							b = 1.0;
							break;
						case 2:
							r = 0.6;
							g = 1.0;
							b = 0.6;
							break;
						}
					}
					/*
					if (syntaxColor[strptr] == 0) {
					} else {
					}*/

					glBegin(GL_QUADS);
						glColor4f(r, g, b, upAlpha);
						glTexCoord2f(ptx              , pty       );
						glVertex2i  (ddx              , ddy       );
						glTexCoord2f(ptx + dtx        , pty       );
						glVertex2i  (ddx + fontWidth  , ddy       );
						glColor4f(r, g, b, downAlpha);
						glTexCoord2f(ptx + dtx        , pty - dty );
						glVertex2i  (ddx + fontWidth  , ddy - fontHeight  );
						glTexCoord2f(ptx              , pty - dty );
						glVertex2i  (ddx              , ddy - fontHeight  );
					glEnd();	
				}
				
				ddx += fontWidth;
				if (strbuf[strptr] == 0xA || strbuf[strptr] == 0xC)
				{
					ddx = 0;
					ddy -= fontHeight;
				}
			}
			glPopAttrib();
			glPopMatrix();
		}
};

};

#endif
