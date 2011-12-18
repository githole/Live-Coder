#include "Core.h"
#include "TextEditor.h"
#include "Logger.h"

int main(int argc, char** argv) {
	LiveCoder::Core core;
	int flags = SDL_OPENGL;
	
	int width = 1280;
	int height = 720;
	/*
	int width = 1920/4;
	int height = 1080/4;*/

	if (argc >= 2) {
	  if (strcmp(argv[1], "-f") == 0) {
	    flags = SDL_OPENGL | SDL_FULLSCREEN;
	  }
	}
	if (argc >= 4) {
	  width = atoi(argv[2]);
	  height = atoi(argv[3]);
	}

	if (core.Initialize("Live Coder", width, height, flags) < 0) {
		return -1;
	}
	core.MainLoop();

	return 0;
}
