#ifndef _MOUSE_BUFFER_
#define	_MOUSE_BUFFER_

namespace LiveCoder {
	
class MouseBuffer
{
private:
	float cursorX;
	float cursorY;

public:
	void SetPosition(float cursorX_, float cursorY_) {
		cursorX = cursorX_;
		cursorY = cursorY_;
	}

	float GetCursorX() { return cursorX; }
	float GetCursorY() { return cursorY; }

	MouseBuffer(void) : cursorX(0.0f), cursorY(0.0f) {
	}
	virtual ~MouseBuffer(void) {
	}
};

}


#endif
