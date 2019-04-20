#pragma once

#include "../UIShader.h"

class CTexture;
class CUI;
class CProgressBarUIShader : public CUIShader
{
public:
	CProgressBarUIShader();
	virtual ~CProgressBarUIShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int UIType);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CTexture*>& Context, void* pContext);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

private:
	enum Type { ProgressBar };

	struct CB_Position
	{
		XMFLOAT2 m_Position;
	};
	ID3D12Resource*		m_pd3dPositionData{ nullptr };
	CB_Position*				m_pMappedPositionData{ nullptr };

	float m_ProgressBarX = -0.25f;
};