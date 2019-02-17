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
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	virtual ~CVertex() { }

	XMFLOAT3						m_xmf3Position;
};

class CIlluminatedVertex : public CVertex
{
public:
	CIlluminatedVertex();
	CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f));
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f));
	virtual ~CIlluminatedVertex();

private:
	XMFLOAT3 m_xmf3Normal;
};

class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CMeshIlluminated();

	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices);
};

class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();

	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet);

private:
	UINT m_pnIndices[36];
	UINT m_nIndices = 0;
	UINT m_nStride = 0;

	ID3D12Resource*						m_pd3dVertexBuffer = NULL;
	ID3D12Resource*						m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;

	ID3D12Resource*						m_pd3dIndexBuffer = NULL;
	ID3D12Resource*						m_pd3dIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW	m_d3dIndexBufferView;
};