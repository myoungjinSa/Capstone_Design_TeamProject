#include "../../../Stdafx/Stdafx.h"
#include "FoliageShader.h"
#include "../../../GameObject/GameObject.h"
#include "../../../GameObject/Foliage/Foliage.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../FrameTransform/FrameTransform.h"

CFoliageShader::CFoliageShader()
{

}

CFoliageShader::~CFoliageShader()
{
}

void CFoliageShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
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
	}

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//D3D12_SHADER_BYTECODE CFoliageShader::CreateVertexShader(int nPipelineState)
//{
//	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSFoliage", "vs_5_1", &m_pd3dPixelShaderBlob));
//}

D3D12_SHADER_BYTECODE CFoliageShader::CreatePixelShader(int nPipelineState)
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSFoliage", "ps_5_1", &m_pd3dPixelShaderBlob));
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
	const map<string,CLoadedModelInfo*>& ModelMap,void *pContext = NULL)
{
	//m_ppFoliageModel01 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Grass_C_01.bin", this, false, "Surrounding");
	//m_ppFoliageModel02 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Grass_D_01.bin", this, false, "Surrounding");
	//m_ppFoliageModel03 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Plant_c_01.bin", this, false, "Surrounding");

	CTerrain *pTerrain = (CTerrain *)pContext;

	float fxPitch = 12.0f;
	float fzPitch = 12.0f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = int(fTerrainWidth * 0.1f / fxPitch);
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

				m_ppObjects[i]->SetChild(m_ppFoliageModel01->m_pModelRootObject, true);
				m_ppObjects[i++]->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, m_ppFoliageModel01);
			}
			else if (i % 3 == 1)
			{
				m_ppObjects[i]->SetChild(m_ppFoliageModel02->m_pModelRootObject, true);
				m_ppObjects[i++]->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, m_ppFoliageModel02);
			}
			else {
				m_ppObjects[i]->SetChild(m_ppFoliageModel03->m_pModelRootObject, true);
				m_ppObjects[i++]->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, m_ppFoliageModel03);
			}
		}
	}

	//CreateShaderVariables(pd3dDevice, pd3dCommandList);


	if (m_ppFoliageModel01)
		delete m_ppFoliageModel01;
	if (m_ppFoliageModel02)
		delete m_ppFoliageModel02;
	if (m_ppFoliageModel03)
		delete m_ppFoliageModel03;
}

void CFoliageShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
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
		else if (distance >= 30.0f && distance < 100.0f)
		{
			lodlevel = CGameObject::LOD_LEVEL1;
		}
		else if (distance > 100 && distance < 200.0f)
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


void CFoliageShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	//UpdateShaderVariables(pd3dCommandList);
	CStandardShader::Render(pd3dCommandList, pCamera, nPipelineState);
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i])
		{
			m_ppObjects[i]->Animate(m_fElapsedTime);
			m_ppObjects[i]->UpdateTransform(NULL,false);
			m_ppObjects[i]->Render(pd3dCommandList, m_ppObjects[i]->GetLodLevel(), pCamera, nPipelineState);
		}
	}
}
//void CFoliageShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	m_pd3dInstancingData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, m_nObjects * sizeof(InstancingData), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
//	m_pd3dInstancingData->Map(0, nullptr, (void**)&m_pMappedInstancingData);
//}
//
//void CFoliageShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	if (m_pd3dInstancingData)
//	{
//		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = m_pd3dInstancingData->GetGPUVirtualAddress();
//		pd3dCommandList->SetGraphicsRootShaderResourceView(24, GPUVirtualAddress);
//		for (int i=0; i < m_nObjects; ++i)
//		{
//			XMStoreFloat4x4(&m_pMappedInstancingData[i].m_World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
//		}
//	}
//}

//void CFoliageShader::ReleaseShaderVariables()
//{
//	if (m_pd3dInstancingData)
	//{
	//	m_pd3dInstancingData->Unmap(0, nullptr);
	//	m_pd3dInstancingData->Release();
	//}
//}

void CFoliageShader::ReleaseObjects()
{
	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->Release();

	//CStandardShader::ReleaseObjects();
}
void CFoliageShader::ReleaseUploadBuffers()
{
	//for (int i = 0; i < m_nObjects; ++i)
		//m_ppObjects[i]->ReleaseUploadBuffers();
}