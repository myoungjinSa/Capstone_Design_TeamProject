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
	void ReleaseUploadBuffers();

	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }	

	XMFLOAT3 ScreenPosition(int x, int y);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	enum gamestate {CHARACTER_SELECT};

	void UIRender();
	void UIClientsNameTextRender();
	void UIChoiceCharacterRender();
	void UIClientsReadyTextRender();

	static void AddClientsCharacter(char id, char matID) { m_ClientsCharacter[id] = matID; }
	void LobbySceneClear();

	CShader* GetCharacterSelectShader() { return m_shaderMap["Select"]; }

private:
	ID3D12RootSignature*		m_pd3dGraphicsRootSignature = NULL;
	
	CShader**			m_ppShaders{ nullptr };
	int							m_nShaders = 0;

	map<string, CShader*>	m_shaderMap;
	static array<char, 6> m_ClientsCharacter;
};

