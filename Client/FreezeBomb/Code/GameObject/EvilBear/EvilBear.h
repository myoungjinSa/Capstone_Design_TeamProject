#pragma once

#include "../GameObject.h"

class CShadow;


class CEvilBear : public CGameObject
{
public:
	CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID);
	virtual ~CEvilBear();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bHammer, bool bBomb, bool bIce, int matID, CCamera *pCamera, int nPipelineState = GameObject);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);

	//void SetSoundPtr();


	void SetActive(bool bActive) { m_bActive = bActive; }
	bool GetActive() { return m_bActive; }

	
	
	void Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);	



private:
	

	bool			m_bActive{ false };		//오브젝트 풀방식을 위해 화면에 렌더링 되려면 m_bActive가 true이어야 함.
	
	CShadow*	m_pShadow;
	
};

