
#include <string>
#ifndef _LOGGER_
#define _LOGGER_

#include <vector>
#include <iostream>

namespace LiveCoder {

class Logger {
private:
	std::vector<std::string> logs;
public:
	void PushString(std::string str) {
		logs.push_back(str);
	}
	void OutputString(std::string str) {
		std::cout << str << std::endl;
		logs.push_back(str);
	}

	std::string ToString() {
		std::string str;
		for (int i = 0; i < logs.size(); i ++)
			str += logs[i] + "\n";

		return str;
	}

	void Save(void) {
		// ...
	}

	static Logger* Instance() {    
		static Logger instance;  // 唯一のインスタンス
    	return &instance;
	}

	Logger(const Logger& rhs) {};
	Logger& operator=(const Logger& rhs) {};

	Logger() {
	}

	virtual ~Logger() {
	}
};

};

#endif