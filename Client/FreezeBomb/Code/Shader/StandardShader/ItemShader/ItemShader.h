#pragma once
#include "../StandardShader.h"
#include "../../../ResourceManager/ResourceManager.h"

class CItem;
class CItemShader : public CStandardShader
{
public:
	CItemShader();
	virtual ~CItemShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CLoadedModelInfo*>& ModelMap, const unordered_map<unsigned char, RoundInfo>& MapObjectInfo, const map<string, Bounds*>& BoundMap, void* pContext = nullptr);

	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();
	void ItemDelete(string key);
	void ChangeRound();

	const map<string, CItem*>& getItemList()	const;

private:
	using RoundItemList = map<string, CItem*>;
	unordered_map<unsigned char, RoundItemList> m_ItemList;

	//unordered_multimap<unsigned char, RoundItemList> m_RemovedItemList;
	unordered_map<string, CItem*> m_RemovedItemList;
};