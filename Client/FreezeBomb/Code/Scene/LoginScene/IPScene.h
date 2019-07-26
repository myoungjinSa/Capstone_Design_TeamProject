#pragma once

#ifdef _WITH_SERVER_
#ifdef _WITH_DIRECT2D_
class CTexture;
class CShader;
class CIPShader;
class CIPInputSystem;

class CIPScene
{
public:
	CIPScene();
	~CIPScene();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, IDWriteTextFormat* pFont, IDWriteTextLayout* pTextLayout, ID2D1SolidColorBrush* pFontColor, ID2D1DeviceContext2* pContext);
	//void AnimateObjects(ID3D12GraphicsCommandList *pd3dCommandList, float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();
	void ReleaseUploadBuffers();

	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

	void ClearIPFont();
	void ProcessInput(HWND hWnd);
	void DrawFont();
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	
protected:
	ID3D12RootSignature						*m_pd3dGraphicsRootSignature = NULL;

	CIPInputSystem*						m_pInput;
public:
	
	
	CShader						**m_ppShaders{ nullptr };
	int							m_nShaders = 0;

};
#endif
#endif