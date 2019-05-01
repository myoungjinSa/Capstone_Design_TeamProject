#pragma once

#include "../UIShader.h"

class CTexture;
class CUI;
class CMenuUIShader : public CUIShader
{
public:
	CMenuUIShader();
	virtual ~CMenuUIShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int UIType);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CTexture*>& Context, void* pContext);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);
	virtual void ReleaseObjects();

	bool getIsRender()	const { return m_IsRender; }
	void setIsRender(bool value) { m_IsRender = value; }

private:
	enum MENU_TYPE {Menu};
	bool m_IsRender = false;
};