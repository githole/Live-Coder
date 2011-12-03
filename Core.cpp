#include "Core.h"

namespace LiveCoder {
	
const char* EffectFileTable[] = {
	"scene1.glsl",
	"scene2.glsl",
	"scene3.glsl",
	"scene4.glsl",
	"scene5.glsl",
	"scene6.glsl",
	"scene7.glsl",
	"scene8.glsl",
	"scene9.glsl",
	"scene10.glsl",
	"scene11.glsl",
	"effect.glsl",
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
	
	// 適当です
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

	 
#ifdef	__WIN32__
	// GLEW
	if (glewInit() != GLEW_OK) {
		Logger::Instance()->OutputString("Error: glewInit()");
		return -1;
	}
	Logger::Instance()->OutputString("glewInit succeeded");
#endif	__WIN32__

	frameBuffer = 0;
	renderBuffer = 0;
	renderTexture = 0;

	glGenFramebuffers(1, &frameBuffer);
	glGenRenderbuffers(1, &renderBuffer);
	glGenTextures(1, &renderTexture);
	// フレームバッファ
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
	
	// レンダーバッファ
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	//
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	return 0;
}

int Core::ProcessSDLEvents() {
    SDL_Event eve;

	// まずマウス関係のイベントを処理する（なぜかPollEventでこない）
	SDL_PumpEvents();
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	mouseBuffer.SetPosition((float)mouseX / width, (float)mouseY / height);

	keyBuffer.Clear();
    while (SDL_PollEvent(&eve)) {
        switch(eve.type) {
        case SDL_KEYDOWN:
			if (editMode)
				keyAnalyzer.Set(eve.key.keysym.sym, true);
			if (eve.key.keysym.sym == SDLK_ESCAPE)
				return -1;
			if (SDLK_F1 <= eve.key.keysym.sym && eve.key.keysym.sym <= SDLK_F12 && eve.key.keysym.sym != SDLK_F11) {
				nowEffect = eve.key.keysym.sym - SDLK_F1;
				shaderGL[nowEffect].CompileFromFile(EffectFileTable[nowEffect]);
				textEditor.Load(EffectFileTable[nowEffect]);
			}
			if (eve.key.keysym.sym== SDLK_F11) {
				if (editMode)
					editMode = false;
				else
					editMode = true;
			}
            break;
        case SDL_KEYUP:
			keyAnalyzer.Set(eve.key.keysym.sym, false);
            break;
        case SDL_QUIT:
            return -1;
        }

    }

	return 0;
}

void Core::Render() {
	// レンダーターゲット切替
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	std::string str = textEditor.ToString();
	if (nowSource != str) {
		nowSource = str;

		shaderGL[nowEffect].Compile(nowSource);
	}

	Uint32 nowTime = SDL_GetTicks();

	float realSec = (nowTime - baseTime) / 1000.0f;
	
	float low = 0.0;
	float mid = 0.0;
	float high = 0.0;
	float cy = 0.0;
	if (shaderGL[nowEffect].Valid()) {
		shaderGL[nowEffect].Bind();
		shaderGL[nowEffect].SetUniform("resolution", (float)width, (float)height);
		shaderGL[nowEffect].SetUniform("time", realSec);
		shaderGL[nowEffect].SetUniform("mouse", mouseBuffer.GetCursorX(), mouseBuffer.GetCursorY());

		if (audioBuffer != NULL) {
			GLfloat texture[1024];
			for (int i = 0; i < 1024; i ++) {
				texture[i] = audioBuffer[i];
			}
			glEnable(GL_TEXTURE_1D);
			glBindTexture(GL_TEXTURE_1D, audioTexture);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage1D(GL_TEXTURE_1D, 0, 1, 1024, 0 , GL_RED, GL_FLOAT, texture);
			shaderGL[nowEffect].SetUniform("rawAudio", (int)0);
			

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

			/*
			float buf[1024];
			for (int i = 0; i < 1024; i ++)
				buf[i] = (float)audioBuffer[i];
			shaderGL.SetUniform("audio", buf, 1024);*/
		}
		glRecti(1, 1, -1, -1);
		shaderGL[nowEffect].Unbind();
	}
	
	if (editMode) {
		keyAnalyzer.Input(&textEditor, EffectFileTable[nowEffect]);
		glPushMatrix();
		// TextEditor Background
		const float aspect = width/static_cast<float>(height);
		const float textEditorHeight = textEditor.GetMaxLineNum() * 11 * (0.25f * 8.0f / width * aspect);
		const float textEditorBGHeight = textEditorHeight * 1.2f;
		const float editorOffsetY =  -(2.0 - textEditorHeight) / 2.0;
		const float editorBGOffsetY =  -(2.0 - textEditorBGHeight) / 2.0;
		glPushMatrix();
		glTranslatef(-1.0f, 1.0f + editorBGOffsetY, 0);
		glPushAttrib(GL_ENABLE_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
			glColor4f(0.0, 0.0, 0.0, 0.0);
			glVertex2f(0.0, 0.0);
			glVertex2f(0.0 + width, 0.0);
			glColor4f(0.0, 0.0, 0.0, 0.8);
			glVertex2f(0.0 + width, -textEditorBGHeight/6.0);
			glVertex2f(0.0, -textEditorBGHeight/6.0);
		glEnd();	
		glBegin(GL_QUADS);
			glColor4f(0.0, 0.0, 0.0, 0.8);
			glVertex2f(0.0, -textEditorBGHeight/6.0);
			glVertex2f(0.0 + width, - textEditorBGHeight/6.0);
			glColor4f(0.0, 0.0, 0.0, 0.8);
			glVertex2f(0.0 + width,  -5.0*textEditorBGHeight/6.0 );
			glVertex2f(0.0,  -5.0*textEditorBGHeight/6.0  );
		glEnd();	
		glBegin(GL_QUADS);
			glColor4f(0.0, 0.0, 0.0, 0.8);
			glVertex2f(0.0,-5.0*textEditorBGHeight/6.0);
			glVertex2f(0.0 + width  ,  -5.0*textEditorBGHeight/6.0);
			glColor4f(0.0, 0.0, 0.0, 0.0);
			glVertex2f(0.0 + width,  -textEditorBGHeight);
			glVertex2f(0.0, - textEditorBGHeight);
		glEnd();	
		glPopAttrib();
		glPopMatrix();
			
		glTranslatef(-1.0f, 1.0f + editorOffsetY,0);
		TextEditorPtrBuffer ptrbuf = textEditor.GetVisibleText();

		bool upAlpha = true;
		bool downAlpha = true;
	

		if (textEditor.GetLineOffset() == 0) {
			upAlpha = false;
		}

		if (textEditor.GetLineOffset() + textEditor.GetMaxLineNum() >= textEditor.GetLineNum()) {
			downAlpha = false;
		}

		const int transRange = 5;

		for (int i = 0; i < ptrbuf.size(); i ++) {
			float up = 1.0;
			float down = 1.0;

			if (upAlpha) {
				if (i < transRange) {
					up = (float)i/transRange;
					down = (float)(i + 1)/transRange;
				}
			}
			if (downAlpha) {
				if (i >= ptrbuf.size() - transRange) {
					up = 1.0 - (float)(i - ptrbuf.size() + transRange)/transRange;
					down = 1.0 - (float)(i + 1 - ptrbuf.size() + transRange)/transRange;
				}
			}

			BitmapFontGL::Instance()->DrawLine(ptrbuf[i]->c_str(), aspect, width, i, up, down);	
			if (i == textEditor.GetCursorPosition().col)
				cy = height - i * 11 - 11/2.0 + height * editorOffsetY/2.0;
		}
		BitmapFontGL::Instance()->DrawLine(EffectFileTable[nowEffect], aspect, width, textEditor.GetMaxLineNum(), 0.5, 0.5);	

		EditorCursor cursor = textEditor.GetCursorPosition();
		BitmapFontGL::Instance()->DrawCursor(cursor.col, cursor.row, aspect, width);
		glPopMatrix();
	}

	// レンダーターゲット元に戻す
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (shaderGL[EffectNum - 1].Valid()) {
		shaderGL[EffectNum - 1].Bind();
		
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		shaderGL[EffectNum - 1].SetUniform("texture0", (int)0);
		shaderGL[EffectNum - 1].SetUniform("resolution", (float)width, (float)height);
		shaderGL[EffectNum - 1].SetUniform("time", realSec);
		shaderGL[EffectNum - 1].SetUniform("mouse", mouseBuffer.GetCursorX(), mouseBuffer.GetCursorY());
		shaderGL[EffectNum - 1].SetUniform("lowFreq", (float)low);
		shaderGL[EffectNum - 1].SetUniform("midFreq", (float)mid);
		shaderGL[EffectNum - 1].SetUniform("highFreq", (float)high);
		shaderGL[EffectNum - 1].SetUniform("editorCursorY", (float)cy);

		//float cy = (textEditor.GetCursorPosition().col - textEditor.GetLineOffset()) * 11

		glRecti(1, 1, -1, -1);
		glBindTexture(GL_TEXTURE_2D, 0);

		shaderGL[EffectNum - 1].Unbind();
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
//	shaderGL.Compile("void main() { gl_FragColor = vec4(1, 0, 1, 1); }");
	baseTime = SDL_GetTicks();

//	textEditor.SetString("uniform float time;\n\nvoid main(){\n  gl_FragColor=(1.0 - gl_FragCoord.y/540.0) * vec4(0.0,sin(length(vec2(400.0 +\n  100.0*sin(time), 300.0 + 200.0*cos(time)) - gl_FragCoord.xy)/4.0)*\n  sin(length(vec2(300.0 + 200.0*sin(time), 400.0 + 100.0*cos(time/2.0)) - gl_FragCoord.xy)/3.0),0.0,1.0);\n}");
//	textEditor.SetString("uniform float time;\nuniform sampler1D rawAudio;\nvoid main(){vec4 col=texture1D(rawAudio, gl_FragCoord.x/960.0);\n  gl_FragColor=col;}");
//	textEditor.Load("init.glsl");

//	shaderGL[EffectNum - 1].CompileFromFile("effect.glsl");

	/*
	for (int i = 0; i < EffectNum; i ++) {
		shaderGL[i].CompileFromFile(EffectFileTable[i]);
	}*/
	// テキスト用エフェクトだけは事前にコンパイルしておく
	shaderGL[EffectNum - 1].CompileFromFile(EffectFileTable[EffectNum - 1]);
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

Core::Core() : width(0), height(0), end(false), nowEffect(0), editMode(true) {
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
}

};
