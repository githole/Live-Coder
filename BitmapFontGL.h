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

namespace LiveCoder {


class BitmapFontGL
{
	private:
		unsigned int texName;
		BitmapFontGL(){};
		
	public:
		static BitmapFontGL* Instance() {    
			static BitmapFontGL instance;  // 唯一のインスタンス
    		return &instance;
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
			glTexImage2D(GL_TEXTURE_2D, 0, 3, 112, 66,
				0, GL_LUMINANCE, GL_UNSIGNED_BYTE, lcfont_bw112x66);
			glPopAttrib();
		}

		void DrawCursor(int col, int row, float aspect, float width) {
			unsigned int fontWidth = 7;
			unsigned int fontHeight = 11;
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
							glVertex2i  (ddx              , ddy       );
							glVertex2i  (ddx + w  , ddy       );
							glVertex2i  (ddx + w  , ddy - fontHeight  );
							glVertex2i  (ddx              , ddy - fontHeight  );
					glEnd();	
			glPopAttrib();
			glPopMatrix();
		}

		void DrawLine(const char* strbuf, float aspect, float width, int line, float upAlpha, float downAlpha) {
			unsigned int fontWidth = 7;
			unsigned int fontHeight = 11;
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
			for(unsigned int strptr = 0; strptr < strend; strptr++)
			{
				if (strbuf[strptr] >= 0x20 && strbuf[strptr] < 0x80)
				{
					unsigned int lowbit = (strbuf[strptr] - 0x20) & 0x0F;
					unsigned int highbit = ((strbuf[strptr] - 0x20) >> 4) & 0x0F;
					float ptx = lowbit / 16.0f;
					float pty = 1.0f - highbit / 6.0f;
					float dtx = 1.0f / 16.0f;
					float dty = 1.0f /  6.0f;
				
					glBegin(GL_QUADS);
						glColor4f(1.0, 1.0, 1.0, upAlpha);
						glTexCoord2f(ptx              , pty       );
						glVertex2i  (ddx              , ddy       );
						glTexCoord2f(ptx + dtx        , pty       );
						glVertex2i  (ddx + fontWidth  , ddy       );
						glColor4f(1.0, 1.0, 1.0, downAlpha);
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
		/*
		void DrawString(const char* strbuf, float aspect, float width)
		{
			unsigned int fontWidth = 7;
			unsigned int fontHeight = 11;
			unsigned int ddx = 0;
			unsigned int ddy = 0;
			glPushMatrix();
			glScalef(0.25f * 8.0f / width,
					 0.25f * 8.0f / width * aspect,
					 0.25f * 8.0f / width);
			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			//glEnable(GL_COLOR_MATERIAL);
			//glColor3f(0.4f, 1.0f, 0.4f);

			glBindTexture(GL_TEXTURE_2D, texName);
			unsigned int strend = strlen(strbuf);
			for(unsigned int strptr = 0; strptr < strend; strptr++)
			{
				if (strbuf[strptr] >= 0x20 && strbuf[strptr] < 0x80)
				{
					unsigned int lowbit = (strbuf[strptr] - 0x20) & 0x0F;
					unsigned int highbit = ((strbuf[strptr] - 0x20) >> 4) & 0x0F;
					float ptx = lowbit / 16.0f;
					float pty = 1.0f - highbit / 6.0f;
					float dtx = 1.0f / 16.0f;
					float dty = 1.0f /  6.0f;
				
					glBegin(GL_QUADS);
						glTexCoord2f(ptx              , pty       );
						glVertex2i  (ddx              , ddy       );
						glTexCoord2f(ptx + dtx        , pty       );
						glVertex2i  (ddx + fontWidth  , ddy       );
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
		}*/
};

};

#endif