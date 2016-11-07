#include "Core.h"
#include "TextEditor.h"
#include "Logger.h"

int main(int argc, char** argv) {
	LiveCoder::Core core;
	int flags = SDL_OPENGL;
	int width = 1280;
	int height = 720;
	int maxFrameRate = -1;

	if (argc >= 2) {
		for(int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-f") == 0) {
				flags = SDL_OPENGL | SDL_FULLSCREEN;
			}
			const char* fpsArgumentPrefix = "--fps=";
			if (strncmp(argv[i], fpsArgumentPrefix, strlen(fpsArgumentPrefix)) == 0) {
				maxFrameRate = atoi(argv[i]+strlen(fpsArgumentPrefix));
			}
		}
	}
	if (argc >= 4) {
	  width = atoi(argv[2]);
	  height = atoi(argv[3]);
	}

	if (core.Initialize("Live Coder", width, height, maxFrameRate, flags) < 0) {
		return -1;
	}
	core.MainLoop();

	return 0;
}
