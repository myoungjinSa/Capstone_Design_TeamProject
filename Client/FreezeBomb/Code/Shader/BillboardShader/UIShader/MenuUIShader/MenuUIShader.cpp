#include "../../../../Stdafx/Stdafx.h"
#include "MenuUIShader.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../GameObject/Billboard/UI/UI.h"
#include "../../../../SoundSystem/SoundSystem.h"
#include "../../../../Scene/Scene.h"

extern bool g_OnCartoonShading;
extern bool g_IsSoundOn;

bool CMenuUIShader::m_IsPlay = false;
byte CMenuUIShader::m_MenuState = MenuBoard;

CMenuUIShader::CMenuUIShader()
{
}

CMenuUIShader::~CMenuUIShader()
{
}

void CMenuUIShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 6;
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

D3D12_SHADER_BYTECODE CMenuUIShader::CreateVertexShader(int UIType)
{
	switch (UIType)
	{
	case MenuBoard:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSMenuBoardUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case Menu_ICON:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSMenuICONUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case Menu_Option:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSMenuOptionUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case Menu_GameOver:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSMenuGameOverUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case Menu_Sound:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSMenuSoundUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case Menu_Cartoon:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSMenuCartoonUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	default:
		break;
	}
}

void CMenuUIShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	CBillboardMesh* pMenuMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	CMaterial* pMaterial = new CMaterial(1);
	auto iter = Context.find("MenuBoard");
	if (iter != Context.end())
		pMaterial->SetTexture((*iter).second, 0);
	CUI* pUI = new CUI(1);
	pUI->SetMesh(pMenuMesh);
	pUI->SetMaterial(0, pMaterial);
	m_UIMap.emplace(MenuBoard, pUI);

	CMaterial* pICONMaterial = new CMaterial(1);
	iter = Context.find("Menu_ICON");
	if (iter != Context.end())
		pICONMaterial->SetTexture((*iter).second, 0);
	CUI* pICONUI = new CUI(1);
	pICONUI->SetMesh(pMenuMesh);
	pICONUI->SetMaterial(0, pICONMaterial);
	m_UIMap.emplace(Menu_ICON, pICONUI);

	CMaterial* pOptionMaterial = new CMaterial(1);
	iter = Context.find("Option");
	if (iter != Context.end())
		pOptionMaterial->SetTexture((*iter).second, 0);
	CUI* pOptionUI = new CUI(1);
	pOptionUI->SetMesh(pMenuMesh);
	pOptionUI->SetMaterial(0, pOptionMaterial);
	m_UIMap.emplace(Menu_Option, pOptionUI);

	CMaterial* pGameOverMaterial = new CMaterial(1);
	iter = Context.find("GameOver");
	if (iter != Context.end())
		pGameOverMaterial->SetTexture((*iter).second, 0);
	CUI* pGameOverUI = new CUI(1);
	pGameOverUI->SetMesh(pMenuMesh);
	pGameOverUI->SetMaterial(0, pGameOverMaterial);
	m_UIMap.emplace(Menu_GameOver, pGameOverUI);

	CMaterial* pSoundMaterial = new CMaterial(1);
	iter = Context.find("Sound");
	if (iter != Context.end())
		pSoundMaterial->SetTexture((*iter).second, 0);
	CUI* pSoundUI = new CUI(1);
	pSoundUI->SetMesh(pMenuMesh);
	pSoundUI->SetMaterial(0, pSoundMaterial);
	m_UIMap.emplace(Menu_Sound, pSoundUI);

	CMaterial* pCartoonMaterial = new CMaterial(1);
	iter = Context.find("Cartoon");
	if (iter != Context.end())
		pCartoonMaterial->SetTexture((*iter).second, 0);
	CUI* pCartoonUI = new CUI(1);
	pCartoonUI->SetMesh(pMenuMesh);
	pCartoonUI->SetMaterial(0, pCartoonMaterial);
	m_UIMap.emplace(Menu_Cartoon, pCartoonUI);

	int musicNum = 1;
	string filename = "../Resource/Sound/Click.wav";

	m_pSound = new CSoundSystem;
	m_pSound->Initialize(filename, FMOD_LOOP_OFF);

	m_MenuInfo.m_MenuBoard_MinMaxPos = XMFLOAT4(-0.3f, -0.5f, 0.3f, 0.5f);
	m_MenuInfo.m_MenuICON_MinMaxPos = XMFLOAT4(0.75f, 0.75f, 0.95f, 0.95f);
	m_MenuInfo.m_MenuBoard_MenuICON_UV = XMFLOAT4(0.f, 0.5f, 0.f, 0.5f);

	m_MenuInfo.m_MenuOption_MinMaxPos = XMFLOAT4(-0.09f, -0.05f, 0.09f, 0.15f);
	m_MenuInfo.m_MenuGameOver_MinMaxPos = XMFLOAT4(-0.15f, -0.35f, 0.15f, -0.15f);
	m_MenuInfo.m_MenuOption_MenuGameOver_UV = XMFLOAT4(0.f, 0.5f, 0.f, 0.5f);

	m_MenuInfo.m_MenuSound_MinMaxPos = XMFLOAT4(-0.2f, -0.05f, 0.2f, 0.15f);
	m_MenuInfo.m_MenuCartoon_MinMaxPos = XMFLOAT4(-0.25f, -0.35f, 0.25f, -0.15f);
	m_MenuInfo.m_MenuSound_MenuCartoon_UV = XMFLOAT4(0.f, 0.5f, 0.5f, 1.f);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CMenuUIShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
}

void CMenuUIShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	UpdateShaderVariables(pd3dCommandList);

	CUIShader::OnPrepareRender(pd3dCommandList, Menu_ICON);
	auto iter = m_UIMap.find(Menu_ICON);
	if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);

	if (m_IsRender == true)
	{
		CUIShader::OnPrepareRender(pd3dCommandList, MenuBoard);
		iter = m_UIMap.find(MenuBoard);
		if (iter != m_UIMap.end())
			(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);

		if (m_MenuState == MenuBoard)
		{
			CUIShader::OnPrepareRender(pd3dCommandList, Menu_Option);
			iter = m_UIMap.find(Menu_Option);
			if (iter != m_UIMap.end())
				(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);

			CUIShader::OnPrepareRender(pd3dCommandList, Menu_GameOver);
			iter = m_UIMap.find(Menu_GameOver);
			if (iter != m_UIMap.end())
				(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
		}
		else
		{
			CUIShader::OnPrepareRender(pd3dCommandList, Menu_Sound);
			iter = m_UIMap.find(Menu_Sound);
			if (iter != m_UIMap.end())
				(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);

			CUIShader::OnPrepareRender(pd3dCommandList, Menu_Cartoon);
			iter = m_UIMap.find(Menu_Cartoon);
			if (iter != m_UIMap.end())
				(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
		}
	}
}

void CMenuUIShader::ReleaseObjects()
{
	for (auto iter = m_UIMap.begin(); iter != m_UIMap.end();)
	{
		(*iter).second->Release();
		iter = m_UIMap.erase(iter);
	}
	m_UIMap.clear();

	if (m_pSound)
		m_pSound->Release();
}

void CMenuUIShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_UI) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dUIData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dUIData->Map(0, nullptr, (void**)&m_pMappedUIData);
}

void CMenuUIShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dUIData)
	{
		m_pMappedUIData->m_MenuBoard_MinMaxPos = m_MenuInfo.m_MenuBoard_MinMaxPos;
		m_pMappedUIData->m_MenuICON_MinMaxPos = m_MenuInfo.m_MenuICON_MinMaxPos;
		m_pMappedUIData->m_MenuBoard_MenuICON_UV = m_MenuInfo.m_MenuBoard_MenuICON_UV;

		m_pMappedUIData->m_MenuOption_MinMaxPos = m_MenuInfo.m_MenuOption_MinMaxPos;
		m_pMappedUIData->m_MenuGameOver_MinMaxPos = m_MenuInfo.m_MenuGameOver_MinMaxPos;
		m_pMappedUIData->m_MenuOption_MenuGameOver_UV = m_MenuInfo.m_MenuOption_MenuGameOver_UV;

		m_pMappedUIData->m_MenuSound_MinMaxPos = m_MenuInfo.m_MenuSound_MinMaxPos;
		m_pMappedUIData->m_MenuCartoon_MinMaxPos = m_MenuInfo.m_MenuCartoon_MinMaxPos;
		m_pMappedUIData->m_MenuSound_MenuCartoon_UV = m_MenuInfo.m_MenuSound_MenuCartoon_UV;

		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = m_pd3dUIData->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(24, GPUVirtualAddress);
	}
}

void CMenuUIShader::ReleaseShaderVariables()
{
	if (m_pd3dUIData)
	{
		m_pd3dUIData->Unmap(0, nullptr);
		m_pd3dUIData->Release();
	}
}

bool CMenuUIShader::UICollisionCheck(XMFLOAT2& mousePos, XMFLOAT2& minUIPos, XMFLOAT2& maxUIPos)
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

void CMenuUIShader::ProcessMessage(bool isMouse, UINT message, XMFLOAT2& mousePos)
{
	switch (isMouse)
	{
	case MOUSE:
		ProcessMouseMessage(message, mousePos);
		break;

	case KEYBOARD:
		ProcessKeyBoardMessage();
		break;

	default:
		break;
	}

}

void CMenuUIShader::ProcessMouseMessage(UINT message, XMFLOAT2& mousePos)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		{
			XMFLOAT2 minPos, maxPos;
			minPos.x = m_MenuInfo.m_MenuICON_MinMaxPos.x;
			minPos.y = m_MenuInfo.m_MenuICON_MinMaxPos.y;
			maxPos.x = m_MenuInfo.m_MenuICON_MinMaxPos.z;
			maxPos.y = m_MenuInfo.m_MenuICON_MinMaxPos.w;

			if (UICollisionCheck(mousePos, minPos, maxPos) == true)
			{
				m_MenuInfo.m_MenuBoard_MenuICON_UV.z = 0.5f;
				m_MenuInfo.m_MenuBoard_MenuICON_UV.w = 1.f;
			}
			else
			{
				if (m_IsRender == false)
				{
					m_MenuInfo.m_MenuBoard_MenuICON_UV.z = 0.f;
					m_MenuInfo.m_MenuBoard_MenuICON_UV.w = 0.5f;
				}
			}
		}
		{
			if (m_IsRender == true)
			{
				if (m_MenuState == MenuBoard)
				{
					XMFLOAT2 Option_minPos, Option_maxPos;
					Option_minPos.x = m_MenuInfo.m_MenuOption_MinMaxPos.x;
					Option_minPos.y = m_MenuInfo.m_MenuOption_MinMaxPos.y;
					Option_maxPos.x = m_MenuInfo.m_MenuOption_MinMaxPos.z;
					Option_maxPos.y = m_MenuInfo.m_MenuOption_MinMaxPos.w;

					XMFLOAT2 GameOver_minPos, GameOver_maxPos;
					GameOver_minPos.x = m_MenuInfo.m_MenuGameOver_MinMaxPos.x;
					GameOver_minPos.y = m_MenuInfo.m_MenuGameOver_MinMaxPos.y;
					GameOver_maxPos.x = m_MenuInfo.m_MenuGameOver_MinMaxPos.z;
					GameOver_maxPos.y = m_MenuInfo.m_MenuGameOver_MinMaxPos.w;

					if (UICollisionCheck(mousePos, Option_minPos, Option_maxPos) == true)
					{
						m_MenuInfo.m_MenuOption_MinMaxPos = XMFLOAT4(-0.115f, -0.075f, 0.115f, 0.175f);
						m_MenuInfo.m_MenuGameOver_MinMaxPos = XMFLOAT4(-0.15f, -0.35f, 0.15f, -0.15f);
						m_MenuInfo.m_MenuOption_MenuGameOver_UV = XMFLOAT4(0.5f, 1.f, 0.f, 0.5f);
					}
					else if (UICollisionCheck(mousePos, GameOver_minPos, GameOver_maxPos) == true)
					{
						m_MenuInfo.m_MenuOption_MinMaxPos = XMFLOAT4(-0.09f, -0.05f, 0.09f, 0.15f);
						m_MenuInfo.m_MenuGameOver_MinMaxPos = XMFLOAT4(-0.175f, -0.375f, 0.175f, -0.125f);
						m_MenuInfo.m_MenuOption_MenuGameOver_UV = XMFLOAT4(0.f, 0.5f, 0.5f, 1.f);
					}
					else
					{
						m_MenuInfo.m_MenuOption_MinMaxPos = XMFLOAT4(-0.09f, -0.05f, 0.09f, 0.15f);
						m_MenuInfo.m_MenuGameOver_MinMaxPos = XMFLOAT4(-0.15f, -0.35f, 0.15f, -0.15f);
						m_MenuInfo.m_MenuOption_MenuGameOver_UV = XMFLOAT4(0.f, 0.5f, 0.f, 0.5f);
					}
				}
				else if (m_MenuState == Menu_Option)
				{
					XMFLOAT2 Sound_minPos, Sound_maxPos;
					Sound_minPos.x = m_MenuInfo.m_MenuSound_MinMaxPos.x;
					Sound_minPos.y = m_MenuInfo.m_MenuSound_MinMaxPos.y;
					Sound_maxPos.x = m_MenuInfo.m_MenuSound_MinMaxPos.z;
					Sound_maxPos.y = m_MenuInfo.m_MenuSound_MinMaxPos.w;

					XMFLOAT2 Cartoon_minPos, Cartoon_maxPos;
					Cartoon_minPos.x = m_MenuInfo.m_MenuCartoon_MinMaxPos.x;
					Cartoon_minPos.y = m_MenuInfo.m_MenuCartoon_MinMaxPos.y;
					Cartoon_maxPos.x = m_MenuInfo.m_MenuCartoon_MinMaxPos.z;
					Cartoon_maxPos.y = m_MenuInfo.m_MenuCartoon_MinMaxPos.w;

					if (UICollisionCheck(mousePos, Sound_minPos, Sound_maxPos) == true)
					{
						//XMFLOAT4(-0.2f, -0.05f, 0.2f, 0.15f);
						m_MenuInfo.m_MenuSound_MinMaxPos = XMFLOAT4(-0.225f, -0.075f, 0.225f, 0.175f);
						m_MenuInfo.m_MenuCartoon_MinMaxPos = XMFLOAT4(-0.25f, -0.35f, 0.25f, -0.15f);
					}
					else if (UICollisionCheck(mousePos, Cartoon_minPos, Cartoon_maxPos) == true)
					{
						m_MenuInfo.m_MenuSound_MinMaxPos = XMFLOAT4(-0.2f, -0.05f, 0.2f, 0.15f);
						m_MenuInfo.m_MenuCartoon_MinMaxPos = XMFLOAT4(-0.275f, -0.375f, 0.275f, -0.125f);
					}
					else
					{
						m_MenuInfo.m_MenuSound_MinMaxPos = XMFLOAT4(-0.2f, -0.05f, 0.2f, 0.15f);
						m_MenuInfo.m_MenuCartoon_MinMaxPos = XMFLOAT4(-0.25f, -0.35f, 0.25f, -0.15f);
					}
				}
			}
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		{
			XMFLOAT2 minPos, maxPos;
			minPos.x = m_MenuInfo.m_MenuICON_MinMaxPos.x;
			minPos.y = m_MenuInfo.m_MenuICON_MinMaxPos.y;
			maxPos.x = m_MenuInfo.m_MenuICON_MinMaxPos.z;
			maxPos.y = m_MenuInfo.m_MenuICON_MinMaxPos.w;

			if (UICollisionCheck(mousePos, minPos, maxPos) == true)
			{
				if (m_pSound && g_IsSoundOn == true)
					m_pSound->PlayIndex(MENU_INPUT);
				m_IsRender = !m_IsRender;

				m_MenuState = MenuBoard;
			}
		}

		if (m_MenuState == MenuBoard)
		{
			XMFLOAT2 Option_minPos, Option_maxPos;
			Option_minPos.x = m_MenuInfo.m_MenuOption_MinMaxPos.x;
			Option_minPos.y = m_MenuInfo.m_MenuOption_MinMaxPos.y;
			Option_maxPos.x = m_MenuInfo.m_MenuOption_MinMaxPos.z;
			Option_maxPos.y = m_MenuInfo.m_MenuOption_MinMaxPos.w;

			XMFLOAT2 GameOver_minPos, GameOver_maxPos;
			GameOver_minPos.x = m_MenuInfo.m_MenuGameOver_MinMaxPos.x;
			GameOver_minPos.y = m_MenuInfo.m_MenuGameOver_MinMaxPos.y;
			GameOver_maxPos.x = m_MenuInfo.m_MenuGameOver_MinMaxPos.z;
			GameOver_maxPos.y = m_MenuInfo.m_MenuGameOver_MinMaxPos.w;

			if (UICollisionCheck(mousePos, Option_minPos, Option_maxPos) == true)
			{
				if (m_pSound && g_IsSoundOn == true)
					m_pSound->PlayIndex(MENU_INPUT);

				m_MenuState = Menu_Option;
			}
			else if (UICollisionCheck(mousePos, GameOver_minPos, GameOver_maxPos) == true)
			{
				::PostQuitMessage(0);
			}
		}
		else if (m_MenuState == Menu_Option)
		{
			XMFLOAT2 Sound_minPos, Sound_maxPos;
			Sound_minPos.x = m_MenuInfo.m_MenuSound_MinMaxPos.x;
			Sound_minPos.y = m_MenuInfo.m_MenuSound_MinMaxPos.y;
			Sound_maxPos.x = m_MenuInfo.m_MenuSound_MinMaxPos.z;
			Sound_maxPos.y = m_MenuInfo.m_MenuSound_MinMaxPos.w;

			XMFLOAT2 Cartoon_minPos, Cartoon_maxPos;
			Cartoon_minPos.x = m_MenuInfo.m_MenuCartoon_MinMaxPos.x;
			Cartoon_minPos.y = m_MenuInfo.m_MenuCartoon_MinMaxPos.y;
			Cartoon_maxPos.x = m_MenuInfo.m_MenuCartoon_MinMaxPos.z;
			Cartoon_maxPos.y = m_MenuInfo.m_MenuCartoon_MinMaxPos.w;

			if (UICollisionCheck(mousePos, Sound_minPos, Sound_maxPos) == true)
			{
				if (m_pSound && g_IsSoundOn == true)
					m_pSound->PlayIndex(MENU_INPUT);

				g_IsSoundOn = !g_IsSoundOn;
				if (g_IsSoundOn == true)
				{
					m_MenuInfo.m_MenuSound_MenuCartoon_UV.x = 0.f;
					m_MenuInfo.m_MenuSound_MenuCartoon_UV.y = 0.5f;

					m_pSound->AllPlay(1.f);

					m_pScene->SceneSoundPlay();
				}
				else
				{
					m_MenuInfo.m_MenuSound_MenuCartoon_UV.x = 0.5f;
					m_MenuInfo.m_MenuSound_MenuCartoon_UV.y = 1.f;

					m_pSound->AllStop();
					m_pScene->SceneSoundStop();

					g_IsSoundOn = false;
				}
			}

			else if (UICollisionCheck(mousePos, Cartoon_minPos, Cartoon_maxPos) == true)
			{
				if (m_pSound && g_IsSoundOn == true)
					m_pSound->PlayIndex(MENU_INPUT);

				g_OnCartoonShading = !g_OnCartoonShading;
				if (g_OnCartoonShading == true)
				{
					m_MenuInfo.m_MenuSound_MenuCartoon_UV.z = 0.f;
					m_MenuInfo.m_MenuSound_MenuCartoon_UV.w = 0.5f;
				}
				else
				{
					m_MenuInfo.m_MenuSound_MenuCartoon_UV.z = 0.5f;
					m_MenuInfo.m_MenuSound_MenuCartoon_UV.w = 1.f;
				}
			}
		}
	}
	break;
	default:
		break;
	}

	ChangeState();
}

void CMenuUIShader::ProcessKeyBoardMessage()
{
	if (m_pSound && g_IsSoundOn == true)
		m_pSound->PlayIndex(MENU_INPUT);

	m_IsRender = !m_IsRender;

	if (m_IsRender == true)
	{
		m_MenuInfo.m_MenuBoard_MenuICON_UV.z = 0.5f;
		m_MenuInfo.m_MenuBoard_MenuICON_UV.w = 1.f;
	}
	else
	{
		m_MenuInfo.m_MenuBoard_MenuICON_UV.z = 0.f;
		m_MenuInfo.m_MenuBoard_MenuICON_UV.w = 0.5f;
		m_MenuState = MenuBoard;
	}

	ChangeState();
}

void CMenuUIShader::ChangeState()
{
	switch (m_MenuState)
	{
	case MenuBoard:
		m_MenuInfo.m_MenuBoard_MenuICON_UV.x = 0.f;
		m_MenuInfo.m_MenuBoard_MenuICON_UV.y = 0.5f;
		break;

	case Menu_Option:
		m_MenuInfo.m_MenuBoard_MenuICON_UV.x = 0.5f;
		m_MenuInfo.m_MenuBoard_MenuICON_UV.y = 1.f;
		break;
	default:
		break;
	}
}