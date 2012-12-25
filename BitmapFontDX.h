#ifndef _BITMAP_FONT_DX_
#define _BITMAP_FONT_DX_

#include <d3d9.h>
#include <string>
#include <iostream>
#include <vector>

#include "inconsolata_160x90.h"

namespace LiveCoder {

	
#define SyntaxTableNum 24
static const char* syntaxTableHLSL[] = {
	"break", "continue", "if", "else", "switch", "return", "for", "while", "do", "typedef", "namespace", "true", "false", "compile",

	"const", "void", "struct", "static", "extern", "register", "volatile", "inline", "target", "nointerpolation",
	
};

class DXCore;

class BitmapFontDX
{
	private:
		BitmapFontDX(){};
		
		bool IsComment;

		int fontWidth;
		int fontHeight;

		LPDIRECT3DTEXTURE9 texture;
		DXCore* dxcore;

		float offsetX, offsetY;
	public:
		static BitmapFontDX* Instance() {    
			static BitmapFontDX instance;  // 唯一のインスタンス
    		return &instance;
		}

		void SetFontSize(int width, int height) {
			fontWidth = width;
			fontHeight = height;
		}
		void SetOffset(float x, float y) {

			offsetX = x;
			offsetY = y;
		}

		virtual ~BitmapFontDX() {
			Unload();
		}

		void Unload() {
			RELEASE(texture);
		}

		void Load(DXCore* dxcore_)
		{
			dxcore = dxcore_;
			dxcore->Device()->CreateTexture(160, 90, 1, 0,  D3DFMT_L8, D3DPOOL_MANAGED, &texture, 0);

			// デフォルト
			SetFontSize(10, 5);
			SetOffset(0.0f, 0.0f);

			D3DLOCKED_RECT rect;
			if (texture->LockRect(0, &rect, NULL, 0) == D3D_OK) {
				const unsigned char* ps = inconsolata_160x90;
				unsigned char* pd = static_cast<unsigned char*>(rect.pBits);
				const DWORD pitch = rect.Pitch;
				for (int y = 0; y < 90; ++y) {
					for (int x = 0; x < 160; ++x) {
						pd[x] = ps[x];
					}
					pd += pitch;
					ps += 160;
				}
				texture->UnlockRect(0);
			}
		}

		void DrawCursor(int col, int row, float aspect, float width) {
			unsigned int ddx = row * fontWidth;
			unsigned int ddy = col * fontHeight;
			unsigned int w = 2;
			
			dxcore->Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			dxcore->Device()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			dxcore->Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
			dxcore->Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);

			struct VERTEX{
				float x,y,z,w;
				D3DCOLOR color;
			};

			const VERTEX vertex[] = {
				{0.5f + offsetX + ddx    , 0.5f + offsetY + ddy             , 0.0f, 1.0f, 0xffffffff},
				{0.5f + offsetX + ddx + w, 0.5f + offsetY + ddy             , 0.0f, 1.0f, 0xffffffff},
				{0.5f + offsetX + ddx + w, 0.5f + offsetY + ddy + fontHeight, 0.0f, 1.0f, 0xffffffff},
				{0.5f + offsetX + ddx    , 0.5f + offsetY + ddy + fontHeight, 0.0f, 1.0f, 0xffffffff},
			};

			dxcore->Device()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			dxcore->Device()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex, sizeof(VERTEX));
		}

		void DrawSelect(float aspect, float width, int line, int start, int end, float red = 1.0f, float green = 1.0f, float blue = 1.0f) {
			unsigned int ddx = start * fontWidth;
			unsigned int ddy = line * fontHeight;
			unsigned int w = 4 + (end - start) * fontWidth;
			
			dxcore->Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			dxcore->Device()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			dxcore->Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
			dxcore->Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);

			struct VERTEX{
				float x,y,z,w;
				D3DCOLOR color;
			};

			const VERTEX vertex[] = {
				{0.5f + offsetX + ddx    , 0.5f + offsetY + ddy             , 0.0f, 1.0f, 0xffffffff},
				{0.5f + offsetX + ddx + w, 0.5f + offsetY + ddy             , 0.0f, 1.0f, 0xffffffff},
				{0.5f + offsetX + ddx + w, 0.5f + offsetY + ddy + fontHeight, 0.0f, 1.0f, 0xffffffff},
				{0.5f + offsetX + ddx    , 0.5f + offsetY + ddy + fontHeight, 0.0f, 1.0f, 0xffffffff},
			};

			dxcore->Device()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			dxcore->Device()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex, sizeof(VERTEX));
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

		D3DCOLOR GetColor(float r, float g, float b, float a) {

			unsigned char ur = 255 * r;
			unsigned char ug = 255 * g;
			unsigned char ub = 255 * b;
			unsigned char ua = 255 * a;

			return (ua << 24) + (ur << 16) + (ug << 8) + ub;
		}

		void DrawLine(const char* strbuf, float aspect, float width, int line, float upAlpha, float downAlpha, bool isError = false, float red = 1.0f, float green = 1.0f, float blue = 1.0f) {
			unsigned int ddx = 0;
			unsigned int ddy = fontHeight * line;

			dxcore->Device()->SetTexture(0, texture);

			dxcore->Device()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			dxcore->Device()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

			dxcore->Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			dxcore->Device()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			dxcore->Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			dxcore->Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			
			dxcore->Device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			dxcore->Device()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			dxcore->Device()->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			dxcore->Device()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
			dxcore->Device()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
			dxcore->Device()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);


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
					int loc = str.find(syntaxTableHLSL[i], j);
					if (loc != std::string::npos) {
						bool ok = true;
						int len = strlen(syntaxTableHLSL[i]);
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
					
					struct VERTEX{
						float x,y,z,w;
						D3DCOLOR color;
						float u,v;
					};

					const VERTEX vertex[] = {
						{ 0.5f + offsetX + ddx          , 0.5f + offsetY + ddy             , 0.f, 1.0f, GetColor(r, g, b, upAlpha), ptx      , pty },
						{ 0.5f + offsetX + ddx+fontWidth, 0.5f + offsetY + ddy             , 0.f, 1.0f, GetColor(r, g, b, upAlpha), ptx + dtx, pty },
						{ 0.5f + offsetX + ddx+fontWidth, 0.5f + offsetY + ddy + fontHeight, 0.f, 1.0f, GetColor(r, g, b, downAlpha), ptx + dtx, (pty - dty) },

						{ 0.5f + offsetX + ddx          , 0.5f + offsetY + ddy             , 0.f, 1.0f, GetColor(r, g, b, upAlpha), ptx      , pty },
						{ 0.5f + offsetX + ddx+fontWidth, 0.5f + offsetY + ddy + fontHeight, 0.f, 1.0f, GetColor(r, g, b, downAlpha), ptx + dtx, (pty - dty) },
						{ 0.5f + offsetX + ddx          , 0.5f + offsetY + ddy + fontHeight, 0.f, 1.0f, GetColor(r, g, b, downAlpha), ptx      , (pty - dty) },
					};

					dxcore->Device()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
					dxcore->Device()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, vertex, sizeof(VERTEX));
				}
				
				ddx += fontWidth;
				if (strbuf[strptr] == 0xA || strbuf[strptr] == 0xC)
				{
					ddx = 0;
					ddy -= fontHeight;
				}
			}
			dxcore->Device()->SetTexture(0, NULL);
		}

};

};

#endif
