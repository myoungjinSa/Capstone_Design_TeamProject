#include "../../../Stdafx/Stdafx.h"
#include "ExplosionParticleShader.h"
#include "../../../GameObject/CubeParticle/ExplosionParticle.h"


CExplosionParticleShader::CExplosionParticleShader()
	:CCubeParticleShader()
{

}

CExplosionParticleShader::~CExplosionParticleShader()
{

}
D3D12_INPUT_LAYOUT_DESC CExplosionParticleShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CExplosionParticleShader::CreateVertexShader(int nPipelineState)
{
	switch (nPipelineState)
	{
	case GameObject:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSExplosionParticle", "vs_5_1", &m_pd3dPixelShaderBlob));
		break;
	case GameObject_Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSExplosionParticleShadow", "vs_5_1", &m_pd3dPixelShaderBlob));
		break;
	}
}
D3D12_SHADER_BYTECODE CExplosionParticleShader::CreatePixelShader(int nPipelineState)
{
	switch (nPipelineState)
	{
	case GameObject:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSExplosionParticle", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	case GameObject_Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSExplosionParticleShadow", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	}
}

 D3D12_DEPTH_STENCIL_DESC CExplosionParticleShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}
void CExplosionParticleShader::BuildObjects(ID3D12Device *pd3dDevice,ID3D12GraphicsCommandList *pd3dCommandList,ID3D12RootSignature* pd3dGraphicsRootSignature,void *pContext)
{
	COLOR c{ 255,12,0,0 };
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,c, 1.0f, 1.0f, 1.0f );
	
	int ParticleCount = 1000;

	for(int i =0;i<ParticleCount;++i)
	{
		CExplosionParticle* pCubeParticle = new CExplosionParticle();
		if(i == 0)
		{
			//첫번째 객체만 메쉬를 갖고 있으면 됨, 그림자 포함
			pCubeParticle->SetMesh(pCubeMesh);
			pCubeParticle->Initialize_Shadow(pCubeParticle);
		}
		pCubeParticle->SetRotationSpeed(720.0f);
		pCubeParticle->SetMovingSpeed(Random(100,500));
		//	pCubeParticle->SetVelocity(XMFLOAT3(-5.0f, 25.0f, 5.0f));
		pCubeParticle->SetAccel(XMFLOAT3(0.0f,-0.98f,0.0f));
		pCubeParticle->SetMass(20.0f);
		pCubeParticle->SetDuration(0.5f);
		pCubeParticle->SetBlowingUp(false);
		pCubeParticle->SetPosition(0, 0, 0);
		pCubeParticle->PrepareExplosion();

		m_ExplosionParticleList.emplace_back(pCubeParticle);
	}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CExplosionParticleShader::ResetParticles()
{
	
	for(auto& particle:m_ExplosionParticleList)
	{
		(*particle).SetElapsedTimeZero();
		(*particle).SetMovingSpeed(Random(100, 500));
		//(*particle).SetDuration(0.5f);

		(*particle).SetBlowingUp(false);
		//(*particle).PrepareExplosion();
	}
	cout << "ResetParticles호출" << endl;
}

void CExplosionParticleShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{

	for (auto iter = m_ExplosionParticleList.begin(); iter != m_ExplosionParticleList.end(); ++iter)
	{
		(*iter)->Animate(elapsedTime);
	}
	

}

void CExplosionParticleShader::ReleaseObjects()
{
	for (auto iter = m_ExplosionParticleList.begin(); iter != m_ExplosionParticleList.end();)
	{
		//(*iter)->Release();
		iter = m_ExplosionParticleList.erase(iter);
	}
	m_ExplosionParticleList.clear();
}
void CExplosionParticleShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	UpdateShaderVariables(pd3dCommandList);

	auto iter = m_ExplosionParticleList.begin();
	OnPrepareRender(pd3dCommandList, GameObject);
	(*iter)->Render(pd3dCommandList, pCamera, GameObject, m_ExplosionParticleList.size());
	OnPrepareRender(pd3dCommandList, GameObject_Shadow);
	(*iter)->Render(pd3dCommandList, pCamera, GameObject_Shadow, m_ExplosionParticleList.size());
}

void CExplosionParticleShader::SetParticleBlowUp(XMFLOAT3& position)
{

	for (auto iter = m_ExplosionParticleList.begin(); iter != m_ExplosionParticleList.end(); ++iter)
	{
		if ((*iter)->GetBlowingUp() == false)
		{
			(*iter)->SetPosition(position);
			(*iter)->SetBlowingUp(true);
		}
	}
}

void CExplosionParticleShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	
	m_pd3dInstancingData[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, nullptr, m_ExplosionParticleList.size() * sizeof(InstancingData), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dInstancingData[0]->Map(0, nullptr, (void**)&m_pMappedInstancingData[0]);
	
}

void CExplosionParticleShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dInstancingData[0])
	{
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = m_pd3dInstancingData[0]->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootShaderResourceView(7, GPUVirtualAddress);

		int i = 0;
		for (auto iter = m_ExplosionParticleList.begin(); iter != m_ExplosionParticleList.end(); ++iter)
		{
			XMStoreFloat4x4(&m_pMappedInstancingData[0][i++].m_World, XMMatrixTranspose(XMLoadFloat4x4(&(*iter)->m_xmf4x4World)));
		}
	}
}
void CExplosionParticleShader::ReleaseShaderVariables()
{
	
	if (m_pd3dInstancingData[0])
	{
		m_pd3dInstancingData[0]->Unmap(0, nullptr);
		m_pd3dInstancingData[0]->Release();
	}
	
}
