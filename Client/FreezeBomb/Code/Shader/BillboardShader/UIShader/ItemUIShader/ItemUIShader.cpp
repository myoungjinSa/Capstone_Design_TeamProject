#include "../../../../Stdafx/Stdafx.h"
#include "ItemUIShader.h"
#include "../../../../GameObject/Billboard/UI/UI.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../Scene/Scene.h"
#include "../../../../GameObject/Player/Player.h"
#include "../../../../GameObject/Item/Item.h"

CItemUIShader::CItemUIShader()
{
}

CItemUIShader::~CItemUIShader()
{
}

void CItemUIShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
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

D3D12_SHADER_BYTECODE CItemUIShader::CreateVertexShader(int UIType)
{
	switch (UIType)
	{
	case ItemBoxUI:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSItemBoxUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case NormalItemUI:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSNormalItemUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case SpecialItemUI:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSSpecialItemUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	//case SubSpecialItemUI:
	//	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSSubSpecialItemUI", "vs_5_1", &m_pd3dVertexShaderBlob));
	//	break;
	}
}

void CItemUIShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	CBillboardMesh* pItemBoxMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	m_nObjects = 4;

	m_ppUIMaterial = new CMaterial*[m_nObjects];

	m_ppUIMaterial[CItem::Empty] = new CMaterial(1);
	auto iter = Context.find("ItemBox");
	if (iter != Context.end()) 
		m_ppUIMaterial[CItem::Empty]->SetTexture((*iter).second, 0);
	
	m_ppUIMaterial[CItem::NormalHammer] = new CMaterial(1);
	iter = Context.find("NormalHammer");
	if (iter != Context.end())
		m_ppUIMaterial[CItem::NormalHammer]->SetTexture((*iter).second, 0);

	m_ppUIMaterial[CItem::GoldHammer] = new CMaterial(1);
	iter = Context.find("GoldHammer");
	if (iter != Context.end())
		m_ppUIMaterial[CItem::GoldHammer]->SetTexture((*iter).second, 0);

	m_ppUIMaterial[CItem::GoldTimer] = new CMaterial(1);
	iter = Context.find("GoldTimer");
	if (iter != Context.end())
		m_ppUIMaterial[CItem::GoldTimer]->SetTexture((*iter).second, 0);


	for (int i = 0; i < m_nObjects; ++i)
	{
		CUI* pUI = new CUI(1);
		pUI->SetMesh(pItemBoxMesh);
		pUI->SetMaterial(0, m_ppUIMaterial[i]);
		m_UIMap.emplace(i, pUI);
	}
}

void CItemUIShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CItemUIShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CUIShader::OnPrepareRender(pd3dCommandList, ItemBoxUI);
	auto iter = m_UIMap.find(CItem::Empty);
	if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);

	if (m_pPlayer)
	{
		if (m_pPlayer->get_Normal_InventorySize() > 0)
		{
			CUIShader::OnPrepareRender(pd3dCommandList, NormalItemUI);
			auto iter = m_UIMap.find(CItem::NormalHammer);
			if (iter != m_UIMap.end())
				(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
		}

		if (m_pPlayer->get_Special_InventorySize() > 0)
		{
			for (auto item : m_pPlayer->get_Special_Inventory())
			{
				byte itemType = item.second->getItemType();
				auto iter2 = m_UIMap.find(itemType);
				if (iter2 != m_UIMap.end())
				{
					//if(itemType == CItem::ItemType::GoldTimer)
				//	{
					CUIShader::OnPrepareRender(pd3dCommandList, SpecialItemUI);
					(*iter2).second->Render(pd3dCommandList, pCamera, nPipelineState);
				//	}
					/*else
					{
						CUIShader::OnPrepareRender(pd3dCommandList, SubSpecialItemUI);
						(*iter2).second->Render(pd3dCommandList, pCamera, nPipelineState);
					}*/
				}
				/*else if(iter2 != m_UIMap.end() && m_pPlayer->GetIsBomb() == false)
				{
					if(itemType == CItem::ItemType::GoldHammer)
					{
						CUIShader::OnPrepareRender(pd3dCommandList, SpecialItemUI);
						(*iter2).second->Render(pd3dCommandList, pCamera, nPipelineState);
					}
					else
					{
						CUIShader::OnPrepareRender(pd3dCommandList, SubSpecialItemUI);
						(*iter2).second->Render(pd3dCommandList, pCamera, nPipelineState);
					}
				}*/
			}
				
				/*byte itemType = m_pPlayer->get_Special_Inventory().begin()->second->getItemType();
				auto iter = m_UIMap.find(itemType);
				if (iter != m_UIMap.end()) 
				{
					if (m_pPlayer->GetIsBomb() == true && itemType == CItem::ItemType::GoldTimer)
					{
						CUIShader::OnPrepareRender(pd3dCommandList, SpecialItemUI);
						(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
					}
					if(m_pPlayer->GetIsBomb() == true && itemType == CItem::ItemType::GoldHammer)
					{
						CUIShader::OnPrepareRender(pd3dCommandList, SubSpecialItemUI);
						(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
					}
					if(m_pPlayer->GetIsBomb() == false && itemType == CItem::ItemType::GoldHammer)
					{
						CUIShader::OnPrepareRender(pd3dCommandList, SpecialItemUI);
						(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
					}
					if(m_pPlayer->GetIsBomb() == false && itemType == CItem::ItemType::GoldTimer)
					{
						CUIShader::OnPrepareRender(pd3dCommandList, SubSpecialItemUI);
						(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
					}
				}*/
				
			
		}
	}
}
