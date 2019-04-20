#include "../../Stdafx/Stdafx.h"
#include "CubeParticleShader.h"
#include "../../Texture/Texture.h"
#include "../../GameObject/CubeParticle/CubeParticle.h"
#include "../../Material/Material.h"
#include "../Shader.h"

CCubeParticleShader::CCubeParticleShader()
{
}

CCubeParticleShader::~CCubeParticleShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; ++i)
			if (m_ppd3dPipelineStates[i])
				m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

void CCubeParticleShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
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

D3D12_INPUT_LAYOUT_DESC CCubeParticleShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CCubeParticleShader::CreateVertexShader(int nPipelineState)
{
	switch (nPipelineState)
	{
	case GameObject:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSCubeParticle", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	case GameObject_Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSCubeParticleShadow", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	}

}

D3D12_SHADER_BYTECODE CCubeParticleShader::CreatePixelShader(int nPipelineState)
{
	switch (nPipelineState)
	{
	case GameObject:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSCubeParticle", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	case GameObject_Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSCubeParticleShadow", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	}
}

void CCubeParticleShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string,CTexture*>& Context,void *pContext)
{
	auto iter = Context.find("IceTexture");
	if (iter != Context.end())
	{
		CCubeMeshTextured* pCubeMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 0.5f, 0.5f, 0.5f);
		CMaterial* pMaterial = new CMaterial(1);
		pMaterial->SetTexture((*iter).second, 0);
		
		int ParticleCount = 100;

		for (int i = 0; i < ParticleCount; i++)
		{
			CCubeParticle* pCubeParticle = new CCubeParticle(1);
			if (i == 0)
			{
				// 첫번째 객체만 메쉬를 갖고있으면됨, 그림자포함
				pCubeParticle->SetMesh(pCubeMesh);
				pCubeParticle->SetMaterial(0, pMaterial);
				pCubeParticle->Initialize_Shadow(pCubeParticle);
			}
			pCubeParticle->setRotationSpeed(720.f);
			pCubeParticle->setMovingSpeed(Random(10, 10));
			pCubeParticle->setDuration(1.0f);
			pCubeParticle->setBlowingUp(false);
			pCubeParticle->SetPosition(0, 0, 0);
			pCubeParticle->PrepareExplosion();

			m_CubeParticleList.emplace_back(pCubeParticle);
		}

		CreateShaderVariables(pd3dDevice, pd3dCommandList);
	}
}

void CCubeParticleShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
	for (auto iter = m_CubeParticleList.begin(); iter != m_CubeParticleList.end(); ++iter)
	{
		(*iter)->Animate(elapsedTime);
	}
}

void CCubeParticleShader::ReleaseObjects()
{
	for (auto iter = m_CubeParticleList.begin(); iter != m_CubeParticleList.end();)
	{
		(*iter)->Release();
		iter = m_CubeParticleList.erase(iter);
	}
	m_CubeParticleList.clear();
}

void CCubeParticleShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates)
	{
		if (m_ppd3dPipelineStates[nPipelineState])
			pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
	}
}

void CCubeParticleShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	UpdateShaderVariables(pd3dCommandList);
	auto iter = m_CubeParticleList.begin();

	OnPrepareRender(pd3dCommandList, GameObject);
	(*iter)->Render(pd3dCommandList, pCamera, GameObject, m_CubeParticleList.size());
	OnPrepareRender(pd3dCommandList, GameObject_Shadow);
	(*iter)->Render(pd3dCommandList, pCamera, GameObject_Shadow, m_CubeParticleList.size());
}

void CCubeParticleShader::SetParticleBlowUp(XMFLOAT3& position)
{
	for (auto iter = m_CubeParticleList.begin(); iter != m_CubeParticleList.end(); ++iter)
	{
		if ((*iter)->getBlowingUp() == false)
		{
			(*iter)->SetPosition(position);
			(*iter)->setBlowingUp(true);
		}
	}
}

void CCubeParticleShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dInstancingData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, m_CubeParticleList.size() * sizeof(InstancingData), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dInstancingData->Map(0, nullptr, (void**)&m_pMappedInstancingData);
}

void CCubeParticleShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dInstancingData)
	{
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = m_pd3dInstancingData->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootShaderResourceView(7, GPUVirtualAddress);

		int i = 0;
		for (auto iter = m_CubeParticleList.begin(); iter != m_CubeParticleList.end(); ++iter)
		{
			XMStoreFloat4x4(&m_pMappedInstancingData[i++].m_World, XMMatrixTranspose(XMLoadFloat4x4(&(*iter)->m_xmf4x4World)));
		}
	}
}

void CCubeParticleShader::ReleaseShaderVariables()
{
	if (m_pd3dInstancingData)
	{
		m_pd3dInstancingData->Unmap(0, nullptr);
		m_pd3dInstancingData->Release();
	}
}
