#include "AudioAnalyzer.h"
#include <math.h>

namespace LiveCoder {

void fft(int n, float theta, float ar[], float ai[])
{
    int m, mh, i, j, k;
    float wr, wi, xr, xi;

    for (m = n; (mh = m >> 1) >= 1; m = mh) {
        for (i = 0; i < mh; i++) {
            wr = cosf(theta * i);
            wi = sinf(theta * i);
            
            for (j = i; j < n; j += m) {
                k = j + mh;
                xr = ar[j] - ar[k];
                xi = ai[j] - ai[k];
                ar[j] += ar[k];
                ai[j] += ai[k];
                ar[k] = wr * xr - wi * xi;
                ai[k] = wr * xi + wi * xr;
            }
        }
        theta *= 2;
    }
    /* ---- unscrambler ---- */
    i = 0;
    for (j = 1; j < n - 1; j++) {
        for (k = n >> 1; k > (i ^= k); k >>= 1);
        if (j < i) {
            xr = ar[j];
            xi = ai[j];
            ar[j] = ar[i];
            ai[j] = ai[i];
            ar[i] = xr;
            ai[i] = xi;
        }
    }
}

AudioAnalyzer::AudioAnalyzer(int frequency_, int captureSize_) :
	frequency(frequency_), captureSize(captureSize_), initialized(false)
{
	ALenum errorCode=0;
	inputDevice = alcCaptureOpenDevice(NULL, frequency, AL_FORMAT_MONO16, frequency/2);
	if (inputDevice == NULL) {
		Logger::Instance()->OutputString("Error: alcCaptureOpenDevice");
		return;
	}

	errorCode = alcGetError(inputDevice);
	if (errorCode != AL_NO_ERROR) {
		Logger::Instance()->OutputString("Error: alcCaptureOpenDevice -- ?");
		return;
	}
	alcCaptureStart(inputDevice); // Begin capturing
	errorCode = alcGetError(inputDevice);
	if (errorCode != AL_NO_ERROR) {
		Logger::Instance()->OutputString("Error: alcCaptureStart");
		alcCaptureCloseDevice(inputDevice);
		return;
	}

	capturedBuffer = new  short[captureSize];
	ffted = new float[captureSize];
	ar = new float[captureSize];
	ai = new float[captureSize];

	initialized = true;

	Logger::Instance()->OutputString("OpenAL succeeded");
}
	
float* AudioAnalyzer::Capture() {
	if (!initialized)
		return NULL;

	ALCint sampled;

	alcGetIntegerv(inputDevice, ALC_CAPTURE_SAMPLES, 1, &sampled);

	if (sampled > captureSize) {
		// Grab the sound
		alcCaptureSamples(inputDevice, capturedBuffer, captureSize);

		const float theta = 2.0f * (3.1415926535898f) / captureSize;
		for (int i = 0; i < captureSize; i ++) {
			ar[i] = capturedBuffer[i] / 32768.0f;
			//		  ar[i] = 0.54 - 0.46 * cos(2.0 * (3.1415926535898f) * (float)capturedBuffer[i] / 32768.0);
			ai[i] = 0.0f;

		}
		fft(captureSize, theta , ar, ai);

		for (int i = 0; i < captureSize; i ++) { 
			ffted[i] = 4.0f * sqrtf(ar[i]*ar[i] + ai[i]*ai[i]) / captureSize;
		}
		return ffted;
	}

	return NULL;
}

AudioAnalyzer::~AudioAnalyzer(void)
{
	if (!initialized)
		return;
	
    alcCaptureStop(inputDevice);
    alcCaptureCloseDevice(inputDevice);
    delete[] capturedBuffer;
    delete[] ffted;
	delete[] ar;
	delete[] ai;
}

}
