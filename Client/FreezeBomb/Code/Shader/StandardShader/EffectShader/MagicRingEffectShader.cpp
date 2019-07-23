#include "../../../Stdafx/Stdafx.h"
#include "MagicRingEffectShader.h"
#include "../../../ResourceManager/ResourceManager.h"
#include "../../../FrameTransform/FrameTransform.h"
#include "../../../GameObject/Effect/MagicRing/MagicRing.h"
#include "../../../GameObject/Player/Player.h"
#include "../../../Texture/Texture.h"
#include "../SkinnedAnimationShader/SkinnedAnimationObjectShader/SkinnedAnimationObjectShader.h"
#include "../../../GameObject/EvilBear/EvilBear.h"

CMagicRingShader::CMagicRingShader()
{

}

CMagicRingShader::~CMagicRingShader()
{
}

void CMagicRingShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader(i);
		m_d3dPipelineStateDesc.PS = CreatePixelShader(i);
		m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
		m_d3dPipelineStateDesc.BlendState = CreateBlendState(i);
		m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState(i);
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

D3D12_BLEND_DESC CMagicRingShader::CreateBlendState(int nPipelineState)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	switch (nPipelineState)
	{
	case GameObject:
		d3dBlendDesc.AlphaToCoverageEnable = FALSE;
		d3dBlendDesc.IndependentBlendEnable = FALSE;
		d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
		d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
		d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		break;

	}

	return(d3dBlendDesc);
}

D3D12_RASTERIZER_DESC CMagicRingShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}
D3D12_SHADER_BYTECODE CMagicRingShader::CreateVertexShader(int nPipelineState)
{
	switch (nPipelineState)
	{
	case GameObject:
		return(CStandardShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	}
}

D3D12_SHADER_BYTECODE CMagicRingShader::CreatePixelShader(int nPipelineState)
{
	switch (nPipelineState)
	{
	case GameObject:
		return(CStandardShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSEffectStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	}
}


void CMagicRingShader::BuildObjects(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList,ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CTexture*>& textureMap, void* pContext)
{
	CLoadedModelInfo* pModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/wind.bin", this, false);
	
	//적 캐릭터들의 위치를 받아와야하기 때문에 CSkinnedAnimationObjectShader의 포인터를 멤버변수로 갖고있어야한다.
	if(pContext)
		m_ptrSkinnedAnimationObjects = (CSkinnedAnimationObjectShader*)pContext;

	for (int i = 0; i < maxObjects; ++i)
	{
		m_MagicRingObjects[i] = new CMagicRing;
		m_MagicRingObjects[i]->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, pModel);

		m_MagicRingObjects[i]->SetChild(pModel->m_pModelRootObject, true);
	}
	
}

void CMagicRingShader::CheckMagicRingActive(CPlayer* pPlayer)
{

	if (pPlayer) {
		if (pPlayer->m_pAnimationController->GetAnimationState() == CAnimationController::RAISEHAND)
		{
			m_MagicRingObjects[pPlayer->GetPlayerID()]->SetActive();
		}
		else
		{
			m_MagicRingObjects[pPlayer->GetPlayerID()]->SetDeActive();
		}
	}
#ifdef _WITH_SERVER_


	for(auto enemyID : m_ptrSkinnedAnimationObjects->m_mapMaterial)
	{
		char id = enemyID.first;

		if (m_ptrSkinnedAnimationObjects->m_ppObjects[id]->m_pAnimationController->GetAnimationState() == CAnimationController::RAISEHAND)
		{
			
			m_MagicRingObjects[id]->SetActive();
		}
		else
		{
			m_MagicRingObjects[id]->SetDeActive();
		}
	}


#else


	if (m_ptrSkinnedAnimationObjects)
	{
		for (int i = 0; i < m_ptrSkinnedAnimationObjects->m_nObjects; ++i)
		{
			if (m_ptrSkinnedAnimationObjects->m_ppObjects[i])
			{
				if(m_ptrSkinnedAnimationObjects->m_ppObjects[i]->m_pAnimationController->GetAnimationState() == CAnimationController::RAISEHAND)
				{
					m_MagicRingObjects[i+1]->SetActive();
				}
				else
				{
					m_MagicRingObjects[i+1]->SetDeActive();
				}
			}

		}
	}
#endif
}
void CMagicRingShader::AnimateObjects(float fTimeElapsed,CCamera* pCamera,CPlayer* pPlayer)
{
	m_fElapsedTime += fTimeElapsed;

	CheckMagicRingActive(pPlayer);
	if(pPlayer && m_MagicRingObjects[pPlayer->GetPlayerID()]->GetIsActive())
	{
		m_MagicRingObjects[pPlayer->GetPlayerID()]->SetPosition(pPlayer->GetPosition());
		m_MagicRingObjects[pPlayer->GetPlayerID()]->SetLookVector(pPlayer->GetLookVector());
		m_MagicRingObjects[pPlayer->GetPlayerID()]->SetRightVector(pPlayer->GetRightVector());
		m_MagicRingObjects[pPlayer->GetPlayerID()]->SetUpVector(pPlayer->GetUpVector());
		m_MagicRingObjects[pPlayer->GetPlayerID()]->Rotate(&pPlayer->GetUpVector(), m_fElapsedTime*240.0f);
		
	}
#ifdef _WITH_SERVER_
	for (auto enemyID : m_ptrSkinnedAnimationObjects->m_mapMaterial)
	{
		char id = enemyID.first;

		if (m_MagicRingObjects[id]->GetIsActive()) 
		{
			m_MagicRingObjects[id]->SetPosition(m_ptrSkinnedAnimationObjects->m_ppObjects[id]->GetPosition());
			m_MagicRingObjects[id]->SetLookVector(dynamic_cast<CEvilBear*>(m_ptrSkinnedAnimationObjects->m_ppObjects[id])->GetLookVector());
			m_MagicRingObjects[id]->SetUpVector(dynamic_cast<CEvilBear*>(m_ptrSkinnedAnimationObjects->m_ppObjects[id])->GetUpVector());
			m_MagicRingObjects[id]->SetRightVector(dynamic_cast<CEvilBear*>(m_ptrSkinnedAnimationObjects->m_ppObjects[id])->GetRightVector());
			m_MagicRingObjects[id]->Rotate(&dynamic_cast<CEvilBear*>(m_ptrSkinnedAnimationObjects->m_ppObjects[id])->GetUpVector(), m_fElapsedTime*240.0f);
			
		}
	}

#else

	if(m_ptrSkinnedAnimationObjects)
	{
		for(int i=0;i<m_ptrSkinnedAnimationObjects->m_nObjects;++i)
		{
			if (m_ptrSkinnedAnimationObjects->m_ppObjects[i] && m_MagicRingObjects[i+1]->GetIsActive())
			{
				
				m_MagicRingObjects[i + 1]->SetPosition(m_ptrSkinnedAnimationObjects->m_ppObjects[i]->GetPosition());
				m_MagicRingObjects[i + 1]->SetLookVector(dynamic_cast<CEvilBear*>(m_ptrSkinnedAnimationObjects->m_ppObjects[i])->GetLookVector());
				m_MagicRingObjects[i + 1]->SetUpVector(dynamic_cast<CEvilBear*>(m_ptrSkinnedAnimationObjects->m_ppObjects[i])->GetUpVector());
				m_MagicRingObjects[i + 1]->SetRightVector(dynamic_cast<CEvilBear*>(m_ptrSkinnedAnimationObjects->m_ppObjects[i])->GetRightVector());
				m_MagicRingObjects[i + 1]->Rotate(&dynamic_cast<CEvilBear*>(m_ptrSkinnedAnimationObjects->m_ppObjects[i])->GetUpVector(), m_fElapsedTime*240.0f);
			}
		}
	}
#endif

}

void CMagicRingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList,CCamera* pCamera,int nPipelineState)
{

	if(m_ppd3dPipelineStates[GameObject])
		pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[GameObject]);


//
//#ifdef _WITH_SERVER_
//	for (int i = 0; i < m_ptrSkinnedAnimationObjects->m_vMaterial.size(); ++i)
//	{
//		char id = m_ptrSkinnedAnimationObjects->m_vMaterial[i].first;
//
//		if (m_MagicRingObjects[id]->GetIsActive())
//		{
//			m_MagicRingObjects[id]->Animate(m_fElapsedTime);
//			m_MagicRingObjects[id]->UpdateTransform(nullptr);
//			m_MagicRingObjects[id]->Render(pd3dCommandList, pCamera, nPipelineState);
//		}
//	}
//#else
	for (int i = 0; i < maxObjects; ++i)
	{
		if (m_MagicRingObjects[i]->GetIsActive())
		{
			//m_MagicRingObjects[i]->Animate(m_fElapsedTime);
			//m_MagicRingObjects[i]->UpdateTransform(nullptr);
			m_MagicRingObjects[i]->Render(pd3dCommandList, pCamera, nPipelineState);

		}
	}
//#endif

}
void CMagicRingShader::ReleaseObjects()
{
	m_ptrSkinnedAnimationObjects = nullptr;

	for (int i = 0; i < maxObjects; ++i)
		delete m_MagicRingObjects[i];

}

void CMagicRingShader::ReleaseUploadBuffers()
{
	for (auto iter = m_MagicRingObjects.begin(); iter != m_MagicRingObjects.end(); ++iter)
		(*iter)->ReleaseUploadBuffers();
}
