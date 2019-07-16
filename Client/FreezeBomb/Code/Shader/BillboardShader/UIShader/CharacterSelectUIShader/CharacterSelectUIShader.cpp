#include "../../../../Stdafx/Stdafx.h"
#include "CharacterSelectUIShader.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../GameObject/Billboard/UI/UI.h"
#include "../../../../SoundSystem/SoundSystem.h"
#include "../../../../Scene/LobbyScene/LobbyScene.h"
#include "../../../../GameFramework/GameFramework.h"

CCharacterSelectUIShader::CCharacterSelectUIShader()
{

}

CCharacterSelectUIShader::~CCharacterSelectUIShader()
{

}

void CCharacterSelectUIShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader();
		m_d3dPipelineStateDesc.PS = CreatePixelShader();
		m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
		m_d3dPipelineStateDesc.BlendState = CreateBlendState();
		m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
		m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
		m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
		m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_d3dPipelineStateDesc.NumRenderTargets = 1;
		m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_d3dPipelineStateDesc.SampleDesc.Count = 1;
		m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[i]);
	}

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CCharacterSelectUIShader::CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{
	//리소스를 사용하려면 뷰를 만들어야한다. 
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	//서술자 힙: 리소스를 서술하는 서술자들을 저장하는 연속적인 메모리 영역(배열)
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	//디바이스(어댑터)마다 서술자 유형별 메모리 크기가다름(32~64바이트)
	// 서술자 힙을 사용하려면 서술자 배열의 원소의 크기를 알아야 함.
	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

void CCharacterSelectUIShader::CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void CCharacterSelectUIShader::CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement)
{
	int nTextures = pTexture->GetTextures();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource *pShaderResource = pTexture->GetTexture(i);				//텍스처 리소스를 하나 얻어온다.
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();		//그 리소스들에 대한 정보를 얻어온다.
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);  //텍스처에 대한 서술자 뷰를 설정한다.
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorStartHandle);		//cpu가 접근할수있는 srv뷰를 만든다.
		m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		//srv뷰를 
		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, m_d3dSrvGPUDescriptorStartHandle);
		m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

D3D12_SHADER_BYTECODE CCharacterSelectUIShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSCharacterSelect", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CCharacterSelectUIShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "PSCharacterSelect", "ps_5_1", &m_pd3dVertexShaderBlob));
}
void CCharacterSelectUIShader::BuildObjects(ID3D12Device *pd3dDevice,ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	const int m_nTextures = 9;
	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, m_nTextures);
	pSelectTextures = new CTexture*[m_nTextures];

	pSelectTextures[BASE] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[BASE]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Base.dds", 0);
	
	pSelectTextures[READY] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[READY]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Ready.dds", 0);

	pSelectTextures[QUIT] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[QUIT]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Quit.dds", 0);

	pSelectTextures[PINK] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[PINK]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Character_Select1.dds", 0);
	
	pSelectTextures[BROWN] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[BROWN]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Character_Select2.dds", 0);

	pSelectTextures[WHITE] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[WHITE]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Character_Select3.dds", 0);
	
	pSelectTextures[BLACK] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[BLACK]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Character_Select4.dds", 0);
	
	pSelectTextures[BLUE] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[BLUE]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Character_Select5.dds", 0);
	
	pSelectTextures[PANDA] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSelectTextures[PANDA]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/Lobby_Character_Select6.dds", 0);
	

	for (int i = 0; i < m_nTextures; ++i)
	{
		vTexture.emplace_back(pSelectTextures[i]);
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSelectTextures[i], 0, false);
	}
	
	CBillboardMesh* pSelectMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	m_nObjects = m_nTextures;	
	m_ppUIMaterial = new CMaterial*[m_nObjects];

	m_ppUIMaterial[0] = new CMaterial(m_nObjects);

	for(int i=0; i < m_nObjects;++i)
	{
		m_ppUIMaterial[0]->SetTexture(vTexture[i], i);
	}

	for (int i = 0; i < m_nObjects; ++i)
	{
		CUI* pUI = new CUI(1);
		pUI->SetMesh(pSelectMesh);
		pUI->SetMaterial(0, m_ppUIMaterial[0]);
		m_UIMap.emplace(i, pUI);
	}
}

void CCharacterSelectUIShader::DecideTextureByCursorPosition(UINT nMessgeID, float mouseX, float mouseY)
{

	if (-0.928f <= mouseX && mouseX <= -0.74f && -0.585f <= mouseY && mouseY <= -0.345f)
	{
		if (isReady == false)
		{
			m_currentTexture = PINK;

			if (nMessgeID == WM_LBUTTONUP)	//버튼이 눌렸을때만 캐릭터 선택됨
			{
				cout << "PINK" << endl;
				m_characterSelect = PINK;
			}
		}
		CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);
	}
	else if (-0.69f <= mouseX && mouseX <= -0.515f && -0.585f <= mouseY && mouseY <= -0.345f)
	{
		if (isReady == false)
		{
			m_currentTexture = BROWN;

			if (nMessgeID == WM_LBUTTONUP)	//버튼이 눌렸을때만 
			{
				cout << "BROWN" << endl;
				m_characterSelect = BROWN;
			}
		}
		CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);
	}
	else if (-0.455f <= mouseX && mouseX <= -0.28f && -0.585f <= mouseY && mouseY <= -0.345f)
	{
		if (isReady == false)
		{
			m_currentTexture = WHITE;

			if (nMessgeID == WM_LBUTTONUP)	//버튼이 눌렸을때만 
			{
				cout << "WHITE" << endl;
				m_characterSelect = WHITE;
			}
		}
		CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);
	}
	else if (-0.92f <= mouseX && mouseX <= -0.748f && -0.88f <= mouseY && mouseY <= -0.645f)
	{
		if (isReady == false)
		{
			m_currentTexture = BLACK;

			if (nMessgeID == WM_LBUTTONUP)	//버튼이 눌렸을때만 
			{
				cout << "BLACK" << endl;
				m_characterSelect = BLACK;
			}
		}
		CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);
	}
	else if (-0.69f <= mouseX && mouseX <= -0.515f && -0.88f <= mouseY && mouseY <= -0.645f)
	{
		if (isReady == false)
		{
			m_currentTexture = BLUE;

			if (nMessgeID == WM_LBUTTONUP)	//버튼이 눌렸을때만 
			{
				cout << "BLUE" << endl;
				m_characterSelect = BLUE;
			}
		}
		CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);
	}
	else if (-0.458f <= mouseX && mouseX <= -0.28f && -0.88f <= mouseY && mouseY <= -0.645f)
	{
		if (isReady == false)
		{
			m_currentTexture = PANDA;

			if (nMessgeID == WM_LBUTTONUP)	//버튼이 눌렸을때만 
			{
				cout << "PANDA" << endl;
				m_characterSelect = PANDA;
			}
		}
		CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);
	}
	else if (0.736f <= mouseX && mouseX <= 0.965f && 0.835f <= mouseY && mouseY <= 0.973f)
	{
		m_currentTexture = QUIT;
		CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);
	}
	else
	{
		if (isReady == false)
		{
			m_currentTexture = BASE;
		}
	}

	

	if (nMessgeID == WM_LBUTTONUP)
	{
		// 풀스크린일 경우 UI 좌표가 조금씩 달라짐...
//#ifndef FullScreenMode
		if (0.426f <= mouseX && mouseX <= 0.978f && -0.95f <= mouseY && mouseY <= -0.775f)
		{
			isReady = !isReady;
			if (isReady)
			{
#ifdef _WITH_SERVER_
				if (isCharacterSelectDone == false)
				{
					Network::GetInstance()->SendReady(m_characterSelect);
					//m_Network.SendReady(g_PlayerCharacter);
					isCharacterSelectDone = true;
				}
#endif
				m_currentTexture = READY;
			}
			else
			{
#ifdef _WITH_SERVER_
				isCharacterSelectDone = false;
				Network::GetInstance()->SendNotReady();
#endif
				m_currentTexture = BASE;
			}
			CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);

		}
//#else
//		if (0.426f <= mouseX && mouseX <= 0.978f && -0.95f <= mouseY && mouseY <= -0.775f)
//		{
//			isReady = !isReady;
//			
//			if (isReady)
//				m_currentTexture = READY;
//			else
//				m_currentTexture = BASE;
//
//		}
//#endif
		else if (0.736f <= mouseX && mouseX <= 0.965f && 0.835f <= mouseY && mouseY <= 0.973f)
			::PostQuitMessage(0);
	}
}

void CCharacterSelectUIShader::Render(ID3D12GraphicsCommandList *pd3dCommandList,int nPipelineState)
{
	if (m_pd3dCbvSrvDescriptorHeap) 
		pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	CUIShader::OnPrepareRender(pd3dCommandList, 0);
	
	auto iter = m_UIMap.find(m_currentTexture);
	if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, nPipelineState,m_currentTexture);
}

void CCharacterSelectUIShader::ReleaseObjects()
{
	if (m_pd3dCbvSrvDescriptorHeap)
		m_pd3dCbvSrvDescriptorHeap->Release();

	CUIShader::ReleaseObjects();

	delete[] pSelectTextures;
}