#include "../../Stdafx/Stdafx.h"
#include "SkyBox.h"
#include "../../Texture/Texture.h"
#include "../../Material/Material.h"
#include "../../Shader/SkyBoxShader/SkyBoxShader.h"
#include "../../Scene/Scene.h"

extern unsigned char g_Round;
CSkyBox::CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : CGameObject(1)
{
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	if (m_pMesh)
	{
		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader)
						m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);

					//라운드 별로 다른 SkyBox 텍스쳐를 사용
					if(g_Round<2)
						m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList,g_Round);
				}
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}
}
