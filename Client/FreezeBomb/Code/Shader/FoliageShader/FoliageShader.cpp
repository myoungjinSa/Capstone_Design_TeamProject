#include "../../Stdafx/Stdafx.h"
#include "../../GameObject/Foliage/Foliage.h"
#include "../../GameObject/Terrain/Terrain.h"
#include "FoliageShader.h"

CFoliageShader::CFoliageShader()
{

}

CFoliageShader::~CFoliageShader()
{

	if (m_ppFoliageModel01)
		delete m_ppFoliageModel01;
	if (m_ppFoliageModel02)
		delete m_ppFoliageModel02;
	if (m_ppFoliageModel03)
		delete m_ppFoliageModel03;
}

D3D12_SHADER_BYTECODE CFoliageShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSGrassStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
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

void CFoliageShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{



	m_ppFoliageModel01 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Grass_C_01.bin", this, false);
	m_ppFoliageModel02 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Grass_D_01.bin", this, false);
	m_ppFoliageModel03 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Plant_c_01.bin", this, false);


	CTerrain *pTerrain = (CTerrain *)pContext;


	float fxPitch = 3.0f;
	float fzPitch = 3.0f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = int(fTerrainWidth/* 0.05f*/ / fxPitch);
	int zObjects = int(fTerrainLength * 0.25f / fzPitch);

	m_nObjects = xObjects * zObjects;
	m_ppObjects = new CGameObject*[m_nObjects];

	int xStart = 0.0f;
	int zStart = 0.0f;

	for (int z = zStart, i = 0; z < zStart + zObjects; z++)
	{
		for (int x = xStart; x < xStart + xObjects; x++)
		{
			m_ppObjects[i] = new CFoliageObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

			float xPosition = x * fxPitch;
			float zPosition = z * fzPitch;

			float fHeight = pTerrain->GetHeight(xPosition, zPosition);

			m_ppObjects[i]->SetPosition(xPosition, fHeight, zPosition);

			if (i % 3 == 0) {
				m_ppObjects[i++]->SetChild(m_ppFoliageModel01->m_pModelRootObject, true);
			}
			else if (i % 3 == 1) {
				m_ppObjects[i++]->SetChild(m_ppFoliageModel02->m_pModelRootObject, true);
			}
			else
			{
				m_ppObjects[i++]->SetChild(m_ppFoliageModel03->m_pModelRootObject, true);
			}

		}
	}

}

void CFoliageShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer *pPlayer)
{
	float distance = 0.0f;
	UINT lodlevel = CGameObject::LOD_LEVEL0;
	for (int i = 0; i < m_nObjects; i++)
	{
		distance = GetDistanceToCamera(m_ppObjects[i], pCamera);


		//lodlevel= (distance < 20.0f) ? CGameObject::LOD_LEVEL0 : CGameObject::LOD_LEVEL1;

		if (distance < 30.0f)
		{
			lodlevel = CGameObject::LOD_LEVEL0;
		}
		else if (distance >= 30.0f && distance < 150.0f)
		{
			lodlevel = CGameObject::LOD_LEVEL1;
		}
		else if (distance > 150 && distance < 300.0f)
		{
			lodlevel = CGameObject::LOD_LEVEL2;
		}
		else
		{
			lodlevel = CGameObject::LOD_BILLBOARD;
		}

		//여기서 자신의 LOD레벨을 구하면 될듯하다.
		m_ppObjects[i]->SetLODlevel(lodlevel);
	}
}


void CFoliageShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{

	CStandardShader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_nObjects; j++)
	{

		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(m_fElapsedTime);
			m_ppObjects[j]->UpdateTransform(NULL);
			m_ppObjects[j]->Render(pd3dCommandList, m_ppObjects[j]->GetLodLevel(), pCamera);

		}

	}
}
