#include "../Stdafx/Stdafx.h"
#include "Scene.h"
#include "../Texture/Texture.h"
#include "../Material/Material.h"
#include "../ShaderManager/ShaderManager.h"
#include "../Shader/Shader.h"
#include "../GameObject/Player/Player.h"
#include "../GameObject/Item/Item.h"
#include "../Shader/StandardShader/MapObjectShader/MapObjectShader.h"
#include "../Shader/StandardShader/ItemShader/ItemShader.h"
#include "../SoundSystem/SoundSystem.h"
#include "../Shader/BillboardShader/UIShader/TimerUIShader/TimerUIShader.h"
#include "../Shader/CubeParticleShader/CubeParticleShader.h"

#include "../GameObject/Surrounding/Surrounding.h"
#include "../Shader/BillboardShader/UIShader/MenuUIShader/MenuUIShader.h"
#include "../Shader/StandardShader/SkinnedAnimationShader/SkinnedAnimationObjectShader/SkinnedAnimationObjectShader.h"
#include "../Network/Network.h"
#include "../GameObject/Item/Item.h"
#include "../GameFramework/GameFramework.h"

ID3D12DescriptorHeap* CScene::m_pd3dCbvSrvDescriptorHeap = NULL;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorStartHandle;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorNextHandle;

extern unsigned char g_Round;

#ifndef _WITH_SERVER_
float CScene::m_TaggerCoolTime = 0.f;
#endif

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildFog()
{
	m_pFog = new FOG;
	::ZeroMemory(m_pFog, sizeof(FOG));

	m_pFog->FogColor = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_pFog->FogFactor.gFogMode = 1.0f;		//1 = 선형 안개 ,2 = 비 선형 안개( 1/e^(d*density)),3 = 비선형 안개(1/e^((d*density)^2))
	m_pFog->FogFactor.gFogStart = 30.0f;
	m_pFog->FogFactor.gFogRange = 100.0f;
	m_pFog->FogFactor.gFogDensity = 0.5f;

}
void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_pLights[0].m_bEnable = false;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.9f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = false;
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
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f,1.0f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.5f, 0.0f);
	m_pLights[3].m_bEnable = false;
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


void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const int& nPlayerCount)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	int nObjects = 0;

	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 
		SkyBox + Terrain + MapObjects + Item + EvilBear + BombParticle + CubeParticle + Snow + TimerUI + ItemUI + Player
	+ MenuUI + Thunder + FirePit);


	// Model을 로드할 때, 셰이더 없이 로드할 경우 이것을 사용함!
	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();
	BuildFog();

	m_pShaderManager = new CShaderManager;
	m_pShaderManager->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, nPlayerCount);

	//사운드 생성
	//CreateSoundSystem();
	
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	CMaterial::ReleaseShaders();
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	if (m_pd3dCbvSrvDescriptorHeap)
		m_pd3dCbvSrvDescriptorHeap->Release();

	if (m_pShaderManager)
		m_pShaderManager->ReleaseObjects();

	ReleaseShaderVariables();

	if (m_pLights)
		delete[] m_pLights;

	if (m_pFog)
		delete[] m_pFog;
}


ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[16];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 3; // t3: gtxtSkyBoxTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 4; // t4: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 5; // t5: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 6; // t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 7; // t7: gtxtSpecularTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 8; // t8: gtxtNormalTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 9; // t9: gtxtMetallicTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 10; // t10: gtxtEmissionTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 1;
	pd3dDescriptorRanges[8].BaseShaderRegister = 11; // t11: gtxtDetailAlbedoTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 12; // t12: gtxtDetailNormalTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[10].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[10].NumDescriptors = 1;
	pd3dDescriptorRanges[10].BaseShaderRegister = 13; // t13: lampTexture
	pd3dDescriptorRanges[10].RegisterSpace = 0;
	pd3dDescriptorRanges[10].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[11].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[11].NumDescriptors = 1;
	pd3dDescriptorRanges[11].BaseShaderRegister = 14; // t14: IceTexture
	pd3dDescriptorRanges[11].RegisterSpace = 0;
	pd3dDescriptorRanges[11].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[12].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[12].NumDescriptors = 1;
	pd3dDescriptorRanges[12].BaseShaderRegister = 15; // t15: SnowTexture
	pd3dDescriptorRanges[12].RegisterSpace = 0;
	pd3dDescriptorRanges[12].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[13].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[13].NumDescriptors = 1;
	pd3dDescriptorRanges[13].BaseShaderRegister = 16; // t16: UI
	pd3dDescriptorRanges[13].RegisterSpace = 0;
	pd3dDescriptorRanges[13].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[14].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[14].NumDescriptors = 1;
	pd3dDescriptorRanges[14].BaseShaderRegister = 17; // t17: ThunderTexture
	pd3dDescriptorRanges[14].RegisterSpace = 0;
	pd3dDescriptorRanges[14].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[15].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[15].NumDescriptors = 1;
	pd3dDescriptorRanges[15].BaseShaderRegister = 18; // t18: FireTexture
	pd3dDescriptorRanges[15].RegisterSpace = 0;
	pd3dDescriptorRanges[15].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;




	D3D12_ROOT_PARAMETER pd3dRootParameters[27];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1;	// b1 : Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 2;	// b2 : 게임오브젝트 월드좌표
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 3;	// b3 : Material
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 4;	// b4 : Lights
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 5; // b5 : BoneOffsets
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 6; // b6 : BoneTransforms
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[6].Descriptor.ShaderRegister = 7;	// b7 : AnimationClip
	pd3dRootParameters[6].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[7].Descriptor.ShaderRegister = 1; // t1 : CubeParticle 인스턴싱
	pd3dRootParameters[7].Descriptor.RegisterSpace = 0;	
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[8].Descriptor.ShaderRegister = 2; // t2 : Snow 인스턴싱
	pd3dRootParameters[8].Descriptor.RegisterSpace = 0;	
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);	
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]); 
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[11].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[11].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]); 
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[12].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[12].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[15].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[15].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[15].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[15].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[16].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[16].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[16].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[16].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[17].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[17].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[17].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[8]);
	pd3dRootParameters[17].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[18].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[18].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[18].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[9]);
	pd3dRootParameters[18].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[19].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[19].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[19].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[10]);
	pd3dRootParameters[19].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[20].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[20].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[20].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[11]);
	pd3dRootParameters[20].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[21].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[21].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[21].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[12]);
	pd3dRootParameters[21].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[22].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[22].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[22].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[13]);
	pd3dRootParameters[22].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[23].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[23].Descriptor.ShaderRegister = 8;	// b8 : Fog
	pd3dRootParameters[23].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[23].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[24].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[24].Descriptor.ShaderRegister = 9;	// b9 : UI
	pd3dRootParameters[24].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[24].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[25].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[25].DescriptorTable.NumDescriptorRanges = 1;		//t17: ThunderTexture
	pd3dRootParameters[25].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[14]);
	pd3dRootParameters[25].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[26].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[26].DescriptorTable.NumDescriptorRanges = 1;		//t18: FireTexture
	pd3dRootParameters[26].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[15]);
	pd3dRootParameters[26].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

	//안개
	UINT ncbFogElementBytes = ((sizeof(FOG) + 255)&~255);
	m_pd3dcbFog = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbFogElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbFog->Map(0, NULL, (void**)&m_pcbMappedFog);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));

	D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, GpuVirtualAddress);

	//안개
	::memcpy(m_pcbMappedFog, m_pFog, sizeof(FOG));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbFogGPUVirtualAddress = m_pd3dcbFog->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(23, d3dcbFogGPUVirtualAddress);
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

	if (m_pd3dcbFog)
	{
		m_pd3dcbFog->Unmap(0, NULL);
		m_pd3dcbFog->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pShaderManager)
		m_pShaderManager->ReleaseUploadBuffers();
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

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	int mouseX = LOWORD(lParam);
	int mouseY = HIWORD(lParam);
	
	switch (nMessageID)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
		if (m_pShaderManager && CTimerUIShader::getTimer() > 0.f )
		{
			auto iter = m_pShaderManager->getShaderMap().find("MenuUI");
			if (iter != m_pShaderManager->getShaderMap().end())
			{
				CMenuUIShader* menu = reinterpret_cast<CMenuUIShader*>((*iter).second);
				menu->ProcessMessage(CMenuUIShader::MOUSE, nMessageID, ScreenPosition(mouseX, mouseY));
			}
		}
		break;

	case WM_RBUTTONDOWN:
		//cout << "마우스 오른쪽 클릭 - x : " << mouseX << ", y : " << mouseY << endl;
		break;
	case WM_LBUTTONUP:
		//cout << "마우스 왼쪽 때짐" << endl;
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYUP:
			switch (wParam)
			{	
				case VK_ESCAPE:
				{
					if (m_pShaderManager && CTimerUIShader::getTimer() > 0.f)
					{
						auto iter = m_pShaderManager->getShaderMap().find("MenuUI");
						if (iter != m_pShaderManager->getShaderMap().end())
						{
							CMenuUIShader* menu = reinterpret_cast<CMenuUIShader*>((*iter).second);

							menu->ProcessMessage(CMenuUIShader::KEYBOARD, nMessageID, XMFLOAT2(0, 0));
						}
					}					
				}
				break;
			}
		break;
	}
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return false;
}

void CScene::AnimateObjects(ID3D12GraphicsCommandList *pd3dCommandList,float fTimeElapsed)
{
	if (m_pShaderManager) 
		m_pShaderManager->AnimateObjects(fTimeElapsed, m_pPlayer->GetCamera(), m_pPlayer);

	if (m_pLights)
	{
		if (g_Round == CGameFramework::STAGE::ROUND_3) 
		{
			//점 조명
			m_pLights[0].m_bEnable = true;
			m_pLights[0].m_xmf3Position = XMFLOAT3(250.0f,10.0f,150.0f);
		}
		else
		{
			m_pLights[0].m_bEnable = false;
		}
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
		m_pLights[2].m_xmf3Position = XMFLOAT3(20.0f, 30.0f, 0.0f);
		m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -0.5f, -0.5f);
	}
}

void CScene::PreRender(ID3D12GraphicsCommandList *pd3dCommandList, float fTimeElapsed, CCamera *pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList, fTimeElapsed);

	//Lights
	UpdateShaderVariables(pd3dCommandList);

	if (m_pShaderManager)
	{
		m_pShaderManager->PreRender(pd3dCommandList,fTimeElapsed, pCamera);
	}

}

void CScene::PostRender(ID3D12GraphicsCommandList *pd3dCommandList,float fTimeElapsed, CCamera *pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	if (m_pShaderManager)
	{
		m_pShaderManager->PostRender(pd3dCommandList,fTimeElapsed, pCamera);
	}

#ifndef _MAPTOOL_MODE_
	CheckWarningTimer();
#endif
}

void CScene::CheckObjectByObjectCollisions(float elapsedTime)
{
	//서버 자체에서 쿨타임을 계산해야한다. 
	//쿨타임 변경 해킹이 있을수 있기 때문
#ifndef _WITH_SERVER_
	if (m_TaggerCoolTime > 0.f)
	{
		m_TaggerCoolTime -= elapsedTime;
		//cout << m_TaggerCoolTime << endl;
	}
	else
		m_TaggerCoolTime = 0.f;

#endif
	if (m_pPlayer)
	{		
		// 플레이어와 충돌 된 오브젝트 정보를 초기화
		//m_pPlayer->SetObjectCollided(nullptr);

		// 플레이어와 정적인 오브젝트 충돌검사
		map<string, CShader*> m = m_pShaderManager->getShaderMap();
		bool isCollided{ false };
		auto iter = m.find("MapObjects");
		if (iter != m.end())
		{
			int i = 0;
			auto MapObjectsList = ((CMapObjectsShader*)((*iter).second))->getMapObjectList();

			// 맵 오브젝트의 충돌 된 오브젝트를 초기화해줌
			for (auto iter2 = MapObjectsList.begin(); iter2 != MapObjectsList.end(); ++iter2)
				(*iter2)->SetObjectCollided(nullptr);

			for (auto iter2 = MapObjectsList.begin(); iter2 != MapObjectsList.end(); ++iter2)
			{		
				if((*iter2)->GetBoundingBox().Intersects(m_pPlayer->GetBoundingBox()))
				{

#ifdef _WITH_SERVER_
					isCollided = true;
					
					Network::GetInstance()->SendSurroundingCollision(dynamic_cast<CSurrounding*>(*iter2)->GetIndex());
					//cout << "index: " << dynamic_cast<CSurrounding*>((*iter2))->GetIndex()<<"\n";
#else
					XMFLOAT3 xmf3CollisionDir = Vector3::SubtractNormalize((*iter2)->GetPosition() ,m_pPlayer->GetPosition());
					xmf3CollisionDir=Vector3::ScalarProduct(xmf3CollisionDir, m_pPlayer->GetMaxVelocity()*0.3f);
					m_pPlayer->SetVelocity(-xmf3CollisionDir.x,-xmf3CollisionDir.y,-xmf3CollisionDir.z);
#endif
					m_pPlayer->SetObjectCollided((*iter2));
					
					//cout << i << "번째 정적인 오브젝트와 충돌" << endl;
				}
				++i;
			}

		}

		iter = m.find("OtherPlayer");
		if (iter != m.end())
		{
			float minDistance = 1000.0f;
			
			char id{ -1 };
#ifdef _WITH_SERVER_
			vector<pair<char, char>> vec = dynamic_cast<CSkinnedAnimationObjectShader*>((*iter).second)->m_vMaterial;
			for (int i = 0; i < vec.size(); ++i)
			{
				id = vec[i].first;
				
				if ((*iter).second->m_ppObjects[id]->GetBoundingBox().Intersects(m_pPlayer->GetBoundingBox()))
				{
					// 술래 체인지
					if (m_pPlayer->GetIsBomb() == true /*&& m_TaggerCoolTime <= 0.f*/)
					{
						//(*iter).second->m_ppObjects[id]->SetIsBomb(true);
						//m_pPlayer->SetIsBomb(false);
						Network::GetInstance()->SendBomberTouch(id);

						//m_TaggerCoolTime = (float)COOLTIME;
					}
					//else if ((*iter).second->m_ppObjects[id]->GetIsBomb() == true /*&& m_TaggerCoolTime <= 0.f*/)
				//{
						//m_pPlayer->SetIsBomb(true);
					//	(*iter).second->m_ppObjects[id]->SetIsBomb(false);

						//m_TaggerCoolTime = (float)COOLTIME;
					//}

					isCollided = true;
				
					Network::GetInstance()->SendPlayerCollision(id);


				}
				
				float dist = Vector3::Length(Vector3::SubtractNormalize((*iter).second->m_ppObjects[id]->GetPosition(), m_pPlayer->GetPosition()));
				(*iter).second->m_ppObjects[id]->SetDistanceToTarget(dist);

			}
			if(isCollided == false)
			{
				Network::GetInstance()->SendNotCollision();
			}
			static bool bBreak = false;

			if (m_pPlayer->AnimationCollision(CAnimationController::ATTACK))
			{
				CGameObject* pHammer = m_pPlayer->FindFrame("Hammer");
				if (pHammer != nullptr)
				{
					for (int i = 0; i < vec.size(); ++i)
					{
						char id = vec[i].first;
						if (pHammer->GetBoundingBox().Intersects((*iter).second->m_ppObjects[id]->GetBoundingBox()))
						{
							Network::GetInstance()->SendUseItem(ITEM::NORMALHAMMER, id);

							// 이 아래 부분 processPacket으로 옮겨야할듯
							if (m_pPlayer->get_Normal_InventorySize() > 0)
							{
								if (bBreak == false)
									bBreak = true;

								m_pPlayer->Sub_Inventory(CItem::NormalHammer);
							}
							m_pShaderManager->ProcessCollision((*iter).second->m_ppObjects[id]->GetPosition());

							CSoundSystem::PlayingSound(CSoundSystem::ICE_BREAK);

							//cout << i << "번째 애니메이션 오브젝트와 플레이어 망치 충돌" << endl;
							break;
						}
					}
				}
			}
			if(m_pPlayer->IsCameraVibe())
			{
				m_bVibeTime += elapsedTime;

				if(m_bVibeTime >1.0f)
				{
					m_pPlayer->SetCameraVibe(false);
					m_bVibeTime = 0.0f;
					if (bBreak)
						bBreak = false;
				}
			}
#else
			for (int i = 0; i < (*iter).second->m_nObjects; ++i)
			{
				if ((*iter).second->m_ppObjects[i]->GetBoundingBox().Intersects(m_pPlayer->GetBoundingBox()))
				{
					// 술래 체인지
					if ((*iter).second->m_ppObjects[i]->GetIsICE() == false)
					{
						if (m_pPlayer->GetIsBomb() == true && m_TaggerCoolTime <= 0.f)
						{
							(*iter).second->m_ppObjects[i]->SetIsBomb(true);

							m_pPlayer->SetIsBomb(false);

							m_TaggerCoolTime = 3.f;
						}
						else if ((*iter).second->m_ppObjects[i]->GetIsBomb() == true && m_TaggerCoolTime <= 0.f)
						{
							if (m_pPlayer->GetIsICE() == false)
							{
								m_pPlayer->SetIsBomb(true);
								(*iter).second->m_ppObjects[i]->SetIsBomb(false);

								m_TaggerCoolTime = 3.f;
							}

						}
					}

					//(*iter).second->m_ppObjects[i]->SetObjectCollided(m_pPlayer);
					//m_pPlayer->SetObjectCollided((*iter).second->m_ppObjects[i]);
					//이쪽에 일단 클라이언트단에서 못움직이게 구현. 추후에 서버에서 해야함
					//플레이어와 오브젝트의 원점에서의 거리를 구한다음 방향을 구하여 그 방향으로는 진행을 못하게 해야할듯

					XMFLOAT3 xmf3CollisionDir = Vector3::SubtractNormalize((*iter).second->m_ppObjects[i]->GetPosition(), m_pPlayer->GetPosition());
					xmf3CollisionDir = Vector3::ScalarProduct(xmf3CollisionDir, (m_pPlayer->GetMaxVelocity()*0.3f));
					m_pPlayer->SetVelocity(-xmf3CollisionDir.x, -xmf3CollisionDir.y, -xmf3CollisionDir.z);

					//cout << i << "번째 애니메이션 오브젝트와 충돌" << endl;
				}

				//각 캐릭터는 플레이어와의 거리 변수를 저장한다.
				//사운드 볼륩 조절에 필요.
				float dist = Vector3::Length(Vector3::SubtractNormalize((*iter).second->m_ppObjects[i]->GetPosition(), m_pPlayer->GetPosition()));
				(*iter).second->m_ppObjects[i]->SetDistanceToTarget(dist);
				//m_pPlayer->SetMinDistanceWithEnemy(minDistance);
			}

			//cout<< "최소 거리:" << minDistance << "\n";


			static bool bBreak = false;

			if (m_pPlayer->AnimationCollision(CAnimationController::ATTACK))
			{
				CGameObject* pHammer = m_pPlayer->FindFrame("Hammer");
				if (pHammer != nullptr)
				{
					for (int i = 0; i < (*iter).second->m_nObjects; ++i)
					{
						if ((*iter).second->m_ppObjects[i]->GetIsICE() == true)
						{
							if (pHammer->GetBoundingBox().Intersects((*iter).second->m_ppObjects[i]->GetBoundingBox()))
							{
								if (m_pPlayer->get_Normal_InventorySize() > 0)
								{
									if (bBreak == false)
										bBreak = true;

									m_pPlayer->Sub_Inventory(CItem::NormalHammer);
								}
								m_pShaderManager->ProcessCollision(Vector3::Add((*iter).second->m_ppObjects[i]->GetPosition(),XMFLOAT3(0.0f,2.0f,0.0f)));
								CSoundSystem::PlayingSound(CSoundSystem::ICE_BREAK);
								(*iter).second->m_ppObjects[i]->SetIsICE(false);
								//cout << i << "번째 애니메이션 오브젝트와 플레이어 망치 충돌" << endl;
								break;
							}
						}
						//if (pHammer->GetBoundingBox().Intersects((*iter).second->m_ppObjects[i]->GetBoundingBox()))
						//{
						//	if (m_pPlayer->get_Normal_InventorySize() > 0)
						//	{
						//		if (bBreak == false)
						//			bBreak = true;

						//		m_pPlayer->Refresh_Inventory(CItem::NormalHammer);
						//	}
						//	m_pShaderManager->ProcessCollision((*iter).second->m_ppObjects[i]->GetPosition());

						//	PlayIceBreakEffect(bBreak);
						//	(*iter).second->m_ppObjects[i]->SetIsICE(false);
						//	//cout << i << "번째 애니메이션 오브젝트와 플레이어 망치 충돌" << endl;
						//	break;
						//}
					}
				}
			}

			if(m_pPlayer->IsCameraVibe())
			{
				m_bVibeTime += elapsedTime;

				if(m_bVibeTime >1.0f)
				{
					m_pPlayer->SetCameraVibe(false);
					m_bVibeTime = 0.0f;
					if (bBreak)
						bBreak = false;
				}
			}
#endif
		}

		// 플레이어와 아이템 오브젝트 충돌검사
		iter = m.find("Item");
		if (iter != m.end())
		{
			CItemShader* pItemShader = dynamic_cast<CItemShader*>((*iter).second);
			for (auto iter2 = pItemShader->getItemList().begin(); iter2 != pItemShader->getItemList().end(); ++iter2)
			{
				
				if ((*iter2).second->GetBoundingBox().Intersects(m_pPlayer->GetBoundingBox()))
				{
					cout <<"플레이어:"<< m_pPlayer->GetBoundingBox().Extents.x << "," << m_pPlayer->GetBoundingBox().Extents.y << "," << m_pPlayer->GetBoundingBox().Extents.z << "\n";
					cout << "아이템:" << (*iter2).second->GetBoundingBox().Extents.x << "," << (*iter2).second->GetBoundingBox().Extents.y << "," << (*iter2).second->GetBoundingBox().Extents.z << "\n";

					//(*iter2).second->SetObjectCollided(m_pPlayer);
					//m_pPlayer->SetObjectCollided((*iter2).second);
					//cout <<"플레이어:"<< m_pPlayer->GetBoundingBox().Extents.x << "," << m_pPlayer->GetBoundingBox().Extents.y << "," << m_pPlayer->GetBoundingBox().Extents.z << "\n";
					//cout << "아이템:" << (*iter2).second->GetBoundingBox().Extents.x << "," << (*iter2).second->GetBoundingBox().Extents.y << "," << (*iter2).second->GetBoundingBox().Extents.z << "\n";
					int itemType = CItem::ItemType::Empty;
					MappingItemStringToItemType((*iter2).first,itemType);
					if (CheckPlayerInventory(itemType)) 
					{
						CSoundSystem::PlayingSound(CSoundSystem::GET_ITEM);
#ifdef _WITH_SERVER_
					//if(CheckPlayerInventory((*iter2).second))
					
						Network::GetInstance()->SendGetItem((*iter2).first);

						// 클라이언트 단에서 지우는 이유는
						// 여러번에 충돌처리로 인하여 여러번 패킷이 보내지지 않도록 함과  동시에
						// 인벤토리에 아이템 등록 처리가 중복되게 하지 않게 하기 위해서다.
						pItemShader->ItemDelete((*iter2).first);
					
#else
						m_pPlayer->Add_Inventory((*iter2).first, (*iter2).second->getItemType());
						// 맵에 있는 아이템 삭제
						pItemShader->ItemDelete((*iter2).first);
				
						// 충돌 된 아이템을 플레이어 인벤토리에 추가한다.
					
#endif
					}
					break;
				}
			}
		}
	}
}

void CScene::CheckWarningTimer()
{
	float timer = CTimerUIShader::getTimer();

	if (0 < timer && timer <= 10.f)
		CSoundSystem::PlayingSound(CSoundSystem::TIMER_WARNING);
	else
		CSoundSystem::StopSound(CSoundSystem::TIMER_WARNING);	
}

bool CScene::DistanceToTarget(XMFLOAT3& pos)
{
	bool ret = false;

	XMFLOAT3 distance = Vector3::Subtract(m_pPlayer->GetPosition(), pos);
	float dis = Vector3::Length(distance);

	if (dis < 100.0f)
		ret = true;

	return ret;
}

XMFLOAT2 CScene::ProcessNameCard(const int& objNum)
{
	map<string, CShader*> m = getShaderManager()->getShaderMap();
	XMFLOAT4X4 viewProj = Matrix4x4::Multiply(m_pPlayer->GetCamera()->GetViewMatrix(), m_pPlayer->GetCamera()->GetProjectionMatrix());

	XMFLOAT2 res{NAN,NAN};
	auto iter = m.find("OtherPlayer");
	if(iter != m.end())
	{
		CGameObject *obj = (*iter).second->m_ppObjects[objNum];
		XMFLOAT3 s = XMFLOAT3(obj->m_xmf4x4ToParent._41, obj->m_xmf4x4ToParent._42, obj->m_xmf4x4ToParent._43);
		XMFLOAT3 pos = Vector3::Add(s, XMFLOAT3(0.0f, 8.0f, 0.0f));
		
		if (DistanceToTarget(pos) == true && m_pPlayer->GetCamera()->IsInFrustum(obj->GetBoundingBox()))
		{
			float viewX = pos.x * viewProj._11 + pos.y * viewProj._21 + pos.z * viewProj._31 + viewProj._41;
			float viewY = pos.x * viewProj._12 + pos.y * viewProj._22 + pos.z * viewProj._32 + viewProj._42;
			float viewZ = pos.x * viewProj._13 + pos.y * viewProj._23 + pos.z * viewProj._33 + viewProj._43;

			XMFLOAT2 screen;
			screen.x = (float)(viewX / viewZ + 1.0f) * (m_pPlayer->GetCamera()->GetViewport().Width * 0.5f);
			screen.y = (float)(-viewY / viewZ + 1.0f) * (m_pPlayer->GetCamera()->GetViewport().Height * 0.5f);
			res = screen;
		}
	}
	return res;
}

XMFLOAT2 CScene::ScreenPosition(int x, int y)
{
	CCamera* pCamera = m_pPlayer->GetCamera();
	if (pCamera != nullptr)
	{
		D3D12_VIEWPORT Viewport = pCamera->GetViewport();

		XMFLOAT2 screenPosition = XMFLOAT2(0.f, 0.f);
		screenPosition.x = (((2.0f * x) / Viewport.Width) - 1) / 1;
		screenPosition.y = - (((2.0f * y) / Viewport.Height) - 1) / 1;

		return screenPosition;
	}

	return XMFLOAT2(0.f, 0.f);
}

void CScene::ChangeRound()
{
	map<string, CShader*> m = getShaderManager()->getShaderMap();
	auto iter = m.find("OtherPlayer");
	if (iter != m.end())
	{

	}

	m_pPlayer->ChangeRound();
}

void CScene::MappingItemStringToItemType(const string& strItem,int& itemType)
{
	if(strstr(strItem.c_str(), "GoldTimer"))
	{
		itemType = CItem::ItemType::GoldTimer;
	}
	else if (strstr(strItem.c_str(), "GoldHammer"))
	{
		itemType = CItem::ItemType::GoldHammer;
	}
	else if(strstr(strItem.c_str(),"NormalHammer"))
	{
		itemType = CItem::ItemType::NormalHammer;
	}
	else
	{
		cout << "비 정상 아이템\n";
		itemType = CItem::ItemType::Empty;
	}
}

bool CScene::CheckPlayerInventory(const int& itemType)
{
	bool ret =false;
	if(m_pPlayer)
	{
		switch(itemType)
		{
		case CItem::ItemType::NormalHammer:
			ret = m_pPlayer->CheckInventoryToGet(itemType);
				break;
		case CItem::ItemType::GoldHammer:
			ret = m_pPlayer->CheckInventoryToGet(itemType);
				break;
		case CItem::ItemType::GoldTimer:
			ret = m_pPlayer->CheckInventoryToGet(itemType);
				break;
		}
	}

	return ret;
}