#ifndef _KEY_BUFFER_
#define	_KEY_BUFFER_

namespace LiveCoder {
	
const int KeyNum = 10;
class KeyBuffer
{
private:
	bool state[KeyNum];
public:

	void Clear() {
		for (int i = 0; i < KeyNum; i ++)
			state[i] = false;
	}

	void SetState(int num, bool st) {
		if (0 <= num && num < KeyNum)
			state[num] = st;
	}

	bool GetState(int num) {
		if (0 <= num && num < KeyNum)
			return state[num];
		return false;
	}

	KeyBuffer(void) {
		for (int i = 0; i < KeyNum; i ++)
			state[i] = false;
	}
	virtual ~KeyBuffer(void) {}
};

}


#endif
