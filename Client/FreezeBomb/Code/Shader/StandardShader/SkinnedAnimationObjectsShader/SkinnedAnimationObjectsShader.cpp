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

D3D12_SHADER_BYTECODE CSkinnedAnimationObjectsShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSSkinnedAnimationStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

void CSkinnedAnimationObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, Bounds*>& Context, void *pContext)
{
	CLoadedModelInfo* pEvilBearModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
		"../Resource/Models/EvilBear.bin", NULL, true);

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
	m_ppObjects[1]->m_pAnimationController->SetTrackAnimationSet(0, 1);
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
		m_ppObjects[i]->SetPosition(Position.x, pTerrain->GetHeight(Position.x, Position.z), Position.z);
		m_ppObjects[i]->SetScale(10, 10, 10);

		m_ppObjects[i]->setID("<EvilBear>");
		auto iter = Context.find(m_ppObjects[i]->getID());
		if (iter != Context.end())
			m_ppObjects[i]->SetOOBB((*iter).second->m_xmf3Center, (*iter).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

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
	CShader::Render(pd3dCommandList,pCamera);

	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
	
			m_ppObjects[i]->Animate(m_fElapsedTime);
			m_ppObjects[i]->UpdateTransform(NULL);
			m_ppObjects[i]->Render(pd3dCommandList,m_ppObjects[i]->GetBoolIce(),m_ppObjects[i]->GetMaterialID(),pCamera);
		}
	}
}