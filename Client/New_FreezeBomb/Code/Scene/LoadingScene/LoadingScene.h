#pragma once
class CTexture;
class CShader;
class CLoadingShader;


class CLoadingScene
{
public:
	CLoadingScene();
	~CLoadingScene();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void AnimateObjects(ID3D12GraphicsCommandList *pd3dCommandList,float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();
	void ReleaseUploadBuffers();

	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }	

	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	
protected:
	ID3D12RootSignature						*m_pd3dGraphicsRootSignature = NULL;
	

	//FMOD 사운드 시스템
	//씬마다 음악이 달라져야 할수 있기 때문에 씬이 사운드를 관리함.
	//CSoundSystem*				m_pSound=NULL;
public:
	
	CShader						**m_ppShaders{ nullptr };
	int							m_nShaders = 0;
};

