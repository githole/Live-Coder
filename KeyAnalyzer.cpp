#include "KeyAnalyzer.h"

namespace LiveCoder {

void KeyAnalyzer::StateSet(KeyAnalyzerSpecialKey type, bool value) {
	special[type].state = value;
	special[type].repeating = false;
	special[type].last = 0;
}


void KeyAnalyzer::Set(SDLKey key, bool value) {


	Uint32 now = SDL_GetTicks();

	if (32 <= key && key <= 122) {
		state[key].state = value;
		state[key].last = 0;
		state[key].repeating = false;
	} else {
		// “ÁŽêƒL[
		switch (key) {
		case SDLK_UP:
			StateSet(KASK_UP, value);
			break;
		case SDLK_DOWN:
			StateSet(KASK_DOWN, value);
			break;
		case SDLK_LEFT:
			StateSet(KASK_LEFT, value);
			break;
		case SDLK_RIGHT:
			StateSet(KASK_RIGHT,value);
			break;
		case SDLK_RETURN:
			StateSet(KASK_ENTER, value);
			break;
		case SDLK_BACKSPACE:
			StateSet(KASK_BACKSPACE, value);
			break;
		case SDLK_RSHIFT:
		case SDLK_LSHIFT:
			StateSet(KASK_SHIFT, value);
			break;
		case SDLK_TAB:
			StateSet(KASK_TAB, value);
			break;
		case SDLK_RCTRL:
		case SDLK_LCTRL:
			StateSet(KASK_CTRL, value);
			break;
		case SDLK_HOME:
			StateSet(KASK_HOME, value);
			break;
		case SDLK_END:
			StateSet(KASK_END, value);
			break;
		}
	}
}


bool IsValid(KeyState* state, Uint32 now) {
	if (state->state) {
		bool ok = false;
		if (state->last != 0) {
			Uint32 interval;
			
			if (state->repeating)
				interval = 25;
			else {
				interval = 250;
			}

			if (now - state->last >= interval) {
				ok = true;
				state->repeating = true;
			}
		} else ok = true;

		if (!ok)
			return ok;

		state->last = now;

		return ok;
	}

	return false;
}

const char shiftTable[][2] = {
	{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9}, {10, 10}, {11, 11}, {12, 12}, {13, 13}, {14, 14}, {15, 15}, 
	{16, 16}, {17, 17}, {18, 18}, {19, 19}, {20, 20}, {21, 21}, {22, 22}, {23, 23}, {24, 24}, {25, 25}, {26, 26}, {27, 27}, {28, 28}, {29, 29}, {30, 30}, {31, 31}, 
	{' ', ' '}, {'!', '!'}, {'"', '"'}, {'#', '#'}, {'$', '$'}, {'%', '%'}, {'&', '&'}, {'\'', '"'}, {'(', '('}, {')', ')'}, {'*', '*'}, {'+', '+'}, {',', '<'}, {'-', '_'}, {'.', '>'}, {'/', '?'}, 
	{'0', ')'}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, {':', '*'}, {';', ':'}, {'<', '<'}, {'=', '+'}, {'>', '>'}, {'?', '?'}, 
	{'@', '@'}, {'A', 'A'}, {'B', 'B'}, {'C', 'C'}, {'D', 'D'}, {'E', 'E'}, {'F', 'F'}, {'G', 'G'}, {'H', 'H'}, {'I', 'I'}, {'J', 'J'}, {'K', 'K'}, {'L', 'L'}, {'M', 'M'}, {'N', 'N'}, {'O', 'O'}, 
	{'P', 'P'}, {'Q', 'Q'}, {'R', 'R'}, {'S', 'S'}, {'T', 'T'}, {'U', 'U'}, {'V', 'V'}, {'W', 'W'}, {'X', 'X'}, {'Y', 'Y'}, {'Z', 'Z'}, {'[', '{'}, {'\\', '|'}, {']', '}'}, {'^', '~'}, {'_', '_'}, 
	{'`', '`'}, {'a', 'A'}, {'b', 'B'}, {'c', 'C'}, {'d', 'D'}, {'e', 'E'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'i', 'I'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {'m', 'M'}, {'n', 'N'}, {'o', 'O'}, 
	{'p', 'P'}, {'q', 'Q'}, {'r', 'R'}, {'s', 'S'}, {'t', 'T'}, {'u', 'U'}, {'v', 'V'}, {'w', 'W'}, {'x', 'X'}, {'y', 'Y'}, {'z', 'Z'}, {'{', '{'}, {'|', '|'}, {'}', '}'}, {'~', '~'}, {127, 127}, 
	{128, 128}, {129, 129}, {130, 130}, {131, 131}, {132, 132}, {133, 133}, {134, 134}, {135, 135}, {136, 136}, {137, 137}, {138, 138}, {139, 139}, {140, 140}, {141, 141}, {142, 142}, {143, 143}, 
	{144, 144}, {145, 145}, {146, 146}, {147, 147}, {148, 148}, {149, 149}, {150, 150}, {151, 151}, {152, 152}, {153, 153}, {154, 154}, {155, 155}, {156, 156}, {157, 157}, {158, 158}, {159, 159}, 
	{160, 160}, {161, 161}, {162, 162}, {163, 163}, {164, 164}, {165, 165}, {166, 166}, {167, 167}, {168, 168}, {169, 169}, {170, 170}, {171, 171}, {172, 172}, {173, 173}, {174, 174}, {175, 175}, 
	{176, 176}, {177, 177}, {178, 178}, {179, 179}, {180, 180}, {181, 181}, {182, 182}, {183, 183}, {184, 184}, {185, 185}, {186, 186}, {187, 187}, {188, 188}, {189, 189}, {190, 190}, {191, 191}, 
	{192, 192}, {193, 193}, {194, 194}, {195, 195}, {196, 196}, {197, 197}, {198, 198}, {199, 199}, {200, 200}, {201, 201}, {202, 202}, {203, 203}, {204, 204}, {205, 205}, {206, 206}, {207, 207}, 
	{208, 208}, {209, 209}, {210, 210}, {211, 211}, {212, 212}, {213, 213}, {214, 214}, {215, 215}, {216, 216}, {217, 217}, {218, 218}, {219, 219}, {220, 220}, {221, 221}, {222, 222}, {223, 223}, 
	{224, 224}, {225, 225}, {226, 226}, {227, 227}, {228, 228}, {229, 229}, {230, 230}, {231, 231}, {232, 232}, {233, 233}, {234, 234}, {235, 235}, {236, 236}, {237, 237}, {238, 238}, {239, 239}, 
	{240, 240}, {241, 241}, {242, 242}, {243, 243}, {244, 244}, {245, 245}, {246, 246}, {247, 247}, {248, 248}, {249, 249}, {250, 250}, {251, 251}, {252, 252}, {253, 253}, {254, 254}, {255, 255}, 
};

void KeyAnalyzer::Input(TextEditor* textEditor, const std::string& filename) {
	

	Uint32 now = SDL_GetTicks();

	
	if (special[KASK_CTRL].state) { // Ctrl + ‚È‚ñ‚Æ‚©
		if (IsValid(&state['p'], now))
			textEditor->MoveCursor(ECMT_UP);
		if (IsValid(&state['n'], now))
			textEditor->MoveCursor(ECMT_DOWN);
		if (IsValid(&state['b'], now))
			textEditor->MoveCursor(ECMT_LEFT);
		if (IsValid(&state['f'], now))
			textEditor->MoveCursor(ECMT_RIGHT);
		if (IsValid(&state['/'], now))
			textEditor->Undo();
		if (IsValid(&state['.'], now))
			textEditor->Redo();

		if (IsValid(&state['s'], now))
			textEditor->Save(filename);
		if (IsValid(&state['o'], now))
			textEditor->Load(filename);
		
		if (IsValid(&state['a'], now))
			textEditor->Home();
		if (IsValid(&state['e'], now))
			textEditor->End();
			
	} else {
		for (int i = 32; i <= 122; i ++) {
			if (IsValid(&state[i], now)) {

				if (special[KASK_SHIFT].state)
					textEditor->InsertCharacter(shiftTable[i][1]);
				else
					textEditor->InsertCharacter(i);
			}
		}

		if (IsValid(&special[KASK_UP], now))
			textEditor->MoveCursor(ECMT_UP);
		if (IsValid(&special[KASK_DOWN], now))
			textEditor->MoveCursor(ECMT_DOWN);
		if (IsValid(&special[KASK_RIGHT], now))
			textEditor->MoveCursor(ECMT_RIGHT);
		if (IsValid(&special[KASK_LEFT], now))
			textEditor->MoveCursor(ECMT_LEFT);

	
		if (IsValid(&special[KASK_HOME], now))
			textEditor->Home();
		if (IsValid(&special[KASK_END], now))
			textEditor->End();

		if (IsValid(&special[KASK_ENTER], now))
				textEditor->InsertCharacter('\n');
		if (IsValid(&special[KASK_BACKSPACE], now))
			textEditor->Backspace();
		if (IsValid(&special[KASK_TAB], now)) {
			textEditor->InsertCharacter(' ');
			textEditor->InsertCharacter(' ');
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