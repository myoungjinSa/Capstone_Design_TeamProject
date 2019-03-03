#pragma once
#include "../../Shader.h"


class CTexture;
class CIceCube;
//class CMaterial;

class CCubeIceShader : public CShader
{
public:
	CCubeIceShader();
	virtual ~CCubeIceShader();


	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE  CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,const map<string,CTexture*>& Context, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void ReleaseObjects();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);
protected:
	
	vector<CIceCube*> m_vIceCube;
};