#pragma once

#ifdef _WITH_SERVER_
#ifdef _WITH_DIRECT2D_
class CTexture;
class CShader;
class CIDShader;
class CIDInput;

class CLoginScene
{
public:
	CLoginScene();
	~CLoginScene();
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, IDWriteTextFormat* pFont, IDWriteTextLayout* pTextLayout, ID2D1SolidColorBrush* pFontColor);
	void AnimateObjects(ID3D12GraphicsCommandList *pd3dCommandList, float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();
	void ReleaseUploadBuffers();

	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList){
		pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	}
	int OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	XMFLOAT3 ScreenPosition(int x, int y);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	CIDInput* GetIDInstance() { return m_pInput; }

	bool IsLogin() { return m_bLogin; }
protected:
	ID3D12RootSignature						*m_pd3dGraphicsRootSignature = NULL;

	bool m_bLogin{ false };
	CIDInput*						m_pInput;
public:
	
	
	CShader						**m_ppShaders{ nullptr };
	int							m_nShaders = 0;
};

#endif
#endif
