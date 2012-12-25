#ifndef _DX_CORE_H_
#define _DX_CORE_H_

#include <Windows.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#define RELEASE(x) { if(x) (x)->Release(); x = NULL; }

namespace LiveCoder {


class DXCore {
private:
	// DirectX
	LPDIRECT3D9       d3d9;
	LPDIRECT3DDEVICE9 d3ddevice;

	LPDIRECT3DSURFACE9 backbufferSurface;
	LPDIRECT3DTEXTURE9 renderTexture;
	LPDIRECT3DSURFACE9 renderSurface;
	
	LPDIRECT3DTEXTURE9 optionTexture;

	int width, height;
public:
	LPDIRECT3DDEVICE9 Device() const {
		return d3ddevice;
	}
	LPDIRECT3DTEXTURE9 RenderTexture() const {
		return renderTexture;
	}
	LPDIRECT3DSURFACE9 RenderSurface() const {
		return renderSurface;
	}
	LPDIRECT3DSURFACE9 BackbufferSurface() const {
		return backbufferSurface;
	}

	int InitializeDX(const int width_, const int height_, const HWND hwnd);

	void ResetDevice();
	void Unload();
	void Load();

	DXCore();
	virtual ~DXCore();
};

};


#endif // _DX_CORE_H_