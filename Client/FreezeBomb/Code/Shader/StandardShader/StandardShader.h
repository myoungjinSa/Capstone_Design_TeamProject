#pragma once

#include "../Shader.h"

class CGameObject;

class CStandardShader : public CShader
{
public:
	CStandardShader();
	virtual ~CStandardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

protected:
	CGameObject**									m_ppObjects = 0;
	int															m_nObjects = 0;
};
