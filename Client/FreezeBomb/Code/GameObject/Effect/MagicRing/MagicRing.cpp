#include "../../../Stdafx/Stdafx.h"
#include "MagicRing.h"
#include "../../../FrameTransform/FrameTransform.h"
#include "../../../Material/Material.h"
#include "../../../Shader/Shader.h"

CMagicRing::CMagicRing()
	:m_bActive{false}
{
}

CMagicRing::~CMagicRing()
{

}

void CMagicRing::Render(ID3D12GraphicsCommandList* pd3dCommandList,CCamera* pCamera, int nPipelineState)
{
	if (IsVisible(pCamera) == true)
	{
		CGameObject::Render(pd3dCommandList, pCamera, GameObject);
	}
}


