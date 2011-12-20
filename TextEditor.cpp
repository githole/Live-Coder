#include "TextEditor.h"

namespace LiveCoder {

void TextEditor::UpdateLog() {
	if (logIndex != -1) {
		nowCursor = log[logIndex].cursor;
		buffer = log[logIndex].buffer;
		lineOffset = log[logIndex].lineOffset;
		std::vector<TextEditorLog>::iterator ite = log.begin() + logIndex;
		log.erase(ite, log.end());

		logIndex = -1;

		SnapShot();
	}
}
void TextEditor::ScrollDown() {
	// 下方向にスクロール
	if (nowCursor.col > lineOffset + maxLineNum - 1) {
		lineOffset ++;
	}
}

void TextEditor::ScrollUp() {
	// 上方向にスクロール
	if (nowCursor.col < lineOffset) {
		lineOffset --;
	}
}
void TextEditor::PageUp() {
	UpdateLog();
	if (nowCursor.col - maxLineNum >= 0) {
		nowCursor.col -= maxLineNum;
		lineOffset -= maxLineNum;
		if (lineOffset < 0)
			lineOffset = 0;
	} else {
	}
}

void TextEditor::MoveHead() {
	UpdateLog();
	nowCursor.col = 0;
	nowCursor.row = 0;
	lineOffset = 0;
}

void TextEditor::MoveTail() {
	UpdateLog();
	nowCursor.col = buffer.size() - 1;
	nowCursor.row = buffer[buffer.size() - 1].length();
	lineOffset = nowCursor.col - maxLineNum + 1;
	if (lineOffset < 0)
		lineOffset = 0;
}

void TextEditor::PageDown() {
	UpdateLog();
	
	if (nowCursor.col + maxLineNum < buffer.size()) {
		nowCursor.col += maxLineNum;
		lineOffset += maxLineNum;
	}
}
	
void TextEditor::MoveCursor(enum EditorCursorMoveType type) {
	UpdateLog();

	switch (type) {
	case ECMT_DOWN:
		if (buffer.size() > nowCursor.col + 1) {
			nowCursor.col ++;
			if (buffer[nowCursor.col].length() < nowCursor.row) {
				nowCursor.row = buffer[nowCursor.col].length();
			}
			ScrollDown();
		}
		break;
	case ECMT_UP:
		if (0 <= nowCursor.col - 1) {
			nowCursor.col --;
			if (buffer[nowCursor.col].length() < nowCursor.row) {
				nowCursor.row = buffer[nowCursor.col].length();
			}
			ScrollUp();
		}
		break;
	case ECMT_LEFT:
 		if (0 <= nowCursor.row - 1) {
			nowCursor.row --;
		} else { // nowCursor.row == 0
			if (nowCursor.col > 0) {
				nowCursor.col --;
				nowCursor.row = buffer[nowCursor.col].length();
				
				ScrollUp();
			}
		}
		break;
	case ECMT_RIGHT:
		if (buffer[nowCursor.col].length() >= nowCursor.row + 1) {
			nowCursor.row ++;
		} else { // nowCursor.rowが右端
			if (nowCursor.col + 1 < buffer.size()) {
				nowCursor.col ++;
				nowCursor.row = 0;
				
				ScrollDown();
			}
		}
		break;
	}
}
std::string TextEditor::ToString() {
	std::string str;
	if (logIndex == -1) {
		for (int i = 0; i < buffer.size(); i ++)
			str += buffer[i] + '\n';
	} else {
		for (int i = 0; i < log[logIndex].buffer.size(); i ++)
			str += log[logIndex].buffer[i] + '\n';
	}
	return str;
}

TextEditorPtrBuffer TextEditor::GetText() {
	TextEditorPtrBuffer buf;
	if (logIndex == -1) {
		for (int i = 0; i < buffer.size(); i ++) {
			buf.push_back(&buffer[i]);
		}
	} else {
		int offset = log[logIndex].lineOffset;
		for (int i = 0; i < log[logIndex].buffer.size(); i ++) {
			buf.push_back(&log[logIndex].buffer[i]);
		}
	}
	return buf;
}

TextEditorPtrBuffer TextEditor::GetVisibleText() {
	TextEditorPtrBuffer buf;

	if (logIndex == -1) {
		for (int i = lineOffset; i < lineOffset + maxLineNum && i < buffer.size(); i ++) {
			buf.push_back(&buffer[i]);
		}
	} else {
		int offset = log[logIndex].lineOffset;
		for (int i = offset; i < offset + maxLineNum && i < log[logIndex].buffer.size(); i ++) {
			buf.push_back(&log[logIndex].buffer[i]);
		}
	}

	return buf;
}

EditorCursor TextEditor::GetCursorPosition() { 
	if (logIndex == -1) {
		EditorCursor cursor;
		cursor.row = nowCursor.row;
		cursor.col = nowCursor.col - lineOffset;
		return cursor; 
	} else {
		EditorCursor cursor;
		cursor.row = log[logIndex].cursor.row;
		cursor.col = log[logIndex].cursor.col - log[logIndex].lineOffset;
		return cursor; 
	}
}
void TextEditor::Copy() {
	if (!IsSelectMode())
		return;

	EditorCursor selectStart_ = selectStart;
	EditorCursor selectEnd_ = nowCursor;

	if (selectStart_.col > selectEnd_.col)
		std::swap(selectStart_, selectEnd_);

	copyBuffer.clear();

	if (selectStart_.col == selectEnd_.col) { // single line
		int start = selectStart_.row;
		int end = selectEnd_.row;
		if (start > end)
			std::swap(start, end);
		copyBuffer.push_back(buffer[selectStart_.col].substr(start, end - start));
	} else { // multi line
		for (int i = selectStart_.col; i <= selectEnd_.col; i ++) {

			std::cout << buffer[i];
			if (i == selectStart_.col) {
				int start = selectStart_.row;
				int end = buffer[i].length();
				copyBuffer.push_back(buffer[i].substr(start, end - start));
			} else if (i < selectEnd_.col) {
				copyBuffer.push_back(buffer[i]);
			} else {
				int start = 0;
				int end = selectEnd_.row;
				copyBuffer.push_back(buffer[i].substr(start, end - start));
			}
		}
	}
}

void TextEditor::DeleteSelectedArea() {
	if (!IsSelectMode())
		return;
	
	SnapShot();
	EditorCursor selectStart_ = selectStart;
	EditorCursor selectEnd_ = nowCursor;

	if (selectStart_.col > selectEnd_.col)
		std::swap(selectStart_, selectEnd_);

	if (selectStart_.col == selectEnd_.col) { // single line
		if (selectStart_.row > selectEnd_.row)
			std::swap(selectStart_, selectEnd_);
		int start = selectStart_.row;
		int end = selectEnd_.row;

		buffer[selectStart_.col].erase(start, end - start);

	} else { // multi line
		for (int i = selectStart_.col; i <= selectEnd_.col; i ++) {

			std::cout << buffer[i];
			if (i == selectStart_.col) {
				int start = selectStart_.row;
				int end = buffer[i].length();
				buffer[i].erase(start, end - start);
			} else if (i < selectEnd_.col) {
				// とりあえず何もしない
//				copyBuffer.push_back(buffer[i]);
			} else {
				int start = 0;
				int end = selectEnd_.row;
				buffer[i].erase(start, end - start);
			}
		}
		
		buffer[selectStart_.col] += buffer[selectEnd_.col];
		buffer.erase(buffer.begin() + selectStart_.col + 1, buffer.begin() + selectEnd_.col + 1);
	}
	nowCursor = selectStart_;

	if (nowCursor.col < lineOffset) {
		lineOffset = nowCursor.col - 1;
	}
	

	EndSelect();
}

void TextEditor::Paste() {
	SnapShot();
	for (int i = 0; i < copyBuffer.size(); i ++) {
		if (i > 0)
			InsertCharacter('\n', false, false);
		for (int j = 0; j < copyBuffer[i].length(); j ++) {
			InsertCharacter(copyBuffer[i][j], false, false);
		}
	}
}


EditorCursor TextEditor::GetSelectStart() {
	EditorCursor cursor;
	cursor.row = selectStart.row;
	cursor.col = selectStart.col - lineOffset;
	return cursor; 
}
void TextEditor::BeginSelect() {
	selectMode = true;
	selectStart = nowCursor;
}

void TextEditor::EndSelect() {
	selectMode = false;
}


void TextEditor::SnapShot() {
	log.push_back(TextEditorLog(nowCursor, buffer, lineOffset));

	if (log.size() > 1000) {
		log.erase(log.begin(), log.begin() + 1);
	}
}

void TextEditor::Backspace() {
	SnapShot();
	UpdateLog();
	if (nowCursor.row == 0) {
		if (nowCursor.col > 0) {
			nowCursor.row = buffer[nowCursor.col - 1].length();
			buffer[nowCursor.col - 1] += buffer[nowCursor.col];
			std::vector<std::string>::iterator ite = buffer.begin() + nowCursor.col;
			buffer.erase(ite);
			nowCursor.col --;
			
			ScrollUp();
		}
	} else {
		buffer[nowCursor.col].erase(nowCursor.row - 1, 1);
		nowCursor.row --;
	}
}

void TextEditor::Delete() {
	SnapShot();
	UpdateLog();

	if (nowCursor.row ==  buffer[nowCursor.col].length()) {
		if (nowCursor.col < buffer.size() - 1) {
			buffer[nowCursor.col] += buffer[nowCursor.col + 1];
			std::vector<std::string>::iterator ite = buffer.begin() + nowCursor.col + 1;
			buffer.erase(ite);
		}
	} else {
		buffer[nowCursor.col].erase(nowCursor.row, 1);
	}
}


void TextEditor::Home() {
	int v = 0;
	for (int i = 0; i < buffer[nowCursor.col].length(); i ++)
		if (buffer[nowCursor.col][i] != ' ')
			break;
		else
			v ++;

	nowCursor.row = v;
}

void TextEditor::End() {
	nowCursor.row = buffer[nowCursor.col].length();
}

void TextEditor::Undo() {
	EndSelect();
	if (logIndex == -1) {
		logIndex = log.size() - 1;
	} else {
		if (logIndex > 0)
			logIndex --;
	}
}

void TextEditor::Redo() {
	EndSelect();
	if (logIndex == -1) {
	} else {
		if (logIndex < log.size() - 1) {
			logIndex ++;
		} else
			logIndex = -1;
	}
}


void TextEditor::Save(std::string filename) {
	FILE *fp = fopen(filename.c_str(), "wt");
	if (fp != NULL){
		fprintf(fp, ToString().c_str());
		fclose(fp);
	}
}

void TextEditor::ClearBuffer() {
	buffer.clear();
	buffer.push_back("");
	nowCursor = EditorCursor();
	lineOffset = 0;
	logIndex = -1;
	log.clear();
//	log.push_back(TextEditorLog(nowCursor, buffer, lineOffset));
}

void TextEditor::Load(std::string filename) {
	FILE *fp = fopen(filename.c_str(), "rt");

	EndSelect();
	
	ClearBuffer();
	if (fp != NULL){
		char buf[1024];
		buffer.clear();
		while (fgets(buf, 1024, fp) != NULL){
			int l = strlen(buf);
			if (buf[l - 1] == '\n')
				buf[l - 1] = '\0';
			buffer.push_back(buf);
		}

		fclose(fp);
	}
}

void TextEditor::InsertCharacter(char ch, bool historyEnable, bool autoIndent) {
	if (historyEnable) {
		SnapShot();
	}

	UpdateLog();
	if (ch == '\n') {
		std::string a = buffer[nowCursor.col].substr(0, nowCursor.row);
		std::string b = "";
		if (buffer[nowCursor.col].length() >= nowCursor.row)
			b = buffer[nowCursor.col].substr(nowCursor.row);
		
		std::vector<std::string>::iterator ite = buffer.begin() + nowCursor.col + 1;

		buffer[nowCursor.col] = a;
		buffer.insert(ite, b);

		nowCursor.col ++;
		nowCursor.row = 0;
		
		ScrollDown();

		// begin auto indent
		if (!autoIndent)
			return;
		if (nowCursor.col > 0) {
			int indent = 0;
			int bracket = 0;
			bool pref = true;
			for (int i = 0; i < buffer[nowCursor.col - 1].length(); i++) {
				if (buffer[nowCursor.col - 1][i] == ' ') {
					if (pref)
						indent ++;
				} else pref = false;
					
				if (buffer[nowCursor.col - 1][i] == '{')
					bracket ++;
			}

			if (bracket > 0)
				indent += 2;

			for (int i = 0; i < indent; i ++)
				InsertCharacter(' ', false);
//			buffer[nowCursor.col].insert(0, indent, ' ');
		}
	} else {
		buffer[nowCursor.col].insert(nowCursor.row, 1, ch);
		nowCursor.row ++;
	}
}

void TextEditor::Output() {
	std::cout << ">>> Begin" << std::endl;

	for (int i = 0; i < buffer.size(); i ++) {
		std::cout << "Line: " << i << std::endl;
		std::cout << buffer[i] << std::endl;
	}

	std::cout << "<<< End" << std::endl;
}

TextEditor::TextEditor(void)
{
	maxLineNum = 28;
	lineOffset = 0;
	selectMode = false;
	// 一行目
	buffer.push_back("");
	log.push_back(TextEditorLog(nowCursor, buffer, lineOffset));
	logIndex = -1;

}


TextEditor::~TextEditor(void)
{
}

};