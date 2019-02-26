#pragma once

struct Bounds
{
	string m_Name;
	XMFLOAT3 m_xmf3Center = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 m_xmf3Extent = XMFLOAT3(0.f, 0.f, 0.f);
};

// 공유 되는 리소스들을 관리하기 위한 클래스
class CTexture;
class CResourceManager
{
public:
	CResourceManager();
	~CResourceManager();

	void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadBound(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadMap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,const string filename);

	//void CreateOffScreenRenderTargeViews(ID3D12Device *pd3dDevice,ID3D12GraphicsCommandList *pd3dCommandList,int clientWidth,int clientHeight);

	void Release();

	const map<string, CTexture*>& getTextureMap()	const { return m_TextureMap; }
	const map<string, Bounds*>& getBoundMap()	const { return m_BoundMap; }

private:

	//static const UINT				m_nCartoonScreenRenderTargetBuffers = 2;
	//ID3D12Resource*					m_ppd3dCartoonScreenRenderTargetBuffers[m_nCartoonScreenRenderTargetBuffers];
	//D3D12_CPU_DESCRIPTOR_HANDLE		m_pd3dCarttonScreenRenderTargetBufferCPUHandles[m_nCartoonScreenRenderTargetBuffers];


	map<string, CTexture*> m_TextureMap;
	map<string, Bounds*> m_BoundMap;
};