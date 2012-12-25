#include "DXCore.h"
#include "Logger.h"

namespace LiveCoder {

static D3DPRESENT_PARAMETERS d3dpp = {
	0, 0,
	D3DFMT_X8R8G8B8, 1, D3DMULTISAMPLE_NONE, 0,
	D3DSWAPEFFECT_DISCARD, 0, !FALSE, TRUE, D3DFMT_D24S8, 0, 0, TRUE,
};

DXCore::DXCore() {
}

DXCore::~DXCore() {
	RELEASE(d3ddevice);
	RELEASE(d3d9);
}

void DXCore::ResetDevice() {
	d3ddevice->Reset(&d3dpp);
}

void DXCore::Load() {
	// Initialize Render Target
	d3ddevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbufferSurface);

	d3ddevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &renderTexture, 0);
	renderTexture->GetSurfaceLevel(0, &renderSurface);
  
	d3ddevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3ddevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d3ddevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  
	d3ddevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	d3ddevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void DXCore::Unload() {
	//Cleanup
	RELEASE(optionTexture);
	RELEASE(renderTexture);
	RELEASE(renderSurface);
}

int DXCore::InitializeDX(const int width_, const int height_, const HWND hwnd) {
	width = width_;
	height = height_;

	d3dpp.BackBufferWidth  = width;
	d3dpp.BackBufferHeight = height;

	// Initialize DirectX
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d9) {
		Logger::Instance()->OutputString("Error: Direct3DCreate9");
		return -1;
	}
	d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3ddevice);
	if (!d3ddevice){
		Logger::Instance()->OutputString("Error: DirectX9 CreateDevice");
		return -1;
	}

	return 0;
}

};