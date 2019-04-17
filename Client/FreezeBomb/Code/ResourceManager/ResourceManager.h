#pragma once

struct Bounds
{
	string m_Name;
	XMFLOAT3 m_xmf3Center = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 m_xmf3Extent = XMFLOAT3(0.f, 0.f, 0.f);
};

struct MapObjectInfo
{
	string m_Name;
	XMFLOAT3 m_Position = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 m_Look = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 m_Up = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 m_Right = XMFLOAT3(0.f, 0.f, 0.f);
};

class CTexture;
class CLoadedModelInfo;
// 공유 되는 리소스들을 관리하기 위한 클래스
class CResourceManager
{
public:
	CResourceManager();
	~CResourceManager();

	void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	void LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void LoadMapObjectInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadBound(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void ReleaseModel();
	void Release();

	const map<string, CTexture*>& getTextureMap()	const { return m_TextureMap; }
	const map<string, CLoadedModelInfo*>& getModelMap()	const { return m_ModelMap; }
	const map<string, Bounds*>& getBoundMap()	const { return m_BoundMap; }
	const multimap<string, MapObjectInfo*>& getMapObjectInfo()	const { return m_MapObjectInfoMultiMap; }

private:
	
	map<string, CTexture*>					m_TextureMap;
	map<string, CLoadedModelInfo*>			m_ModelMap;
	map<string, Bounds*>						m_BoundMap;

	multimap<string, MapObjectInfo*>	m_MapObjectInfoMultiMap;
};