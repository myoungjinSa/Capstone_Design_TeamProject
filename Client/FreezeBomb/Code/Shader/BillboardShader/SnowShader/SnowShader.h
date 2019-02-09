#pragma once

#include "../BillboardShader.h"
#include "../../../Cube/Cube.h"

class CBillboard;
class CMaterial;
class CPlayer;
class CTerrain;

class CSnowShader : public CBillboardShader
{
public:
	CSnowShader();
	virtual ~CSnowShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ReleaseObjects();

private:
	CBillboard**		m_ppObjects = 0;
	int						m_nObjects = 0;

	float					m_fTimeLagScale = 0.05f;

	CMaterial*		m_pMaterial{ nullptr };

	CCube				m_refCubeObject;
	CPlayer*			m_pPlayer{ nullptr };		//플레이어 위치 근처에서 계속 떨어져야 하기 때문에 

	CTerrain*			m_pTerrain{ nullptr };	//눈 오브젝트가 터레인과 충돌하면 다시 위에서부터 떨어진다.
};