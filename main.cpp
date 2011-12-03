#include "Core.h"
#include "TextEditor.h"

int main(int argc, char** argv) {
	LiveCoder::Core core;

	
	if (core.Initialize("Live Coder", 1920/2, 1080/2, SDL_OPENGL) < 0) {
		return -1;
	}
	/*
	if (core.Initialize("Live Coder", 1920, 1200, SDL_OPENGL | SDL_FULLSCREEN) < 0) {
		return -1;
	}*/
	core.MainLoop();

	return 0;
}
