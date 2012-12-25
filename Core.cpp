#include "Core.h"

namespace LiveCoder {
	
const int POSTFxID = 11;
const char* EffectFileTable[] = {
	"scene1.fx", // 0
	"scene2.fx",
	"scene3.fx",
	"scene4.fx",
	"scene5.fx",
	"scene6.fx",
	"scene7.fx",
	"scene8.fx",
	"scene9.fx",
	"scene10.fx", // 9
	"non.fx",
	"effect.fx", // 11

	"scene11.fx", // 12
	"scene12.fx", 
	"scene13.fx", 
	"scene14.fx", 
	"scene15.fx", 
	"scene16.fx", 
	"scene17.fx", 
	"scene18.fx", 
	"scene19.fx",
	"scene20.fx",
	"non.fx",
	"non.fx",
	
	"scene21.fx", // 24
	"scene22.fx", 
	"scene23.fx", 
	"scene24.fx", 
	"scene25.fx", 
	"scene26.fx", 
	"scene27.fx", 
	"scene28.fx", 
	"scene29.fx",
	"scene30.fx",
	"non.fx",
	"non.fx",
};

// 初期化
int Core::Initialize(std::string title_ = "Title" , int width_ = DefaultWidth, int height_ = DefaultHeight, int SDLflags = SDL_OPENGL) {
    const SDL_VideoInfo* info = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Logger::Instance()->OutputString("Error: " + std::string(SDL_GetError()));
		return -1;
    }
	Logger::Instance()->OutputString("SDL_Init succeeded");
	
	title = title_;
	SDL_WM_SetCaption(title.c_str(), NULL);
	
    info = SDL_GetVideoInfo( );

    if (!info) {
		Logger::Instance()->OutputString("Error: " + std::string(SDL_GetError()));
		return -1;
    }
	Logger::Instance()->OutputString("SDL_GetVideoInfo succeeded");
	
    bpp = info->vfmt->BitsPerPixel;
	
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	width = width_;
	height = height_;
	
    if (SDL_SetVideoMode(width, height, bpp, SDLflags) == 0) {
		Logger::Instance()->OutputString("Error: " + std::string(SDL_GetError()));
		return -1;
    }
	Logger::Instance()->OutputString("SDL_SetVideoMode succeeded");
	
	// Get Window Handle
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	SDL_GetWMInfo(&wm_info);
	
	if (dxcore.InitializeDX(width, height, wm_info.window)) {
		Logger::Instance()->OutputString("Error: DXCore InitializeDX");
		return -1;
	}

	dxcore.Load();
	BitmapFontDX::Instance()->Load(&dxcore);
	for (int i = 0; i < EffectNum; i ++)
		shaderDX[i].Load(&dxcore);

	 /*
#ifdef	__NEEDSGLEW__
	// GLEW
	if (glewInit() != GLEW_OK) {
		Logger::Instance()->OutputString("Error: glewInit()");
		return -1;
	}
	Logger::Instance()->OutputString("glewInit succeeded");
#endif	__NEEDSGLEW__
	*/

	// Option texture
	glGenTextures(1, &optionTexture);
	SDL_Surface* bmp = SDL_LoadBMP("option.bmp");
	if (bmp != NULL) {
		Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
#endif
		SDL_Surface* tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, bmp->w, bmp->h, 32, rmask, gmask, bmask, amask);
		SDL_Rect rect;
		rect.x = rect.y = 0;
		rect.w = bmp->w; rect.h = bmp->h;
		SDL_BlitSurface(bmp, &rect, tmp, &rect);

		SDL_LockSurface(tmp);

		/*
		d3ddevice->CreateTexture(bmp->w, bmp->h, 1, 0,  D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED, &optionTextureDx, 0);

		D3DLOCKED_RECT locked_rect;
		optionTextureDx->LockRect(0, &locked_rect, NULL, D3DLOCK_DISCARD);
		const int stride = 1; 
		const int rowPitch = locked_rect.Pitch;
		unsigned char* pixels = (unsigned char*)locked_rect.pBits;
		for (int y = 0; y < bmp->h; ++y) {
			for (int x = 0; x < bmp->w; ++x) {
				pixels[x + rowPitch * y] = ((unsigned char*)tmp->pixels)[x + bmp->w * y];
			}
		}
		optionTextureDx->UnlockRect(0);
		*/

		/*
		//glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, optionTexture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->w, bmp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glActiveTexture(GL_TEXTURE0);
		*/

		SDL_UnlockSurface(tmp);

		SDL_FreeSurface(tmp);
		SDL_FreeSurface(bmp);
	} else {
		Logger::Instance()->OutputString("Error: SDL_LoadBMP");
	}

	/*
	frameBuffer = 0;
	renderBuffer = 0;
	renderTexture = 0;
	backTexture = 0;

	glGenFramebuffers(1, &frameBuffer);
	glGenRenderbuffers(1, &renderBuffer);
	glGenTextures(1, &renderTexture);
	glGenTextures(1, &backTexture);

	// Initialize back buffer
	glBindTexture(GL_TEXTURE_2D, backTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Initialize frame buffer
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
	
	// Initialize render buffer
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	// Initialize render texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/



	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EnableUNICODE(true);
	SDL_EnableKeyRepeat(250, 25);

	errorHighlight = true;
	return 0;
}

int Core::ProcessSDLEvents() {
    SDL_Event eve;

	// Process mouse events
	SDL_PumpEvents();
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	mouseBuffer.SetPosition(((float)mouseX / width), 1.0f - (float)mouseY / height);

	keyBuffer.Clear();
    while (SDL_PollEvent(&eve)) {
        switch(eve.type) {
		case SDL_KEYDOWN:
			{
			Uint16 ch = eve.key.keysym.unicode;
			SDLKey key = isprint(ch) ? (SDLKey)ch : eve.key.keysym.sym;

			if (editMode)
				keyAnalyzer.KeyHit(&textEditor, key, eve.key.keysym.mod, EffectFileTable[nowEffect]);
			if (eve.key.keysym.sym == SDLK_ESCAPE)
				return -1;

			int ctrl = eve.key.keysym.mod & KMOD_CTRL;
			int alt = eve.key.keysym.mod & KMOD_ALT;

			if (SDLK_F1 <= eve.key.keysym.sym && eve.key.keysym.sym <= SDLK_F12 && eve.key.keysym.sym != SDLK_F11) {
				nowEffect = eve.key.keysym.sym - SDLK_F1;
				if (alt)
					nowEffect += 12;
				else if (ctrl)
					nowEffect += 24;

				shaderDX[nowEffect].CompileFromFile(EffectFileTable[nowEffect]);
				textEditor.Load(EffectFileTable[nowEffect]);
			}

			if (eve.key.keysym.sym == SDLK_F11) {
				if (editMode)
					editMode = false;
				else
					editMode = true;
			}

			if (eve.key.keysym.mod & KMOD_CTRL && eve.key.keysym.sym == SDLK_TAB) {
				errorHighlight = !errorHighlight;
			}
			if (eve.key.keysym.mod & KMOD_CTRL && eve.key.keysym.sym == SDLK_SPACE) {
				static bool cursor = true;
				cursor = !cursor;
				SDL_ShowCursor(cursor);
			}

			}
			break;

        case SDL_QUIT:
            return -1;
        }

    }

	return 0;
}

void Core::RenderEditorAndScene() {
	dxcore.Device()->SetRenderTarget(0, dxcore.RenderSurface());
	dxcore.Device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);
	
	// シェーダーコンパイル
	std::string str = textEditor.ToString();
	if (nowSource != str) {
		nowSource = str;

		if (shaderDX[nowEffect].Compile(nowSource) != 0)
			nowCompiled = true;
		else
			nowCompiled = false;
	}

	Uint32 nowTime = SDL_GetTicks();

	float realSec = (nowTime - baseTime) / 1000.0f;
	
	float low = 0.0f;
	float mid = 0.0f;
	float high = 0.0f;
	float cy = -9999.0f;

	if (shaderDX[nowEffect].Valid()) {
		shaderDX[nowEffect].Bind();
		shaderDX[nowEffect].SetUniform(0, D3DXVECTOR4(width, height, 0.0f, 0.0f));
		shaderDX[nowEffect].SetUniform(1, D3DXVECTOR4(realSec, 0.0f, 0.0f, 0.0f));
		shaderDX[nowEffect].SetUniform(2, D3DXVECTOR4(mouseBuffer.GetCursorX(), mouseBuffer.GetCursorY(), 0.0f, 0.0f));
		
		if (audioBuffer != NULL) {
			int lowband = (int)floor(500.0 * 1024.0 / 44100.0 + 0.5);
			int midband = (int)floor(5000.0 * 1024.0 / 44100.0 + 0.5);

			for (int i = 0; i < lowband; i ++)
			  low += audioBuffer[i];
			for (int i = lowband; i < midband; i ++)
			  mid += audioBuffer[i];
			for (int i = midband; i < 1024;i ++)
			  high += audioBuffer[i];

			//- 500 low
			// 500 - 5000 mid
			// 5000 - high

			low /= lowband;
			mid /= (midband - lowband);
			high /= (1024 - midband);

			shaderDX[nowEffect].SetUniform(3, D3DXVECTOR4((float)low, 0.0f, 0.0f, 0.0f));
			shaderDX[nowEffect].SetUniform(4, D3DXVECTOR4((float)mid, 0.0f, 0.0f, 0.0f));
			shaderDX[nowEffect].SetUniform(5, D3DXVECTOR4((float)high, 0.0f, 0.0f, 0.0f));
		}

		// render shader
		dxcore.Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		struct VERTEX{
			float x,y,z,w;
		};

		const VERTEX vertex1[] = {
			{0.0f , 0.0f, 0.0f, 1.0f},
			{width, 0.0f, 0.0f, 1.0f},
			{width, height, 0.0f, 1.0f},
			{0.0f , height, 0.0f, 1.0f},
		};

		dxcore.Device()->SetFVF(D3DFVF_XYZRHW);
		dxcore.Device()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex1, sizeof(VERTEX));

		shaderDX[nowEffect].Unbind();
	}

	const int fontWidth = 10;
	const int fontHeight = 15;
	BitmapFontDX::Instance()->SetFontSize(fontWidth, fontHeight);
	
	if (editMode) {
		//keyAnalyzer.Input(&textEditor, EffectFileTable[nowEffect]);
		// TextEditor Background
		const float aspect = width/static_cast<float>(height);
		const float textEditorHeight = textEditor.GetMaxLineNum() * fontHeight;
		const float textEditorBGHeight = textEditorHeight * 1.2f;
		const float editorOffsetY = (height - textEditorHeight) / 2.0f;
		const float editorBGOffsetY =  (height - textEditorBGHeight) / 2.0f;
		
		// render editor background
		dxcore.Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		dxcore.Device()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		dxcore.Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		dxcore.Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		struct VERTEX{
			float x,y,z,w;
			D3DCOLOR color;
		};

		const VERTEX vertex1[] = {
			{0.0f , editorBGOffsetY, 0.0f, 1.0f, 0x00000000},
			{width, editorBGOffsetY, 0.0f, 1.0f, 0x00000000},
			{width, editorBGOffsetY + textEditorBGHeight/5.0f, 0.0f, 1.0f, 0xE0000000},
			{0.0f , editorBGOffsetY + textEditorBGHeight/5.0f, 0.0f, 1.0f, 0xE0000000},
		};
		const VERTEX vertex2[] = {
			{0.0f , editorBGOffsetY + textEditorBGHeight/5.0f, 0.0f, 1.0f, 0xE0000000},
			{width, editorBGOffsetY + textEditorBGHeight/5.0f, 0.0f, 1.0f, 0xE0000000},
			{width, editorBGOffsetY + textEditorBGHeight*4.0f/5.0f, 0.0f, 1.0f, 0xE0000000},
			{0.0f , editorBGOffsetY +  textEditorBGHeight*4.0f/5.0f, 0.0f, 1.0f, 0xE0000000},
		};
		const VERTEX vertex3[] = {
			{0.0f , editorBGOffsetY + textEditorBGHeight*4.0f/5.0f, 0.0f, 1.0f, 0xE0000000},
			{width, editorBGOffsetY + textEditorBGHeight*4.0f/5.0f, 0.0f, 1.0f, 0xE0000000},
			{width, editorBGOffsetY + textEditorBGHeight, 0.0f, 1.0f, 0x00000000},
			{0.0f , editorBGOffsetY + textEditorBGHeight, 0.0f, 1.0f, 0x00000000},
		};

		dxcore.Device()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		dxcore.Device()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex1, sizeof(VERTEX));
		dxcore.Device()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex2, sizeof(VERTEX));
		dxcore.Device()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex3, sizeof(VERTEX));

		// render editor
		BitmapFontDX::Instance()->SetOffset(0.0f, editorOffsetY);
		
		TextEditorPtrBuffer ptrbuf = textEditor.GetVisibleText();
		TextEditorPtrBuffer textbuf = textEditor.GetText();

		bool upAlpha = true;
		bool downAlpha = true;
	
		if (textEditor.GetLineOffset() == 0) {
			upAlpha = false;
		}
		 
		if (textEditor.GetLineOffset() + textEditor.GetMaxLineNum() >= textEditor.GetLineNum()) {
			downAlpha = false;
		}

		const int transRange = 5;
		BitmapFontDX::Instance()->Reset();
		for (int i = 0; i < textEditor.GetLineOffset(); i ++) {
			BitmapFontDX::Instance()->ProcessComment(*textbuf[i]);
		}

		const std::set<int>& errorLinesFS = shaderDX[nowEffect].GetErrorLinesFS();

		for (int i = 0; i < ptrbuf.size(); i ++) {
			float up = 1.0f;
			float down = 1.0f;

			if (upAlpha) {
				if (i < transRange) {
					up = (float)i/transRange;
					down = (float)(i + 1)/transRange;
				}
			}
			if (downAlpha) {
				if (i >= ptrbuf.size() - transRange) {
					up = 1.0f - (float)(i - ptrbuf.size() + transRange)/transRange;
					down = 1.0f - (float)(i + 1 - ptrbuf.size() + transRange)/transRange;
				}
			}

			bool errorLine = false;
			if (errorLinesFS.find(i + textEditor.GetLineOffset() + 1) != errorLinesFS.end())
				errorLine = true;
			if (errorHighlight == false)
				errorLine = false;

			BitmapFontDX::Instance()->DrawLine(ptrbuf[i]->c_str(), aspect, width, i, up, down, errorLine);	
			if (i == textEditor.GetCursorPosition().col)
				cy = i * fontHeight + fontHeight/2.0f + editorOffsetY + 0.5;
			
		}
		
		if (nowCompiled)
			BitmapFontDX::Instance()->DrawLine(EffectFileTable[nowEffect], aspect, width, textEditor.GetMaxLineNum(), 0.5f, 0.5f);	
		else
			BitmapFontDX::Instance()->DrawLine(EffectFileTable[nowEffect], aspect, width, textEditor.GetMaxLineNum(), 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);	

		BitmapFontDX::Instance()->DrawLine("F1-F10: Change File  F11: Show/Hide code  F12: Edit PostFx", aspect, width, textEditor.GetMaxLineNum() + 1, 0.5f, 0.5f);
		
		// render editor cursor
		EditorCursor cursor = textEditor.GetCursorPosition();
		BitmapFontDX::Instance()->DrawCursor(cursor.col, cursor.row, aspect, width);

		if (textEditor.IsSelectMode()) {
			EditorCursor selectStart = textEditor.GetSelectStart();
			if (selectStart.col > cursor.col)
				std::swap(selectStart, cursor);

			if (selectStart.col == cursor.col) {
				int start = selectStart.row;
				int end = cursor.row;


				BitmapFontDX::Instance()->DrawSelect(aspect, width, selectStart.col, start, end, 0.5f, 0.5f, 0.5f);
			} else {
				for (int i = selectStart.col; i <= cursor.col; i ++) {
					if (i < 0 || i >= textEditor.GetMaxLineNum())
						continue;

					if (i == selectStart.col) {
						BitmapFontDX::Instance()->DrawSelect(aspect, width, i, selectStart.row, textEditor.GetLineLength(i));
					} else if (i < cursor.col) {
						BitmapFontDX::Instance()->DrawSelect(aspect, width, i, 0, textEditor.GetLineLength(i));
					} else if (i == cursor.col) {
						BitmapFontDX::Instance()->DrawSelect(aspect, width, i, 0, cursor.row);
					}
				}
			}
		}
	}

	dxcore.Device()->SetRenderTarget(0, dxcore.BackbufferSurface());
	dxcore.Device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x000000FF, 1.0f, 0);

	// backbuffer -> front
	{
		float offset = -0.5f;
		if (shaderDX[POSTFxID].Valid()) {
			offset = 0.0f;
			shaderDX[POSTFxID].Bind();
			
			shaderDX[nowEffect].SetUniform(0, D3DXVECTOR4(width, height, 0.0f, 0.0f));
			shaderDX[nowEffect].SetUniform(1, D3DXVECTOR4(realSec, 0.0f, 0.0f, 0.0f));
			shaderDX[nowEffect].SetUniform(2, D3DXVECTOR4(mouseBuffer.GetCursorX(), mouseBuffer.GetCursorY(), 0.0f, 0.0f));
			shaderDX[nowEffect].SetUniform(3, D3DXVECTOR4((float)low, 0.0f, 0.0f, 0.0f));
			shaderDX[nowEffect].SetUniform(4, D3DXVECTOR4((float)mid, 0.0f, 0.0f, 0.0f));
			shaderDX[nowEffect].SetUniform(5, D3DXVECTOR4((float)high, 0.0f, 0.0f, 0.0f));
			shaderDX[nowEffect].SetUniform(6, D3DXVECTOR4((float)cy, 0.0f, 0.0f, 0.0f));

		}
	
		dxcore.Device()->SetTexture(0, dxcore.RenderTexture());
		dxcore.Device()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		dxcore.Device()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		dxcore.Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		struct VERTEX{
			float x,y,z,w;
			D3DCOLOR color;
			float u, v;
		};

		const VERTEX vertex1[] = {
			{offset, offset, 0.0f, 1.0f, 0xFFFFFFFF, 0.0f, 0.0f},
			{offset + width, offset, 0.0f, 1.0f, 0xFFFFFFFF, 1.0f, 0.0f},
			{offset + width, offset + height, 0.0f, 1.0f, 0xFFFFFFFF, 1.0f, 1.0f},
			{offset, offset + height, 0.0f, 1.0f, 0xFFFFFFFF, 0.0f, 1.0f},
		};

		dxcore.Device()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		dxcore.Device()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex1, sizeof(VERTEX));
		dxcore.Device()->SetTexture(0, NULL);
	
		if (shaderDX[POSTFxID].Valid()) {
			shaderDX[POSTFxID].Unbind();
		}
	}
}

void Core::Render() {
	
    HRESULT hr = E_FAIL;
	hr = dxcore.Device()->TestCooperativeLevel();
    if(hr == D3DERR_DEVICELOST) {
		// うわぁぁぁ
    } else {
		if(hr == D3DERR_DEVICENOTRESET) {
			BitmapFontDX::Instance()->Unload();
			for (int i = 0; i < EffectNum; i ++)
				shaderDX[i].Unload();
			dxcore.Unload();
			dxcore.ResetDevice();
			dxcore.Load();
			for (int i = 0; i < EffectNum; i ++)
				shaderDX[i].Load(&dxcore);
			BitmapFontDX::Instance()->Load(&dxcore);
		}
		if(dxcore.Device()->BeginScene() == D3D_OK) {
			this->RenderEditorAndScene();

			dxcore.Device()->EndScene();
		}
		dxcore.Device()->Present(NULL, NULL, NULL, NULL);
	}
}

int Core::MainLoop() {
	Logger::Instance()->OutputString("MainLoop...");
	baseTime = SDL_GetTicks();

	// precompile the effect for editor
	shaderDX[POSTFxID].CompileFromFile(EffectFileTable[POSTFxID]);
	textEditor.Load(EffectFileTable[0]);
	
	glGenTextures(1 , &audioTexture);
	while (!end) {
		if (ProcessSDLEvents() < 0)
			break;

		audioBuffer = audioAnalyzer->Capture();
		
		Render();
	}

	Logger::Instance()->OutputString("End");

	return 0;
}

Core::Core() : width(0), height(0), end(false), nowEffect(0), editMode(true), nowCompiled(false)
{
	// OpenAL
	audioAnalyzer = new AudioAnalyzer(44100, 1024);
}

Core::~Core() {
	delete audioAnalyzer;

}

};
