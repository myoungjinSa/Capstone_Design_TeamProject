#pragma once

#include "../SkinnedAnimationShader.h"

class CLoadedModelInfo;
struct Bounds;
class CSkinnedAnimationObjectShader : public CSkinnedAnimationShader
{
public:
	CSkinnedAnimationObjectShader();
	virtual ~CSkinnedAnimationObjectShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CLoadedModelInfo*>& ModelMap, const map<string, Bounds*>& Context,const int& nPlayerCount, void *pContext = nullptr);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera,CPlayer* pPlayer = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);

	void MappingUserToEvilbear(char id);
	
	vector<char> m_vMaterial;
private:
	
	char  m_userID;
	int   m_userCount;
	float m_elapsedTime = 0.f;
};