#pragma once

#include "../Mesh.h"

class CBillboardMesh : public CMesh
{
public:
	CBillboardMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float width, float height, float fDepth, float fxPosition, float fyPosition, float fzPosition);
	virtual ~CBillboardMesh();

	virtual void ReleaseUploadBuffers();
	virtual void OnPreRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet, int nInstance);
	//void SetLodLevel(UINT lodlevel) { m_LodLevel = lodlevel; }
	//UINT GetLodLevel()	const { return m_LodLevel; }
	
	void SetVertexPosition(UINT index,XMFLOAT3 pos) {
		m_pxmf3Positions[index] = pos;
	}
private:
	XMFLOAT2*									m_pxmf2TextureCoords0 = nullptr;
	ID3D12Resource*							m_pd3dTextureCoord0Buffer = nullptr;
	ID3D12Resource*							m_pd3dTextureCoord0UploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord0BufferView;

	//UINT												m_LodLevel = 0;
};