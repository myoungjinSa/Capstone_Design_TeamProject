#include "../../../Stdafx/Stdafx.h"
#include "FoliageShader.h"
#include "../../../GameObject/GameObject.h"
#include "../../../GameObject/Foliage/Foliage.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../FrameTransform/FrameTransform.h"

extern unsigned char g_Round;

CFoliageShader::CFoliageShader()
{
}

CFoliageShader::~CFoliageShader()
{
}

void CFoliageShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 3;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader(i);
		m_d3dPipelineStateDesc.PS = CreatePixelShader(i);
		m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
		m_d3dPipelineStateDesc.BlendState = CreateBlendState();
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

		if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
		if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();
		if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

D3D12_SHADER_BYTECODE CFoliageShader::CreatePixelShader(int nPipelineState)
{
	switch(nPipelineState)
	{
	case GameObject:
	case GameObject_Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSFogFoliage", "ps_5_1", &m_pd3dPixelShaderBlob));
	case No_FogObject:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSFoliage", "ps_5_1", &m_pd3dPixelShaderBlob));
	}
	
}

D3D12_BLEND_DESC CFoliageShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
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

	return(d3dBlendDesc);
}

float CFoliageShader::GetDistanceToCamera(CGameObject* pObject, CCamera *pCamera)
{
	float dist = 0.0;

	XMFLOAT3 cameraPosition = pCamera->GetPosition();
	XMFLOAT3 objectPosition = pObject->GetPosition();

	dist = Vector3::Length(Vector3::Subtract(cameraPosition, objectPosition));

	return dist;
}

void CFoliageShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
	const unordered_map<unsigned char, RoundInfo>& RoundMapObjectInfo, void* pContext)
{
	unordered_map<string, CLoadedModelInfo*> foliageModelList;
	m_pFoliageModel0 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Grass_C_01.bin", this, false);
	foliageModelList.emplace("Foliage0", m_pFoliageModel0);
	m_pFoliageModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Grass_D_01.bin", this, false);
	foliageModelList.emplace("Foliage1", m_pFoliageModel1);
	m_pFoliageModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Plant_c_01.bin", this, false);
	foliageModelList.emplace("Foliage2", m_pFoliageModel2);

	unsigned char round = 0;
	for (auto iter = RoundMapObjectInfo.begin(); iter != RoundMapObjectInfo.end(); ++iter)
	{		
		list<CGameObject*> objectList;
		for (auto iter2 = foliageModelList.begin(); iter2 != foliageModelList.end(); ++iter2)
		{
			string name = (*iter2).first;
			for (auto iter3 = (*iter).second.lower_bound(name); iter3 != (*iter).second.upper_bound(name); ++iter3)
			{
				CFoliageObject* pFoliage = new CFoliageObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				pFoliage->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter2).second);
				pFoliage->SetChild((*iter2).second->m_pModelRootObject, true);
				pFoliage->SetPosition((*iter3).second->m_Position);
				pFoliage->SetLookVector((*iter3).second->m_Look);
				pFoliage->SetUpVector((*iter3).second->m_Up);
				pFoliage->SetRightVector((*iter3).second->m_Right);

				objectList.emplace_back(pFoliage);
			}
		}
		m_MapObjectList.emplace(round++, objectList);
	}

	delete m_pFoliageModel0;
	delete m_pFoliageModel1;
	delete m_pFoliageModel2;
}

void CFoliageShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
{
	float distance = 0.0f;
	UINT lodlevel = CGameObject::LOD_LEVEL0;

	auto iter = m_MapObjectList.find(g_Round);
	if (iter != m_MapObjectList.end())
	{
		for (auto iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); ++iter2)
		{
			distance = GetDistanceToCamera((*iter2), pCamera);

			if (distance < 30.0f)
				lodlevel = CGameObject::LOD_LEVEL0;
			
			else if (distance >= 30.0f && distance < 100.0f)
				lodlevel = CGameObject::LOD_LEVEL1;
			
			else if (distance > 100 && distance < 200.0f)
				lodlevel = CGameObject::LOD_LEVEL2;
			
			//여기서 자신의 LOD레벨을 구하면 될듯하다.
			(*iter2)->SetLODlevel(lodlevel);
		}
	}
}


void CFoliageShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CStandardShader::Render(pd3dCommandList, pCamera, nPipelineState);
	auto iter = m_MapObjectList.find(g_Round);
	if (iter != m_MapObjectList.end())
	{
		for (auto iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); ++iter2)
		{
			(*iter2)->Animate(m_fElapsedTime);
			(*iter2)->UpdateTransform(nullptr);
			(*iter2)->Render(pd3dCommandList, (*iter2)->GetLodLevel(), pCamera, nPipelineState);
		}
	}
}

void CFoliageShader::ReleaseObjects()
{
	for (auto iter = m_MapObjectList.begin(); iter != m_MapObjectList.end(); )
	{
		for (auto iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); )
		{
			(*iter2)->Release();
			iter2 = (*iter).second.erase(iter2);
		}
		(*iter).second.clear();
		iter = m_MapObjectList.erase(iter);
	}
	m_MapObjectList.clear();
}

void CFoliageShader::ReleaseUploadBuffers()
{
	for (auto iter = m_MapObjectList.begin(); iter != m_MapObjectList.end(); ++iter)
	{
		for (auto iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); ++iter2)
		{
			(*iter2)->ReleaseUploadBuffers();
		}
	}
}