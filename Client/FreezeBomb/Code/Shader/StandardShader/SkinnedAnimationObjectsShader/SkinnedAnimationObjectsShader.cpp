#include "../../../Stdafx/Stdafx.h"
#include "SkinnedAnimationObjectsShader.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../GameObject/EvilBear/EvilBear.h"
#include "../../../ResourceManager/ResourceManager.h"

CSkinnedAnimationObjectsShader::CSkinnedAnimationObjectsShader()
{
}

CSkinnedAnimationObjectsShader::~CSkinnedAnimationObjectsShader()
{
}

void CSkinnedAnimationObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
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

D3D12_SHADER_BYTECODE CSkinnedAnimationObjectsShader::CreateVertexShader(int nPipelineState)
{
	switch (nPipelineState)
	{
	case GameObject:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSSkinnedAnimationStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	case GameObject_Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSAnimationShadow", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	}
}

D3D12_SHADER_BYTECODE CSkinnedAnimationObjectsShader::CreatePixelShader(int nPipelineState)
{
	switch (nPipelineState)
	{
	case GameObject:
		return(CStandardShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	case GameObject_Shadow:
		return(CStandardShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSShadow", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	}
}

D3D12_INPUT_LAYOUT_DESC CSkinnedAnimationObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 7;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[5] = { "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[6] = { "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

void CSkinnedAnimationObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, Bounds*>& Context, void *pContext)
{
	CLoadedModelInfo* pEvilBearModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
		"../Resource/Models/EvilBear.bin", this, true, "Enemy");

	CTerrain* pTerrain = (CTerrain *)pContext;

	m_nObjects = 5;
	m_ppObjects = new CGameObject*[m_nObjects];

	m_ppObjects[0] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,CGameObject::MATERIALTYPE::PINK);
	m_ppObjects[0]->SetChild(pEvilBearModel->m_pModelRootObject, true);
	m_ppObjects[0]->m_pAnimationController = new CAnimationController(2, pEvilBearModel->m_pAnimationSets);
	// 0번 트랙에 0번 애니메이션을 Set
	m_ppObjects[0]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	// 1번 트랙에 1번 애니메이션을 Set
	m_ppObjects[0]->m_pAnimationController->SetTrackAnimationSet(1, 1);
	// 0번 트랙에 가중치를 80%
	m_ppObjects[0]->m_pAnimationController->SetTrackWeight(0, 0.8f);
	// 1번 트랙에 가중치를 20% 
	m_ppObjects[0]->m_pAnimationController->SetTrackWeight(1, 0.2f);
	m_ppObjects[0]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pEvilBearModel);

	m_ppObjects[1] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,CGameObject::MATERIALTYPE::BLACK);
	m_ppObjects[1]->SetChild(pEvilBearModel->m_pModelRootObject, true);
	m_ppObjects[1]->m_pAnimationController = new CAnimationController(1, pEvilBearModel->m_pAnimationSets);
	//m_ppObjects[1]->m_pAnimationController->SetTrackAnimationSet(1, 2);
	m_ppObjects[1]->m_pAnimationController->SetTrackAnimationSet(0, 9);
	m_ppObjects[1]->m_pAnimationController->SetTrackSpeed(0, 0.8f);
	//m_ppObjects[1]->m_pAnimationController->SetTrackWeight(0, 0.9);
	//m_ppObjects[1]->m_pAnimationController->SetTrackWeight(1, 0.1);
	// 애니메이션의 속도를 0.25로 주어서 느리게 애니메이션 동작을 하도록 Set
	m_ppObjects[1]->m_pAnimationController->SetTrackSpeed(0, 0.25f);
	m_ppObjects[1]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pEvilBearModel);

	m_ppObjects[2] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,CGameObject::MATERIALTYPE::BROWN);
	m_ppObjects[2]->SetChild(pEvilBearModel->m_pModelRootObject, true);
	m_ppObjects[2]->m_pAnimationController = new CAnimationController(1, pEvilBearModel->m_pAnimationSets);
	m_ppObjects[2]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	// 애니메이션의 시작위치를 다르게 준다. 그러면 같은 동작이더라도 다르게 애니메이션함
	m_ppObjects[2]->m_pAnimationController->SetTrackPosition(0, 0.95f);
	m_ppObjects[2]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pEvilBearModel);

	m_ppObjects[3] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,CGameObject::MATERIALTYPE::BLUE);
	m_ppObjects[3]->SetChild(pEvilBearModel->m_pModelRootObject, true);
	m_ppObjects[3]->m_pAnimationController = new CAnimationController(1, pEvilBearModel->m_pAnimationSets);
	m_ppObjects[3]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	m_ppObjects[3]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pEvilBearModel);

	m_ppObjects[4] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,CGameObject::MATERIALTYPE::ICEMAT);
	m_ppObjects[4]->SetChild(pEvilBearModel->m_pModelRootObject, true);
	m_ppObjects[4]->m_pAnimationController = new CAnimationController(1, pEvilBearModel->m_pAnimationSets);
	m_ppObjects[4]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	m_ppObjects[4]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pEvilBearModel);

	XMFLOAT3 Position;
	for (int i = 0; i < m_nObjects; ++i)
	{
		Position.x = Random(0.f, 500.f);
		Position.z = Random(0.f, 300.f);
		Position.y = pTerrain->GetHeight(Position.x, Position.z);
		m_ppObjects[i]->SetPosition(Position);
		m_ppObjects[i]->SetScale(10, 10, 10);

		m_ppObjects[i]->setID("<EvilBear>");
		auto iter = Context.find(m_ppObjects[i]->getID());
		if (iter != Context.end())
			m_ppObjects[i]->SetOOBB((*iter).second->m_xmf3Center, (*iter).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));
	}

	CEvilBear* pEvilBearShadow = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::PINK);
	pEvilBearShadow->SetPosition(m_ppObjects[0]->GetPosition().x + 10, 0.1, m_ppObjects[0]->GetPosition().z + 10);
	pEvilBearShadow->SetScale(10, 10, 10);
	pEvilBearShadow->SetChild(pEvilBearModel->m_pModelRootObject, true);
	pEvilBearShadow->m_pAnimationController = new CAnimationController(2, pEvilBearModel->m_pAnimationSets);
	// 0번 트랙에 0번 애니메이션을 Set
	pEvilBearShadow->m_pAnimationController->SetTrackAnimationSet(0, 0);
	// 1번 트랙에 1번 애니메이션을 Set
	pEvilBearShadow->m_pAnimationController->SetTrackAnimationSet(1, 1);
	// 0번 트랙에 가중치를 80%
	pEvilBearShadow->m_pAnimationController->SetTrackWeight(0, 0.8f);
	// 1번 트랙에 가중치를 20% 
	pEvilBearShadow->m_pAnimationController->SetTrackWeight(1, 0.2f);
	pEvilBearShadow->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pEvilBearModel);
	m_ShadowObjectVector.emplace_back(pEvilBearShadow);

	if (pEvilBearModel)
		delete pEvilBearModel;
}

void CSkinnedAnimationObjectsShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
{
	m_fElapsedTime = fTimeElapsed;
	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			m_ppObjects[i]->Animate(m_fElapsedTime);
		}
	}
}

void CSkinnedAnimationObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	int i = 0;
	for (auto iter = m_ShadowObjectVector.begin(); iter != m_ShadowObjectVector.end(); ++iter)
	{
		CStandardShader::Render(pd3dCommandList, pCamera, GameObject_Shadow);
		(*iter)->Animate(m_fElapsedTime);
		(*iter)->UpdateTransform(nullptr);
		(*iter)->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{	
			CStandardShader::Render(pd3dCommandList, pCamera, GameObject);

			m_ppObjects[i]->Animate(m_fElapsedTime);
			m_ppObjects[i]->UpdateTransform(NULL);
			m_ppObjects[i]->Render(pd3dCommandList,m_ppObjects[i]->GetIsHammer(),m_ppObjects[i]->GetIsBomb(),m_ppObjects[i]->GetBoolIce(),m_ppObjects[i]->GetMaterialID(),pCamera);
		}
	}
}