#include "../../Stdafx/Stdafx.h"
#include "SkyBox.h"
#include "../../Texture/Texture.h"
#include "../../Material/Material.h"
#include "../../Shader/SkyBoxShader/SkyBoxShader.h"
#include "../../Scene/Scene.h"

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

	CGameObject::Render(pd3dCommandList, pCamera, nPipelineState);
}
