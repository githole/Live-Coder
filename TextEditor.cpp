#include "TextEditor.h"

namespace LiveCoder {

void TextEditor::UpdateLog() {
	if (logIndex != -1) {
		nowCursor = log[logIndex].cursor;
		buffer = log[logIndex].buffer;

		std::vector<TextEditorLog>::iterator ite = log.begin() + logIndex + 1;
		log.erase(ite, log.end());
				
		logIndex = -1;
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
	/*
	std::string str;

	for (int i = 0; i < buffer.size(); i ++)
		str += buffer[i] + '\n';

	return str;*/
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

void TextEditor::Backspace() {
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
	if (logIndex == -1) {
		logIndex = log.size() - 1;
	} else {
		if (logIndex > 0)
			logIndex --;
	}
}

void TextEditor::Redo() {
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
	log.clear();
	log.push_back(TextEditorLog(nowCursor, buffer, lineOffset));
}

void TextEditor::Load(std::string filename) {
	FILE *fp = fopen(filename.c_str(), "rt");
	
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
		log.push_back(TextEditorLog(nowCursor, buffer, lineOffset));

		if (log.size() > 1000) {
			log.erase(log.begin(), log.begin() + 1);
		}
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
	maxLineNum = 32;
	lineOffset = 0;
	// 一行目
	buffer.push_back("");
	log.push_back(TextEditorLog(nowCursor, buffer, lineOffset));
	logIndex = -1;

}


TextEditor::~TextEditor(void)
{
}

};