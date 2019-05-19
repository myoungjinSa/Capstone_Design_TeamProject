#pragma once
class CTexture;
class CShader;
class CLoginShader;

class CLoginScene
{
public:
	CLoginScene();
	~CLoginScene();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void AnimateObjects(ID3D12GraphicsCommandList *pd3dCommandList, float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList){
		pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	}

	
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	
protected:
	ID3D12RootSignature						*m_pd3dGraphicsRootSignature = NULL;
public:
	
	CShader						**m_ppShaders{ nullptr };
	int							m_nShaders = 0;

};