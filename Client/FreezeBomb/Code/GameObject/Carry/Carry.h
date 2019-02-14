#pragma once

#include "../GameObject.h"

class CLampParticle;
class CBillboardMesh;
class CCarry : public CGameObject
{
public:
	CCarry(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CCarry();

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);
	virtual void ReleaseShaderVariables();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

	void setCarry(bool Carry) { m_Carry = Carry; }

private:
	bool							m_Carry = false;

	bool*						m_pcbMappedCarry{ nullptr };
	ID3D12Resource*		m_pd3dcbCarry{ nullptr };

	CLampParticle*			m_pLampParticle{ nullptr };

	CBillboardMesh*		m_pBillboardMesh{ nullptr };
};