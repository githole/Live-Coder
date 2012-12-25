#ifndef _SHADER_DX_H_
#define _SHADER_DX_H_


#include <stdio.h>
#include <string>
#include <set>

#include <d3d9.h>
#include <d3dx9.h>


#include "Logger.h"

namespace LiveCoder {

class DXCore;

class ShaderDX {
private:
	bool OK;
	std::set<int> errorLinesFS;
	std::set<int> errorLinesVS;
	
	LPDIRECT3DPIXELSHADER9 ps;

	DXCore *dxcore;
public:
	ShaderDX();
	virtual ~ShaderDX();

	const std::set<int>& GetErrorLinesFS() { return errorLinesFS; }
	const std::set<int>& GetErrorLinesVS() { return errorLinesVS; }

	int Compile(const std::string& shader);
	int CompileFromFile(const std::string& filename);

	void Load(DXCore *dxcore_);
	void Unload();
	
	bool Valid();
	void Bind();
	void Unbind();

	void SetUniform(const int register_num, D3DXVECTOR4 &v);
};

};

#endif
