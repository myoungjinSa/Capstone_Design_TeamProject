#pragma once

#include "../StandardShader.h"

class CLoadedModelInfo;
struct Bounds;
class CItem;
class CItemShader : public CStandardShader
{
public:
	CItemShader();
	virtual ~CItemShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		const map<string, CLoadedModelInfo*>& ModelMap, const map<string, Bounds*>& Context, void *pContext = nullptr);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();
	void ItemDelete(string key);
	

	const map<string, CItem*>& getItemMap()	const { return m_ItemMap; }

private:
	map<string, CItem*> m_ItemMap;
};