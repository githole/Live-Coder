#ifndef _TEXT_EDITOR_
#define _TEXT_EDITOR_

#include <string>
#include <vector>
#include <iostream>

namespace LiveCoder {

enum EditorCursorMoveType {
	ECMT_UP,
	ECMT_DOWN,
	ECMT_LEFT,
	ECMT_RIGHT,
};

class EditorCursor {
public:
	int row;
	int col;

	EditorCursor() : row(0), col(0) {
	}
};

typedef std::vector<std::string> TextEditorBuffer;
typedef std::vector<std::string*> TextEditorPtrBuffer;

class TextEditorLog {
public:
	EditorCursor cursor;
	TextEditorBuffer buffer;

	int lineOffset;

	TextEditorLog(const EditorCursor& cursor_, const TextEditorBuffer& buffer_, const int lineOffset_) : 
		cursor(cursor_), buffer(buffer_), lineOffset(lineOffset_) {
		}
};

class TextEditor
{
private:
	std::vector<TextEditorLog> log;
	int logIndex;

	EditorCursor nowCursor;
	TextEditorBuffer buffer;

	int maxLineNum;
	int lineOffset;
public:
	void MoveCursor(enum EditorCursorMoveType type);
	void InsertCharacter(char ch, bool historyEnable = true, bool autoIndent = true);
	void Backspace();

	void ScrollDown();
	void ScrollUp();
	
	void Undo();
	void Redo();
	void Home();
	void End();

	void UpdateLog();

	void Save(std::string filename);
	void Load(std::string filename);

	EditorCursor GetCursorPosition();

	TextEditorPtrBuffer GetVisibleText();
	const int GetMaxLineNum() { return maxLineNum; }
	const int GetLineOffset() { return lineOffset; }
	const int GetLineNum() {
		if (logIndex == -1) {
			return buffer.size();
		} else {
			return log[logIndex].buffer.size();
		}
	}

	std::string ToString();

	void ClearBuffer();
	void Output();

	TextEditor(void);
	virtual ~TextEditor(void);
};

}

#endif