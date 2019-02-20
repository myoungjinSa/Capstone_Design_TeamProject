#include "../../../Stdafx/Stdafx.h"
#include "../../../GameObject/Cube/IceParticle/IceCube.h"
#include "../../../Texture/Texture.h"
#include "../../../Material/Material.h"
#include "../../../Mesh/Mesh.h"
#include "IceParticleShader.h"

CCubeIceShader::CCubeIceShader()
{
}

CCubeIceShader::~CCubeIceShader()
{
}

D3D12_INPUT_LAYOUT_DESC CCubeIceShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CCubeIceShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSIceCube", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CCubeIceShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSIceCube", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CCubeIceShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string,CTexture*>& Context,void *pContext)
{
	CCubeMeshTextured *pExplosionMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 2.f, 2.0f, 2.0f);

	CMaterial*	pIceMaterial = new CMaterial(1);
	auto iter = Context.find("IceTexture");
	if (iter != Context.end())
	{
		pIceMaterial->SetTexture((*iter).second, 0);
	}

	CIceCube* pIceCube = new CIceCube(1);
	pIceCube->SetMesh(pExplosionMesh);
	pIceCube->SetPosition(50.0f, 15.0f, 50.0f);
	pIceCube->SetMaterial(0, pIceMaterial);
	pIceCube->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pIceCube->SetRotationSpeed(90.0f);
	pIceCube->SetMovingSpeed(1.0f);
	pIceCube->SetExplosionDuration(5.0f);
	pIceCube->SetExplode(true);

	m_vIceCube.emplace_back(pIceCube);
	
	CIceCube::PrepareExplosion(pd3dDevice, pd3dCommandList);
}

void CCubeIceShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer *pPlayer)
{
	for (auto iter = m_vIceCube.begin(); iter != m_vIceCube.end(); ++iter)
	{
		(*iter)->Animate(fTimeElapsed);
	}
}

void CCubeIceShader::ReleaseObjects()
{
	for (auto iter = m_vIceCube.begin(); iter != m_vIceCube.end();)
	{
		delete (*iter);
		iter = m_vIceCube.erase(iter);
	}
	m_vIceCube.clear();
}

void CCubeIceShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	
	for (auto iter = m_vIceCube.begin(); iter != m_vIceCube.end(); ++iter)
	{
		(*iter)->Render(pd3dCommandList, pCamera);
	}
}
