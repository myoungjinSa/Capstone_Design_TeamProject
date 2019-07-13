#pragma once

#include "../GameObject.h"

class CShadow;
class CBomb;

class CEvilBear : public CGameObject
{
public:
	CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID);
	virtual ~CEvilBear();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera,int matID, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);

	void SetActive(bool bActive) { m_bActive = bActive; }
	bool GetActive() { return m_bActive; }
	void Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);	


	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }


	CBomb*			GetBombPtr() { return m_BombParticle; }
private:
	bool			m_bActive{ false };		//오브젝트 풀방식을 위해 화면에 렌더링 되려면 m_bActive가 true이어야 함.
	CBomb*			m_BombParticle{ nullptr };


	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	CShadow*	m_pShadow;
};

