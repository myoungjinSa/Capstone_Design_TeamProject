#pragma once

class CShader;
class CPlayer;
class CCamera;
class CResourceManager;
class CSobelCartoonShader;
class CShaderManager
{
public:
	CShaderManager();
	~CShaderManager();

	void Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const int& nPlayerCount);
	void AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer = nullptr);
	void ReleaseObjects();
	void ReleaseUploadBuffers();
	
	void PostRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CCamera* pCamera = nullptr);
	void PreRender(ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CCamera *pCamera = nullptr);

	void ProcessCollision(XMFLOAT3& position);				//충돌 여부 처리 함수

	const map<string, CShader*>& getShaderMap()	const { return m_ShaderMap; }
	CResourceManager* getResourceManager()	const { return m_pResourceManager; }

private:

	int									m_nShaders = 0;
	//snow나 ui들은 post처리 안되야하기 때문에 m_nShader - m_nPostShader 많이 postshader개수로 파악되어야 함
	int									m_nPostShaders = 0;
	CShader**					m_ppShaders{ nullptr };

	map<string, CShader*>	m_ShaderMap;

	map<string, CShader*>	m_PostShaderMap;

	CResourceManager*			m_pResourceManager{ nullptr };

	CPlayer*	m_pPlayer{ nullptr };
};