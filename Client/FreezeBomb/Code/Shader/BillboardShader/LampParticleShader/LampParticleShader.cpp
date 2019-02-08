#include "../../../Stdafx/Stdafx.h"
#include "LampParticleShader.h"
#include "../../../GameObject/GameObject.h"
#include "../../../GameObject/Billboard/LampParticle/LampParticle.h"
#include "../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../Material/Material.h"
#include "../../../Texture/Texture.h"
#include "../../../Scene/Scene.h"

CLampParticleShader::CLampParticleShader()
{
}

CLampParticleShader::~CLampParticleShader()
{
}

D3D12_SHADER_BYTECODE CLampParticleShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSLampParticle", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CLampParticleShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSLampParticle", "ps_5_1", &m_pd3dVertexShaderBlob));
}

void CLampParticleShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	//m_nObjects = 1;
	//m_ppObjects = new CGameObject*[m_nObjects];

	//CWickParticle* pWickParticle = new CWickParticle(1);
	//CBillboardMesh* pBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	//pWickParticle->SetMesh(pBillboardMesh);

	//CTexture *pParticleTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	//pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Model/Textures/fire.dds", 0);

	////UINT ncbElementBytes = ((sizeof(CB_PARTICLE_INFO) + 255)&~255);

	////CScene::CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbParticleObjects, ncbElementBytes);
	//CScene::CreateShaderResourceViews(pd3dDevice, pParticleTexture, 17, false);

	//CMaterial *pParticleMaterial = new CMaterial(1);
	//pParticleMaterial->SetTexture(pParticleTexture);

	//pWickParticle->SetMaterial(0, pParticleMaterial);

	//m_ppObjects[0] = pWickParticle;
}

void CLampParticleShader::AnimateObjects(float elapsedTime, CCamera *pCamera, CPlayer *pPlayer)
{
	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; ++i)
			if (m_ppObjects[i])
			{
				XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
				((CLampParticle*)m_ppObjects[i])->SetLookAt(xmf3CameraPosition);
				m_ppObjects[i]->Animate(elapsedTime);
			}

	}
}

void CLampParticleShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);

	if (m_ppObjects)
		for (int i = 0; i < m_nObjects; i++)
			if (m_ppObjects[i])
				m_ppObjects[i]->Render(pd3dCommandList, pCamera);
}

void CLampParticleShader::ReleaseObjects()
{
	if (m_ppObjects)
		for (int i = 0; i < m_nObjects; ++i)
			m_ppObjects[i]->Release();
}

