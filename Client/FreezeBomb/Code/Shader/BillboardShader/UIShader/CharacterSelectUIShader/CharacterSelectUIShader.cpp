#include "../../../../Stdafx/Stdafx.h"
#include "CharacterSelectUIShader.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../GameObject/Billboard/UI/UI.h"
#include "../../../../SoundSystem/SoundSystem.h"
#include "../../../../Scene/LobbyScene/LobbyScene.h"
#include "../../../../GameFramework/GameFramework.h"
#include "../../../../Direct2D/Direct2D.h"

bool CCharacterSelectUIShader::m_IsReady = false;
char CCharacterSelectUIShader::m_ChoiceCharacter = NONE;

extern byte g_PlayerCharacter;

CCharacterSelectUIShader::CCharacterSelectUIShader()
{
}

CCharacterSelectUIShader::~CCharacterSelectUIShader()
{
}

void CCharacterSelectUIShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 3;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader(i);
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

D3D12_SHADER_BYTECODE CCharacterSelectUIShader::CreateVertexShader(int UIType)
{
	switch ((UITYPE)UIType)
	{
	case UITYPE::BASE:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSLobbyBase", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	case UITYPE::READY:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSLobbyReady", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	case UITYPE::QUIT:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSLobbyQuit", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	default:
		break;
	}

}

D3D12_SHADER_BYTECODE CCharacterSelectUIShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "PSCharacterSelect", "ps_5_1", &m_pd3dVertexShaderBlob));
}

void CCharacterSelectUIShader::BuildObjects(ID3D12Device *pd3dDevice,ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_nObjects = 9;
	int nTextures = 3;
	m_ppTextures = new CTexture*[nTextures];
	
	m_ppTextures[BASE] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	m_ppTextures[BASE]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/New/LobbyBase.dds", 0);

	m_ppTextures[READY] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	m_ppTextures[READY]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/New/Ready.dds", 0);

	m_ppTextures[QUIT] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	m_ppTextures[QUIT]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Lobby/New/Quit.dds", 0);

	CBillboardMesh* pSelectMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);
	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, m_nObjects);

	int index = 0;
	// minX, minY, maxX, maxY
	XMFLOAT4* minmaxPos = new XMFLOAT4[m_nObjects];

	float widthSize = 0.209375f, heightSize = 0.3694441f;
	float widthGap = 0.03125f, heightGap = 0.0611109f;

	// BASE
	minmaxPos[index++] = { 0.f, 0.f, 0.f, 0.f };
	// READY
	minmaxPos[index++] = { 0.220312f, -0.538889f, 0.971875f, -0.208333f };
	// QUIT
	minmaxPos[index++] = { 0.220312f, -0.941667f, 0.971875f, -0.611111f };

	// PINK ~ PANDA
	minmaxPos[index++] = { 0.25f, 0.3388889f, 0.459375f, 0.708333f };
	minmaxPos[index++] = { 0.459375f + widthGap, 0.3388889f , 0.459375f + widthGap + widthSize, 0.708333f };
	minmaxPos[index++] = { 0.459375f + 2 * widthGap + widthSize, 0.3388889f , 0.459375f + 2 * widthGap + 2 * widthSize, 0.708333f };
	minmaxPos[index++] = { 0.25f, 0.3388889f - heightGap - heightSize, 0.459375f, 0.3388889f - heightGap };
	minmaxPos[index++] = { 0.459375f + widthGap, 0.3388889f - heightGap - heightSize, 0.459375f + widthGap + widthSize, 0.3388889f - heightGap };
	minmaxPos[index++] = { 0.459375f + 2 * widthGap + widthSize, 0.3388889f - heightGap - heightSize, 0.459375f + 2 * widthGap + 2 * widthSize, 0.3388889f - heightGap };

	for (int i = 0; i < m_nObjects; ++i)
	{
		CUI* pUI = new CUI(1);
		pUI->SetUIMinPos(XMFLOAT2(minmaxPos[i].x, minmaxPos[i].y));
		pUI->SetUIMaxPos(XMFLOAT2(minmaxPos[i].z, minmaxPos[i].w));

		if (i < UITYPE::PINK)
		{
			pUI->SetMesh(pSelectMesh);
			CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_ppTextures[i], 0, false);
			CMaterial* pMaterial = new CMaterial(1);
			pMaterial->SetTexture(m_ppTextures[i], 0);
			pUI->SetMaterial(0, pMaterial);
		}
		m_UIMap.emplace(i, pUI);
	}

	delete[] minmaxPos;
	
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CCharacterSelectUIShader::CallbackMouse(UINT nMessgeID, float mouseX, float mouseY)
{
	for (auto iter = m_UIMap.begin(); iter != m_UIMap.end(); ++iter)
	{
		if ((*iter).first == UITYPE::BASE)	continue;

		XMFLOAT2 min = reinterpret_cast<CUI*>((*iter).second)->GetUIMinPos();
		XMFLOAT2 max = reinterpret_cast<CUI*>((*iter).second)->GetUIMaxPos();

		if (UICollisionCheck(XMFLOAT2(mouseX, mouseY), min, max) == false)	continue;
		
		CUI* p = reinterpret_cast<CUI*>((*iter).second);

		if (m_IsReady == false)
		{
			MoveInteraction((*iter).first);
		}

		if (nMessgeID == WM_LBUTTONUP)
		{
			ClickInteraction((*iter).first);
			m_MouseState = MOUSE_STATE::MOUSE_CLICK;
			CSoundSystem::PlayingSound(CSoundSystem::CLICK);
			return;
		}

		// BASE였다가 캐릭터를 고를때,
		if (m_MouseState == MOUSE_STATE::NONE)
		{
			m_MouseState = MOUSE_STATE::MOUSE_ON;
			CSoundSystem::PlayingSound(CSoundSystem::CHARACTER_SELECT);
			return;
		}

		// 계속 마우스가 메뉴에 올려있을 때,
		else if (m_MouseState == MOUSE_STATE::MOUSE_ON)
			return;
		
	}

	if (m_IsReady == false)
	{
		// 로비 중에서 아무것도 충돌된 것이 없을 때,
		m_MouseState = MOUSE_STATE::NONE;

		for (auto iter = m_UIMap.begin(); iter != m_UIMap.end(); ++iter)
		{
			CUI* p = reinterpret_cast<CUI*>((*iter).second);
			p->SetUV(XMFLOAT2(0.f, 0.5f));
			CDirect2D::GetInstance()->GetImageInfo("Characters").m_FrameXNum = 0;
		}
	}
}

void CCharacterSelectUIShader::MoveInteraction(int key)
{
	int frame = 0;

	switch (key)
	{
	case UITYPE::PINK:			frame = 1;		break;
	case UITYPE::BROWN:		frame = 2;	break;
	case UITYPE::WHITE:			frame = 3;	break;
	case UITYPE::BLACK:			frame = 4;	break;
	case UITYPE::BLUE:			frame = 5;	break;
	case UITYPE::PANDA:		frame = 6;	break;
	case UITYPE::READY:
	case UITYPE::QUIT:
		{
			auto iter = m_UIMap.find(key);
			if (iter != m_UIMap.end())
				reinterpret_cast<CUI*>((*iter).second)->SetUV(XMFLOAT2(0.5f, 1.f));
			break;
		}

	default:
		break;
	}

	CDirect2D::GetInstance()->GetImageInfo("Characters").m_FrameXNum = frame;
}

void CCharacterSelectUIShader::ClickInteraction(int click)
{
	switch (click)
	{
	case UITYPE::PINK:
	case UITYPE::BROWN:
	case UITYPE::WHITE:
	case UITYPE::BLACK:
	case UITYPE::BLUE:
	case UITYPE::PANDA:
		if (m_IsReady == false)
		{
			m_ChoiceCharacter = static_cast<unsigned char>(click) - PINK;
#ifdef _WITH_SERVER_
			// 내가 무슨캐릭터를 선택했는지 보냄
			Network::GetInstance()->SendChoiceCharacter(m_ChoiceCharacter);
#endif
		}
		break;

	case UITYPE::READY:
		if (m_ChoiceCharacter == NONE)
		{
			cout << "캐릭터가 선택되지 않았습니다." << endl;
			break;
		}

		m_IsReady = !m_IsReady;
#ifdef _WITH_SERVER_
		if (m_IsReady == false)
			Network::GetInstance()->SendNotReady();
		else
		{
			g_PlayerCharacter = m_ChoiceCharacter - PINK;
			Network::GetInstance()->SendReady();
		}

#else
		if (m_IsReady == true)
			g_PlayerCharacter = m_ChoiceCharacter;
#endif
		break;

	case UITYPE::QUIT:
		::PostQuitMessage(0);
		break;

	default:
		break;
	}
}

void CCharacterSelectUIShader::Render(ID3D12GraphicsCommandList *pd3dCommandList,int nPipelineState)
{
	UpdateShaderVariables(pd3dCommandList);

	if (m_pd3dCbvSrvDescriptorHeap) 
		pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	CUIShader::OnPrepareRender(pd3dCommandList, (int)UITYPE::BASE);
	auto iter = m_UIMap.find(UITYPE::BASE);
	if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, nPipelineState);

	CUIShader::OnPrepareRender(pd3dCommandList, (int)UITYPE::READY);
	iter = m_UIMap.find(UITYPE::READY);
	if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, nPipelineState);

	CUIShader::OnPrepareRender(pd3dCommandList, (int)UITYPE::QUIT);
	iter = m_UIMap.find(UITYPE::QUIT);
	if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, nPipelineState);
}

void CCharacterSelectUIShader::ReleaseObjects()
{
	if (m_pd3dCbvSrvDescriptorHeap)
		m_pd3dCbvSrvDescriptorHeap->Release();

	CUIShader::ReleaseObjects();

	delete[] m_ppTextures;
}

bool CCharacterSelectUIShader::UICollisionCheck(XMFLOAT2& mousePos, XMFLOAT2& minUIPos, XMFLOAT2& maxUIPos)
{
	if (mousePos.x < minUIPos.x)
		return false;
	else if (mousePos.x > maxUIPos.x)
		return false;
	else if (mousePos.y > maxUIPos.y)
		return false;
	else if (mousePos.y < minUIPos.y)
		return false;

	return true;
}

void CCharacterSelectUIShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_UI) + 255) & ~255); //256의 배수
	m_pd3dUIData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dUIData->Map(0, nullptr, (void**)&m_pMappedUIData);
}

void CCharacterSelectUIShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dUIData)
	{
		auto iter = m_UIMap.find(UITYPE::READY);
		if(iter != m_UIMap.end())
		{
			CUI* p = reinterpret_cast<CUI*>((*iter).second);
			XMFLOAT4 pos = XMFLOAT4(p->GetUIMinPos().x, p->GetUIMinPos().y, p->GetUIMaxPos().x, p->GetUIMaxPos().y);
			m_pMappedUIData->m_Ready_MinMaxPos = pos;

			m_pMappedUIData->m_ReadyAndQuit_UV.x = p->GetUV().x;
			m_pMappedUIData->m_ReadyAndQuit_UV.y = p->GetUV().y;
		}

		iter = m_UIMap.find(UITYPE::QUIT);
		if (iter != m_UIMap.end())
		{
			CUI* p = reinterpret_cast<CUI*>((*iter).second);
			XMFLOAT4 pos = XMFLOAT4(p->GetUIMinPos().x, p->GetUIMinPos().y, p->GetUIMaxPos().x, p->GetUIMaxPos().y);
			m_pMappedUIData->m_Quit_MinMaxPos = pos;

			m_pMappedUIData->m_ReadyAndQuit_UV.z = p->GetUV().x;
			m_pMappedUIData->m_ReadyAndQuit_UV.w = p->GetUV().y;
		}

		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = m_pd3dUIData->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, GPUVirtualAddress);
	}
}

void CCharacterSelectUIShader::ReleaseShaderVariables()
{
	if (m_pd3dUIData)
	{
		m_pd3dUIData->Unmap(0, nullptr);
		m_pd3dUIData->Release();
	}
}