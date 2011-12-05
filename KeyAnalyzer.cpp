#include "KeyAnalyzer.h"

namespace LiveCoder {

void KeyAnalyzer::KeyHit(TextEditor* textEditor, SDLKey key, SDLMod mod,  const std::string& filename) {
	
	if (32 <= key && key <= 126) {
		if (mod & KMOD_CTRL) {
			if (key == 'p' || key == 'P')
				textEditor->MoveCursor(ECMT_UP);
			else if (key == 'n' || key == 'N')
				textEditor->MoveCursor(ECMT_DOWN);
			else if (key == 'b' || key == 'B')
				textEditor->MoveCursor(ECMT_LEFT);
			else if (key == 'f' || key == 'F')
				textEditor->MoveCursor(ECMT_RIGHT);
			else if (key == '/')
				textEditor->Undo();
			else if (key == '.')
				textEditor->Redo();
			else if (key == 's' || key == 'S')
				textEditor->Save(filename);
			else if (key == 'o' || key == 'O')
				textEditor->Load(filename);
			else if (key == 'a' || key == 'A')
				textEditor->Home();
			else if (key == 'e' || key == 'E')
				textEditor->End();
		} else {
			textEditor->InsertCharacter(key);
		}
	} else {
		switch (key) {
		case SDLK_UP:
			textEditor->MoveCursor(ECMT_UP);
			break;
		case SDLK_DOWN:
			textEditor->MoveCursor(ECMT_DOWN);
			break;
		case SDLK_LEFT:
			textEditor->MoveCursor(ECMT_LEFT);
			break;
		case SDLK_RIGHT:
			textEditor->MoveCursor(ECMT_RIGHT);
			break;

		case SDLK_HOME:
			textEditor->Home();
			break;
		case SDLK_END:
			textEditor->End();
			break;
		case SDLK_RETURN:
			textEditor->InsertCharacter('\n');
			break;
		case SDLK_BACKSPACE:
			textEditor->Backspace();
			break;
		case SDLK_TAB:
			textEditor->InsertCharacter(' ');
			textEditor->InsertCharacter(' ');
			break;
		}
	}
}


KeyAnalyzer::KeyAnalyzer(void)
{
}


KeyAnalyzer::~KeyAnalyzer(void)
{
}

}