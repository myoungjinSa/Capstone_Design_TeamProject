//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

ID3D12DescriptorHeap *CScene::m_pd3dCbvSrvDescriptorHeap = NULL;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorStartHandle;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorNextHandle;

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 112); 

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature); 

	BuildDefaultLightsAndMaterials();

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.3f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Terrain/HeightMap.raw"), 257, 257, xmf3Scale, xmf4Color);

	m_nShaders = 4;
	m_ppShaders = new CShader*[m_nShaders];

	CSnowBillboardShader *pSnowShader = new CSnowBillboardShader();
	pSnowShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pSnowShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	m_ppShaders[0] = pSnowShader;

	CPineTreesObjectsShader *pPineTreeShader = new CPineTreesObjectsShader();
	pPineTreeShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pPineTreeShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	m_ppShaders[1] = pPineTreeShader;

	CDeerObjectsShader *pDeerShader = new CDeerObjectsShader();
	pDeerShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pDeerShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,m_pTerrain);
	m_ppShaders[2] = pDeerShader;

	CFoliageShader *pFoliageShader = new CFoliageShader();
	pFoliageShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pFoliageShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	m_ppShaders[3] = pFoliageShader;

	//CHellicopterObjectsShader *pHellicopterObjectsShader = new CHellicopterObjectsShader();
	//pHellicopterObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//pHellicopterObjectsShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//pHellicopterObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);

	//m_ppShaders[0] = pHellicopterObjectsShader;

//	CAngrybotObjectsShader *pAngrybotObjectsShader = new CAngrybotObjectsShader();
////	pAngrybotObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
////	pAngrybotObjectsShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
//	pAngrybotObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
//
//	m_ppShaders[1] = pAngrybotObjectsShader;

	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/EvilbearA.bin", NULL, true);

	m_nGameObjects =6;
	m_ppGameObjects = new CGameObject*[m_nGameObjects];

	m_ppGameObjects[0] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,CGameObject::eMaterials::WHITE);
	m_ppGameObjects[0]->SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_ppGameObjects[0]->m_pAnimationController = new CAnimationController(2, pAngrybotModel->m_pAnimationSets);				//애니메이션 트랙의 개수와 애니메이션이 저장된 행렬을 넘겨준다.
	m_ppGameObjects[0]->m_pAnimationController->SetTrackAnimationSet(0, 0);													//0번 트랙에는 0번 애니메이션
	m_ppGameObjects[0]->m_pAnimationController->SetTrackAnimationSet(1, 3);													//1번 트랙에는 1번 애니메이션
	m_ppGameObjects[0]->m_pAnimationController->SetTrackWeight(0, 0.6f);													//0번 트랙에 애니메이션은 80퍼센트
	m_ppGameObjects[0]->m_pAnimationController->SetTrackWeight(1, 0.4f);													//1번 트랙에 애니메이션은 20퍼센트
	m_ppGameObjects[0]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pAngrybotModel);		//각 오브젝트마다 스킨메쉬 트랜스폼 행렬을 생성
	m_ppGameObjects[0]->SetPosition(400.0f, m_pTerrain->GetHeight(400.0f, 700.0f), 700.0f);
	m_ppGameObjects[0]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppGameObjects[1] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,CGameObject::eMaterials::BLACK);
	m_ppGameObjects[1]->SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_ppGameObjects[1]->m_pAnimationController = new CAnimationController(1, pAngrybotModel->m_pAnimationSets);				//1개의 애니메이션 트랙, 1개의 애니메이션 배열 
	m_ppGameObjects[1]->m_pAnimationController->SetTrackAnimationSet(0, 7);													//0번 트랙을 1번 애니메이션으로 set
	m_ppGameObjects[1]->m_pAnimationController->SetTrackSpeed(0, 1.0f);													//0번 트랙의 스피드를 0.25배
	m_ppGameObjects[1]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pAngrybotModel);
	m_ppGameObjects[1]->SetPosition(450.0f, m_pTerrain->GetHeight(450.0f, 680.0f), 680.0f);
	m_ppGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppGameObjects[2] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,CGameObject::eMaterials::BLUE);
	m_ppGameObjects[2]->SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_ppGameObjects[2]->m_pAnimationController = new CAnimationController(1, pAngrybotModel->m_pAnimationSets);
	m_ppGameObjects[2]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	m_ppGameObjects[2]->m_pAnimationController->SetTrackPosition(0, 0.95f);
	m_ppGameObjects[2]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pAngrybotModel);
	m_ppGameObjects[2]->SetPosition(420.0f, m_pTerrain->GetHeight(420.0f, 750.0f), 750.0f);
	m_ppGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppGameObjects[3] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,CGameObject::eMaterials::PANDA);
	m_ppGameObjects[3]->SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_ppGameObjects[3]->m_pAnimationController = new CAnimationController(1, pAngrybotModel->m_pAnimationSets);
	m_ppGameObjects[3]->m_pAnimationController->SetTrackAnimationSet(0, 0);
	m_ppGameObjects[3]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pAngrybotModel);
	m_ppGameObjects[3]->SetPosition(410.0f, m_pTerrain->GetHeight(410.0f, 735.0f), 735.0f);
	m_ppGameObjects[3]->SetScale(20.0f, 20.0f, 20.0f);


	m_ppGameObjects[4] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, CGameObject::eMaterials::BROWN);
	m_ppGameObjects[4]->SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_ppGameObjects[4]->m_pAnimationController = new CAnimationController(2, pAngrybotModel->m_pAnimationSets);
	m_ppGameObjects[4]->m_pAnimationController->SetTrackAnimationSet(0, CTerrainPlayer::eState::RUNFAST);
	m_ppGameObjects[4]->m_pAnimationController->SetTrackAnimationSet(1, CTerrainPlayer::eState::WALKFRONT);
	m_ppGameObjects[4]->m_pAnimationController->SetTrackWeight(0, 0.5f);
	m_ppGameObjects[4]->m_pAnimationController->SetTrackWeight(1, 0.5f);
	m_ppGameObjects[4]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pAngrybotModel);
	m_ppGameObjects[4]->SetPosition(470.0f, m_pTerrain->GetHeight(470.0f, 700.0f), 700.0f);
	m_ppGameObjects[4]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppGameObjects[5] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, CGameObject::eMaterials::ICE);
	m_ppGameObjects[5]->SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_ppGameObjects[5]->m_pAnimationController = new CAnimationController(1, pAngrybotModel->m_pAnimationSets);
	m_ppGameObjects[5]->m_pAnimationController->SetTrackAnimationSet(0, CTerrainPlayer::eState::RUNFAST);
	m_ppGameObjects[5]->m_pAnimationController->SetTrackWeight(0, 0.5f);
	m_ppGameObjects[5]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pAngrybotModel);
	m_ppGameObjects[5]->SetPosition(500.0f, m_pTerrain->GetHeight(500.0f, 740.0f), 740.0f);
	m_ppGameObjects[5]->SetScale(20.0f, 20.0f, 20.0f);

	
	

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (pAngrybotModel)
	{
		delete pAngrybotModel;
	}
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++)
		{
			if (m_ppGameObjects[i])
			{
				m_ppGameObjects[i]->Release();
			}
		}
		delete[] m_ppGameObjects;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[13];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtSpecularTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtNormalTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtMetallicTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 10; //t10: gtxtEmissionTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 11; //t11: gtxtEmissionTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 12; //t12: gtxtEmissionTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 1;
	pd3dDescriptorRanges[8].BaseShaderRegister = 1; //t1: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 2; //t2: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[10].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[10].NumDescriptors = 1;
	pd3dDescriptorRanges[10].BaseShaderRegister = 14; //t14: gtxtParticle
	pd3dDescriptorRanges[10].RegisterSpace = 0;
	pd3dDescriptorRanges[10].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	
	pd3dDescriptorRanges[11].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[11].NumDescriptors = 1;
	pd3dDescriptorRanges[11].BaseShaderRegister = 15; //t15: gtxtSnowFlake
	pd3dDescriptorRanges[11].RegisterSpace = 0;
	pd3dDescriptorRanges[11].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	pd3dDescriptorRanges[12].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[12].NumDescriptors = 1;
	pd3dDescriptorRanges[12].BaseShaderRegister = 16; //t16: gtxtGrassBillboardTexture
	pd3dDescriptorRanges[12].RegisterSpace = 0;
	pd3dDescriptorRanges[12].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;





	D3D12_ROOT_PARAMETER pd3dRootParameters[19];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 33;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[8]);
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[9]);
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[15].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[15].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[15].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[10]);
	pd3dRootParameters[15].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[16].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[16].Descriptor.ShaderRegister = 5; //Particle
	pd3dRootParameters[16].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[16].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[17].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[17].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[17].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[11]);
	pd3dRootParameters[17].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[18].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[18].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[18].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[12]);
	pd3dRootParameters[18].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;




	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObjects[i]) 
		{
			m_ppGameObjects[i]->ReleaseUploadBuffers();
		}
	}
}

void CScene::CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
		m_d3dCbvGPUDescriptorNextHandle.ptr = m_d3dCbvGPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
	}
	return(d3dCbvGPUDescriptorHandle);
}

D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
		case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
		case RESOURCE_TEXTURE2D_ARRAY:
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
			d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
			d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			break;
		case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
			d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
			d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
			d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
			d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
			break;
		case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
			d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
			d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			break;
		case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
			d3dShaderResourceViewDesc.Buffer.NumElements = 0;
			d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
			d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			break;
	}
	return(d3dShaderResourceViewDesc);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateShaderResourceViews(ID3D12Device *pd3dDevice, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorNextHandle;
	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		int nTextureType = pTexture->GetTextureType();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource *pShaderResource = pTexture->GetTexture(i);
			D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

			pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
	return(d3dSrvGPUDescriptorHandle);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W': m_ppGameObjects[0]->MoveForward(+3.0f); break;
		case 'S': m_ppGameObjects[0]->MoveForward(-3.0f); break;
		case 'A': m_ppGameObjects[0]->MoveStrafe(-3.0f); break;
		case 'D': m_ppGameObjects[0]->MoveStrafe(+3.0f); break;
		case 'Q': m_ppGameObjects[0]->MoveUp(+3.0f); break;
		case 'R': m_ppGameObjects[0]->MoveUp(-3.0f); break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (int i = 0; i < m_nShaders; i++)
	{
		if (m_ppShaders[i])
		{
			m_ppShaders[i]->AnimateObjects(fTimeElapsed,m_pPlayer->GetCamera(),m_pPlayer);
		}
	}

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}


void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObjects[i])
		{
			m_ppGameObjects[i]->UpdateTransform(NULL);				//animateObject와 UpdateTransform의 순서가 바뀌면 애니메이션을 하는 캐릭터가 바뀐다.
			m_ppGameObjects[i]->Animate(m_fElapsedTime, m_pPlayer->GetCamera());
			m_ppGameObjects[i]->Render(pd3dCommandList,false,m_ppGameObjects[i]->GetMatID(),pCamera);
		}
	}

	for (int i = 0; i < m_nShaders; i++)
	{
		if (m_ppShaders[i])
		{
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);
		}
	}
}

