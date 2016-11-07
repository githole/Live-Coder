#include "Core.h"

namespace LiveCoder {
	
const int POSTFxID = 11;
const char* EffectFileTable[] = {
	"scene1.glsl", // 0
	"scene2.glsl",
	"scene3.glsl",
	"scene4.glsl",
	"scene5.glsl",
	"scene6.glsl",
	"scene7.glsl",
	"scene8.glsl",
	"scene9.glsl",
	"scene10.glsl", // 9
	"non.glsl",
	"effect.glsl", // 11

	"scene11.glsl", // 12
	"scene12.glsl", 
	"scene13.glsl", 
	"scene14.glsl", 
	"scene15.glsl", 
	"scene16.glsl", 
	"scene17.glsl", 
	"scene18.glsl", 
	"scene19.glsl",
	"scene20.glsl",
	"non.glsl",
	"non.glsl",
	
	"scene21.glsl", // 24
	"scene22.glsl", 
	"scene23.glsl", 
	"scene24.glsl", 
	"scene25.glsl", 
	"scene26.glsl", 
	"scene27.glsl", 
	"scene28.glsl", 
	"scene29.glsl",
	"scene30.glsl",
	"non.glsl",
	"non.glsl",
};

// èâä˙âª
int Core::Initialize(std::string title_ = "Title" , int width_ = DefaultWidth, int height_ = DefaultHeight, Uint32 maxFrameRate = DefaultMaxFrameRate, int SDLflags = SDL_OPENGL) {
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
	if (maxFrameRate == -1) {
		minFrameTime = -1;
	} else {
		minFrameTime = 1000.0f/maxFrameRate;
	}
	
    if (SDL_SetVideoMode(width, height, bpp, SDLflags) == 0) {
		Logger::Instance()->OutputString("Error: " + std::string(SDL_GetError()));
		return -1;
    }
	Logger::Instance()->OutputString("SDL_SetVideoMode succeeded");

	 
#ifdef	__NEEDSGLEW__
	// GLEW
	if (glewInit() != GLEW_OK) {
		Logger::Instance()->OutputString("Error: glewInit()");
		return -1;
	}
	Logger::Instance()->OutputString("glewInit succeeded");
#endif	__NEEDSGLEW__

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
		//glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, optionTexture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->w, bmp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glActiveTexture(GL_TEXTURE0);
		SDL_UnlockSurface(tmp);

		SDL_FreeSurface(tmp);
		SDL_FreeSurface(bmp);
	} else {
		Logger::Instance()->OutputString("Error: SDL_LoadBMP");
	}

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

				shaderGL[nowEffect].CompileFromFile(EffectFileTable[nowEffect]);
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

void Core::Render() {
	// Swap render target
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	std::string str = textEditor.ToString();
	if (nowSource != str) {
		nowSource = str;

		if (shaderGL[nowEffect].Compile(nowSource) != 0)
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

	const int fontWidth = 10;
	const int fontHeight = 15;
	BitmapFontGL::Instance()->SetFontSize(fontWidth, fontHeight);

	if (shaderGL[nowEffect].Valid()) {
		shaderGL[nowEffect].Bind();
		shaderGL[nowEffect].SetUniform("resolution", (float)width, (float)height);
		shaderGL[nowEffect].SetUniform("time", realSec);
		shaderGL[nowEffect].SetUniform("mouse", mouseBuffer.GetCursorX(), mouseBuffer.GetCursorY());
		
		glEnable(GL_TEXTURE_2D); 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, optionTexture);
		shaderGL[nowEffect].SetUniform("optTex", (int)1);
		
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, backTexture);
		shaderGL[nowEffect].SetUniform("backbuffer", (int)2);
		

		// Calculate low, mid, high freq.
		if (audioBuffer != NULL) {
			GLfloat texture[1024];
			for (int i = 0; i < 1024; i ++) {
				texture[i] = audioBuffer[i];
			}
			
			glEnable(GL_TEXTURE_1D);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_1D, audioTexture);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage1D(GL_TEXTURE_1D, 0, 1, 1024, 0 , GL_RED, GL_FLOAT, texture);
			shaderGL[nowEffect].SetUniform("fft", (int)0);
			
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

			shaderGL[nowEffect].SetUniform("lowFreq", (float)low);
			shaderGL[nowEffect].SetUniform("midFreq", (float)mid);
			shaderGL[nowEffect].SetUniform("highFreq", (float)high);
		}
		glRecti(1, 1, -1, -1);
		shaderGL[nowEffect].Unbind();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE0);
	}

	// Copy render texture to back buffer texture
	glBindTexture(GL_TEXTURE_2D, backTexture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	if (editMode) {
		//keyAnalyzer.Input(&textEditor, EffectFileTable[nowEffect]);
		glPushMatrix();
		// TextEditor Background
		const float aspect = width/static_cast<float>(height);
		const float textEditorHeight = textEditor.GetMaxLineNum() * fontHeight * (0.25f * 8.0f / width * aspect);
		const float textEditorBGHeight = textEditorHeight * 1.2f;
		const float editorOffsetY =  -(2.0f - textEditorHeight) / 2.0f;
		const float editorBGOffsetY =  -(2.0f - textEditorBGHeight) / 2.0f;
		glPushMatrix();
		glTranslatef(-1.0f, 1.0f + editorBGOffsetY, 0.f);
		glPushAttrib(GL_ENABLE_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
		// render editor background
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
			glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(0.0f + width, 0.0f);
			glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
			glVertex2f(0.0f + width, -textEditorBGHeight/5.0f);
			glVertex2f(0.0f, -textEditorBGHeight/5.0f);
		glEnd();	
		glBegin(GL_QUADS);
			glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
			glVertex2f(0.0f, -textEditorBGHeight/5.0f);
			glVertex2f(0.0f + width, - textEditorBGHeight/5.0f);
			glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
			glVertex2f(0.0f + width,  -4.0f*textEditorBGHeight/5.0f );
			glVertex2f(0.0f,  -4.0f*textEditorBGHeight/5.0f  );
		glEnd();	
		glBegin(GL_QUADS);
			glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
			glVertex2f(0.0f,-4.0f*textEditorBGHeight/5.0f);
			glVertex2f(0.0f + width  ,  -4.0f*textEditorBGHeight/5.0f);
			glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
			glVertex2f(0.0f + width,  -textEditorBGHeight);
			glVertex2f(0.0f, - textEditorBGHeight);
		glEnd();	
		glPopAttrib();
		glPopMatrix();

			
		// render editor
		glTranslatef(-1.0f, 1.0f + editorOffsetY,0.f);
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
		BitmapFontGL::Instance()->Reset();
		for (int i = 0; i < textEditor.GetLineOffset(); i ++) {
			BitmapFontGL::Instance()->ProcessComment(*textbuf[i]);
		}

		std::set<int> errorLinesFS = shaderGL[nowEffect].GetErrorLinesFS();

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

			BitmapFontGL::Instance()->DrawLine(ptrbuf[i]->c_str(), aspect, width, i, up, down, errorLine);	
			if (i == textEditor.GetCursorPosition().col)
				cy = height - i * fontHeight - fontHeight/2.0f + height * editorOffsetY/2.0f;
		}
		

		if (nowCompiled)
			BitmapFontGL::Instance()->DrawLine(EffectFileTable[nowEffect], aspect, width, textEditor.GetMaxLineNum(), 0.5f, 0.5f);	
		else
			BitmapFontGL::Instance()->DrawLine(EffectFileTable[nowEffect], aspect, width, textEditor.GetMaxLineNum(), 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);	

		BitmapFontGL::Instance()->DrawLine("F1-F10: Change File  F11: Show/Hide code  F12: Edit PostFx", aspect, width, textEditor.GetMaxLineNum() + 1, 0.5f, 0.5f);
		
		// render editor cursor
		EditorCursor cursor = textEditor.GetCursorPosition();
		BitmapFontGL::Instance()->DrawCursor(cursor.col, cursor.row, aspect, width);

		if (textEditor.IsSelectMode()) {
			EditorCursor selectStart = textEditor.GetSelectStart();
			if (selectStart.col > cursor.col)
				std::swap(selectStart, cursor);

			if (selectStart.col == cursor.col) {
				int start = selectStart.row;
				int end = cursor.row;


				BitmapFontGL::Instance()->DrawSelect(aspect, width, selectStart.col, start, end, 0.5f, 0.5f, 0.5f);
			} else {
				for (int i = selectStart.col; i <= cursor.col; i ++) {
					if (i < 0 || i >= textEditor.GetMaxLineNum())
						continue;

					if (i == selectStart.col) {
						BitmapFontGL::Instance()->DrawSelect(aspect, width, i, selectStart.row, textEditor.GetLineLength(i));
					} else if (i < cursor.col) {
						BitmapFontGL::Instance()->DrawSelect(aspect, width, i, 0, textEditor.GetLineLength(i));
					} else if (i == cursor.col) {
						BitmapFontGL::Instance()->DrawSelect(aspect, width, i, 0, cursor.row);
					}
				}
			}
		}
//		BitmapFontGL::Instance()->DrawSelect(aspect, width, 5, 1, 9);

		glPopMatrix();
	}

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (shaderGL[POSTFxID].Valid()) {
		shaderGL[POSTFxID].Bind();
		
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		shaderGL[POSTFxID].SetUniform("texture0", (int)0);
		shaderGL[POSTFxID].SetUniform("resolution", (float)width, (float)height);
		shaderGL[POSTFxID].SetUniform("time", realSec);
		shaderGL[POSTFxID].SetUniform("mouse", mouseBuffer.GetCursorX(), mouseBuffer.GetCursorY());
		shaderGL[POSTFxID].SetUniform("lowFreq", (float)low);
		shaderGL[POSTFxID].SetUniform("midFreq", (float)mid);
		shaderGL[POSTFxID].SetUniform("highFreq", (float)high);
		shaderGL[POSTFxID].SetUniform("editorCursorY", (float)cy);

		//float cy = (textEditor.GetCursorPosition().col - textEditor.GetLineOffset()) * 11

		glRecti(1, 1, -1, -1);
		glBindTexture(GL_TEXTURE_2D, 0);

		shaderGL[POSTFxID].Unbind();
	} else {
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		glRecti(1, 1, -1, -1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	SDL_GL_SwapBuffers();
}

int Core::MainLoop() {
	
	Logger::Instance()->OutputString("MainLoop...");

	BitmapFontGL::Instance()->CreateTexture();
	baseTime = SDL_GetTicks();

	// precompile the effect for editor
	shaderGL[POSTFxID].CompileFromFile(EffectFileTable[POSTFxID]);
	textEditor.Load(EffectFileTable[0]);
	
	glGenTextures(1 , &audioTexture);
	while (!end) {
		Uint32 frameStartTime = SDL_GetTicks();

		if (ProcessSDLEvents() < 0)
			break;

		audioBuffer = audioAnalyzer->Capture();
		
		Render();

		Uint32 frameEndTime = SDL_GetTicks();
		Uint32 frameDeltaTime = SDL_GetTicks() - frameStartTime;
		if (minFrameTime != -1 && frameDeltaTime <= minFrameTime) {
			SDL_Delay(minFrameTime - frameDeltaTime);
		}
	}

	Logger::Instance()->OutputString("End");

	return 0;
}

Core::Core() : width(0), height(0), end(false), nowEffect(0), editMode(true), nowCompiled(false) {
	// OpenAL
	audioAnalyzer = new AudioAnalyzer(44100, 1024);
}

Core::~Core() {
	delete audioAnalyzer;

	if (frameBuffer != 0)
	    glDeleteFramebuffers(1, &frameBuffer);
	if (renderBuffer != 0)
		glDeleteRenderbuffers(1, &renderBuffer);
	if (renderTexture != 0)
		glDeleteTextures(1, &renderTexture);
	if (backTexture != 0)
		glDeleteTextures(1, &backTexture);
}

};
