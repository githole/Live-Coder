#ifndef _TEXT_EDITOR_
#define _TEXT_EDITOR_

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>

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

	bool selectMode;
	EditorCursor selectStart, selectEnd;

	TextEditorBuffer copyBuffer;
public:
	void MoveCursor(enum EditorCursorMoveType type);
	void InsertCharacter(char ch, bool historyEnable = true, bool autoIndent = true);
	void Backspace();
	void Delete();

	void ScrollDown();
	void ScrollUp();
	
	void MoveHead();
	void MoveTail();

	void Undo();
	void Redo();
	void Home();
	void End();

	void PageUp();
	void PageDown();

	void Copy();
	void DeleteSelectedArea();
	void Paste();

	void SnapShot();

	void BeginSelect();
	void EndSelect();
	bool IsSelectMode() { return selectMode; }
	EditorCursor GetSelectStart();

	void UpdateLog();

	void Save(std::string filename);
	void Load(std::string filename);

	EditorCursor GetCursorPosition();

	TextEditorPtrBuffer GetText();
	TextEditorPtrBuffer GetVisibleText();
	const int GetMaxLineNum() { return maxLineNum; }
	const int GetLineOffset() { 
		if (logIndex == -1) {
			return lineOffset;
		} else {
			return log[logIndex].lineOffset;
		}
	}
	const int GetLineNum() {
		if (logIndex == -1) {
			return buffer.size();
		} else {
			return log[logIndex].buffer.size();
		}
	}

	const int GetLineLength(const int line) {
		if (line+lineOffset >= 0 && line+lineOffset < buffer.size())
			return buffer[line + lineOffset].length();
		return 0;
	}

	std::string ToString();

	void ClearBuffer();
	void Output();

	TextEditor(void);
	virtual ~TextEditor(void);
};

}

#endif
