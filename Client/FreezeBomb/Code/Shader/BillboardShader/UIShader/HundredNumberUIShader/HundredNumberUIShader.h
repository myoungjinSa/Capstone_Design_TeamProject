#pragma once

#include "../UIShader.h"

#pragma once

#include "../UIShader.h"

class CTexture;
class CMaterial;
class CUI;
class CHundredNumberUIShader : public CUIShader
{
public:
	CHundredNumberUIShader();
	virtual ~CHundredNumberUIShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, const map<int, CTexture*>& Context);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ReleaseObjects();

private:
	CMaterial**			m_ppHundredNumberMaterial{ nullptr };
	map<int, CUI*>	m_HundredNumberUIMap;
};