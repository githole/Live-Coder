#ifndef _KEY_ANALYZER_
#define _KEY_ANALYZER_

#include <SDL.h>

#include "TextEditor.h"

namespace LiveCoder {


class KeyAnalyzer
{
private:
public:
	void KeyHit(TextEditor* textEditor, SDLKey key, SDLMod mod,  const std::string& filename);


	KeyAnalyzer(void);
	~KeyAnalyzer(void);
};
}

#endif