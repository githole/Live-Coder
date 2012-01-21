#ifndef _AUDIO_ANALYZER_
#define _AUDIO_ANALYZER_


#ifdef	__APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#ifdef __linux__
#include <AL/al.h>
#include <AL/alc.h>
#else
#include <al.h>
#include <alc.h>
#endif
#endif

#include "Logger.h"

namespace LiveCoder {

class AudioAnalyzer
{
private:
	int frequency;
	int captureSize;

	ALCdevice* inputDevice;
	short* capturedBuffer;
	float* ffted;
	float* ar;
	float* ai;

	bool initialized;
public:
	float* Capture();

	AudioAnalyzer(int frequency_, int captureSize_);
	virtual ~AudioAnalyzer(void);
};
};

#endif
