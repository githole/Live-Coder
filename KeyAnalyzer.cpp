#include "KeyAnalyzer.h"

namespace LiveCoder {

bool IsMoveKey(SDLKey key) {
	if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT ||
		key == SDLK_HOME || key == SDLK_END ||
		key == 'p' || key == 'P' || key == 'n' || key == 'N' || key == 'b' || key == 'B' || key == 'f' || key == 'F') {
			return true;
	}
	return false;
}

void KeyAnalyzer::KeyHit(TextEditor* textEditor, SDLKey key, SDLMod mod,  const std::string& filename) {

	/*
	if ((mod & KMOD_SHIFT) && IsMoveKey(key)) {
		if (!textEditor->IsSelectMode())
			textEditor->BeginSelect();
	} else {
		textEditor->EndSelect();
	}*/

	if (IsMoveKey(key)) {
		if (mod & KMOD_SHIFT) {
			if (!textEditor->IsSelectMode())
				textEditor->BeginSelect();
		} else {
			textEditor->EndSelect();
		}
	}

	if (mod & KMOD_CTRL) {
		// unix like keybind
		if (key == 'p' || key == 'P')
			textEditor->MoveCursor(ECMT_UP);
		else if (key == 'n' || key == 'N')
			textEditor->MoveCursor(ECMT_DOWN);
		else if (key == 'b' || key == 'B')
			textEditor->MoveCursor(ECMT_LEFT);
		else if (key == 'f' || key == 'F')
			textEditor->MoveCursor(ECMT_RIGHT);

		// shortcut key
		else if (key == 'z' || key == 'Z')
			textEditor->Undo();
		else if (key == 'y' || key == 'Y')
			textEditor->Redo();
		else if (key == 's' || key == 'S')
			textEditor->Save(filename);
		else if (key == 'o' || key == 'O')
			textEditor->Load(filename);
		else if (key == 'a' || key == 'A')
			textEditor->Home();
		else if (key == 'e' || key == 'E')
			textEditor->End();
		else if (key == 'c' || key == 'C')
			textEditor->Copy();
		else if (key == 'x' || key == 'X') {
			textEditor->Copy();
			textEditor->DeleteSelectedArea();
		} else if (key == 'v' || key == 'V')
			textEditor->Paste();
		else if (key == SDLK_HOME)
			textEditor->MoveHead();
		else if (key == SDLK_END)
			textEditor->MoveTail();
	} else if (32 <= key && key <= 126) {
		if (textEditor->IsSelectMode()) {
			textEditor->DeleteSelectedArea();
		}
		textEditor->InsertCharacter(key);
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

		case SDLK_PAGEUP:
			textEditor->PageUp();
			break;
		case SDLK_PAGEDOWN:
			textEditor->PageDown();
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
			if (textEditor->IsSelectMode()) {
				textEditor->DeleteSelectedArea();
			} else {
				textEditor->Backspace();
			}
			break;
		case SDLK_DELETE:
			if (textEditor->IsSelectMode()) {
				textEditor->DeleteSelectedArea();
			} else {
				textEditor->Delete();
			}
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