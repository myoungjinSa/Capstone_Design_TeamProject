#pragma once

#include "../../UIShader.h"

class CTexture;
class CUI;
class COutcomeUIShader : public CUIShader
{
public:
	COutcomeUIShader();
	virtual ~COutcomeUIShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CTexture*>& Context, void* pContext);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);
	virtual void ReleaseObjects();

	enum OUTCOME_TYPE {WIN=0,LOSE};
	void SetOutcome(OUTCOME_TYPE outcome)
	{
		m_outcome = outcome;
	}

	bool getIsRender()	const { return m_IsRender; }
	void setIsRender(bool value) { m_IsRender = value; }

private:
	
	int m_outcome = LOSE;
	bool m_IsRender = false;
	float m_ElaspedTime = 0.f;
};
