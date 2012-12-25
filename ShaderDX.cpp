#include "ShaderDX.h"
#include "DXCore.h"
#include "Logger.h"

namespace LiveCoder {

ShaderDX::ShaderDX() {
	ps = NULL;
	dxcore = NULL;
}

ShaderDX::~ShaderDX() {
	Unload();
}

void ShaderDX::Load(DXCore *dxcore_) {
	dxcore = dxcore_;
}

void ShaderDX::Unload() {
	RELEASE(ps);
}



int ShaderDX::Compile(const std::string& shader) {
	errorLinesFS.clear();

	LPD3DXBUFFER pVSBuffer = NULL;
	LPD3DXBUFFER pPSBuffer = NULL;
	LPD3DXBUFFER pError = NULL;
	HRESULT result;
	LPDIRECT3DPIXELSHADER9 tmpps = NULL;

	
	if (D3DXCompileShader(shader.c_str(), shader.length(), 0, 0, "ps", "ps_3_0", D3DXSHADER_OPTIMIZATION_LEVEL3|D3DXSHADER_PREFER_FLOW_CONTROL, &pPSBuffer, &pError, 0) == D3D_OK) {
		dxcore->Device()->CreatePixelShader(static_cast< const DWORD* >(pPSBuffer->GetBufferPointer()), &tmpps);
	}
	
	if (pError) {
		Logger::Instance()->OutputString("Compile error in pixel shader.\n");
		const char* error_str  = static_cast< const char* >(pError->GetBufferPointer());
		Logger::Instance()->OutputString(error_str);

		// 適当パーズ
		const int bufSize = strlen(error_str);
		std::string tmpStr;

		for (int i = 0; i <  bufSize; i ++) {

			if (error_str[i] == '\n') {
				int num = 0;
				int numcnt = 0;
				bool inNum = false;
				int in_p = 0;
				for (int j = 0; j < tmpStr.length(); j ++) {
					if (tmpStr[j] == '(') {
						in_p ++;
					} else if (tmpStr[j] == ')') {
						in_p --;
					}

					if (inNum) {
						if (isdigit(tmpStr[j])) {
							num = num * 10 + (tmpStr[j] - '0');
						} else {
							inNum = false;
							if (in_p > 0)
								numcnt ++;

							// 1番目の数字
							if (numcnt == 1) {
								errorLinesFS.insert(num);
								break;
							}
							num = 0;
						}
					} else {
						if (isdigit(tmpStr[j])) {
							inNum = true;
							num = num * 10 + (tmpStr[j] - '0');
						}
					}
				}
				tmpStr = "";
			} else {
				tmpStr += error_str[i];
			} 
		}
	}
		
	if (tmpps == NULL) {
		return 0;
	}

	RELEASE(ps);
	ps = tmpps;

	return 1;
}

int ShaderDX::CompileFromFile(const std::string& filename) {
	FILE* fp = fopen(filename.c_str(), "rt");
	if (fp != NULL) {
		std::string shader;
		char buf[1024];
		while (fgets(buf, 1024, fp) != NULL) {
			shader += buf;
		}

		fclose(fp);

		return this->Compile(shader);
	}
	return 0;
}

bool ShaderDX::Valid() {
	if (ps)
		return true;
	else
		return false;
}

void ShaderDX::Bind() {
	dxcore->Device()->SetPixelShader(static_cast< LPDIRECT3DPIXELSHADER9 >(ps));
}

void ShaderDX::Unbind() {
	dxcore->Device()->SetPixelShader(static_cast< LPDIRECT3DPIXELSHADER9 >(NULL));
}


//shader constant

void ShaderDX::SetUniform(const int register_num, D3DXVECTOR4 &v) {
	dxcore->Device()->SetPixelShaderConstantF(register_num, (FLOAT*)&v, 4);
}

};