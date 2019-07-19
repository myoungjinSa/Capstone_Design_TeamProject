#pragma once

#include "../UIShader.h"

class CTexture;
class CCharacterSelectUIShader: public CUIShader
{
public:
	CCharacterSelectUIShader();
	virtual ~CCharacterSelectUIShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int UIType);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	
	void CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);
	void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList,int nPipelineStates);
	virtual void ReleaseObjects();
	
		//cpu,gpu 디스크립터 핸들을 반환해주는 함수가 각각 필요 
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	//상수 버퍼용 디스크립터 핸들과 쉐이더 리소스용 디스크립터 핸들 필요
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	//enum LOBBY_CHARACTERSEL { PINK, BROWN, WHITE, BLACK, BLUE, PANDA, BASE, READY, QUIT };
	
	void CallbackMouse(UINT nMessgeID, float mouseX, float mouseY);
	void MoveInteraction(int key);
	void ClickInteraction(int click);

	bool UICollisionCheck(XMFLOAT2& mousePos, XMFLOAT2& minUIPos, XMFLOAT2& maxUIPos);

	static bool GetIsReady() { return m_IsReady; }
	static char GetChoiceCharacter() { return m_ChoiceCharacter - 3; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

private:
	ID3D12DescriptorHeap*						m_pd3dCbvSrvDescriptorHeap = nullptr;			//cbv,srv의 서술자 힙

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;
	
	CTexture** m_ppTextures { nullptr };

	enum MOUSE_STATE { NONE = -1, MOUSE_ON, MOUSE_CLICK };

	static bool m_IsReady;
	static char	 m_ChoiceCharacter;

	MOUSE_STATE m_MouseState = MOUSE_STATE::NONE;

	enum UITYPE { BASE, READY, QUIT, PINK, BROWN, WHITE, BLACK, BLUE, PANDA };

	struct CB_UI
	{
		XMFLOAT4 m_Ready_MinMaxPos;
		XMFLOAT4 m_Quit_MinMaxPos;
		XMFLOAT4 m_ReadyAndQuit_UV;
	};

	ID3D12Resource*		m_pd3dUIData{ nullptr };
	CB_UI*						m_pMappedUIData{ nullptr };
};