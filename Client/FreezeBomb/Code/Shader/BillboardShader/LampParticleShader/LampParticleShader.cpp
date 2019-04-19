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

void CLampParticleShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
	const map<string, CTexture*>& Context, void *pContext)
{
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

