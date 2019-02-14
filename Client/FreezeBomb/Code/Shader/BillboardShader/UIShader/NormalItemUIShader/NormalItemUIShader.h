#pragma once

#include "../UIShader.h"

class CTexture;
class CNoramlItemUIShader : public CUIShader
{
public:
	CNoramlItemUIShader();
	virtual ~CNoramlItemUIShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CTexture*>& Context, void* pContext);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ReleaseObjects();

	void setRender(bool value) { m_Render = value; }

private:
	bool m_Render = false;
};