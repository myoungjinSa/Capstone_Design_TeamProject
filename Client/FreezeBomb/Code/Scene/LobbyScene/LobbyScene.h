#pragma once

class CShader;
class CLobbyScene
{
public:
	CLobbyScene();
	~CLobbyScene();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void AnimateObjects(ID3D12GraphicsCommandList *pd3dCommandList,float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }	

	XMFLOAT3 ScreenPosition(int x, int y);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam,int gameState);

	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	enum gamestate {CHARACTER_SELECT};

	void UIRender();

	static void AddClientsCharacter(char id, char matID) { m_ClientsCharacter[id] = matID; }
protected:
	ID3D12RootSignature*		m_pd3dGraphicsRootSignature = NULL;
	map<string, CShader*>	m_shaderMap;
	static array<char, 6> m_ClientsCharacter;

public:
	CShader**			m_ppShaders{ nullptr };
	int							m_nShaders = 0;
};

