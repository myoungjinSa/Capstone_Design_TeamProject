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

	void SetLodLevel(UINT lodlevel) { m_LodLevel = lodlevel; }
	UINT GetLodLevel()	const { return m_LodLevel; }

private:
	XMFLOAT2*									m_pxmf2TextureCoords0 = nullptr;
	ID3D12Resource*							m_pd3dTextureCoord0Buffer = nullptr;
	ID3D12Resource*							m_pd3dTextureCoord0UploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord0BufferView;

	UINT												m_LodLevel = 0;
};

class CVertex
{
public:
	XMFLOAT3						m_xmf3Position;

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CTexturedVertex : public CVertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CTexturedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; }
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; }
	~CTexturedVertex() { }
};


class CUIMesh : public CMesh
{
public:
	CUIMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CUIMesh();

	void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet);

private:
	UINT											m_nStride = 0;

	ID3D12Resource*						m_pd3dVertexBuffer = nullptr;
	ID3D12Resource*						m_pd3dVertexUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;
};
