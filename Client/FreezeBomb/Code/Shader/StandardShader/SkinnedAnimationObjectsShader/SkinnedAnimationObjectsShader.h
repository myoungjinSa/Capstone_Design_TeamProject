#pragma once
#include "../StandardShader.h"

class CGameObject;

class CSkinnedAnimationObjectsShader : public CStandardShader
{
public:
	CSkinnedAnimationObjectsShader();
	virtual ~CSkinnedAnimationObjectsShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

protected:
	CGameObject**	m_ppObjects = 0;
	int							m_nObjects = 0;
};
