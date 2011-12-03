#ifndef _KEY_ANALYZER_
#define _KEY_ANALYZER_

#include <SDL.h>

#include "TextEditor.h"

namespace LiveCoder {

struct KeyState {
	bool state;
	Uint32 last;
	bool repeating;

	KeyState(void) : state(false), repeating(false), last(0) {}
};

enum KeyAnalyzerSpecialKey {
	KASK_SHIFT = 0,
	KASK_UP,
	KASK_DOWN,
	KASK_LEFT,
	KASK_RIGHT,
	KASK_ENTER,
	KASK_BACKSPACE,
	KASK_TAB,
	KASK_CTRL,
	KASK_HOME,
	KASK_END,

	KASK_NUM
};

class KeyAnalyzer
{
private:
	KeyState state[256];


	KeyState special[KASK_NUM];
	/*
	KeyState shifted;
	KeyState up, down, left, right;
	KeyState enter;
	KeyState backspace;*/
	void StateSet(KeyAnalyzerSpecialKey type, bool value);
public:
	void Set(SDLKey key, bool value);

	void Input(TextEditor* textEditor, const std::string& filename);

	KeyAnalyzer(void);
	~KeyAnalyzer(void);
};
}

#endif