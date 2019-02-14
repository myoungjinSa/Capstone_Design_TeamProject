#include "../../Stdafx/Stdafx.h"
#include "BillboardMesh.h"

CBillboardMesh::CBillboardMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition)
	:CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = (fWidth * 0.5f) + fxPosition;
	float fy = (fHeight * 0.5f) + fyPosition;
	float fz = (fDepth * 0.5f) + fzPosition;

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf2TextureCoords0 = new XMFLOAT2[m_nVertices];

	if (fWidth == 0.0f)
	{
		if (fxPosition > 0.0f)
		{
			m_pxmf3Positions[0] = XMFLOAT3(+fx, +fy, -fz);
			m_pxmf3Positions[1] = XMFLOAT3(+fx, -fy, -fz);
			m_pxmf3Positions[2] = XMFLOAT3(+fx, -fy, +fz);
			m_pxmf3Positions[3] = XMFLOAT3(+fx, -fy, +fz);
			m_pxmf3Positions[4] = XMFLOAT3(+fx, +fy, +fz);
			m_pxmf3Positions[5] = XMFLOAT3(+fx, +fy, -fz);

			m_pxmf2TextureCoords0[0] = XMFLOAT2(1.0f, 0.0f);
			m_pxmf2TextureCoords0[1] = XMFLOAT2(1.0f, 1.0f);
			m_pxmf2TextureCoords0[2] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[3] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[4] = XMFLOAT2(0.0f, 0.0f);
			m_pxmf2TextureCoords0[5] = XMFLOAT2(1.0f, 0.0f);

		}
		else
		{
			m_pxmf3Positions[0] = XMFLOAT3(+fx, +fy, +fz);
			m_pxmf3Positions[1] = XMFLOAT3(+fx, -fy, +fz);
			m_pxmf3Positions[2] = XMFLOAT3(+fx, -fy, -fz);
			m_pxmf3Positions[3] = XMFLOAT3(+fx, -fy, -fz);
			m_pxmf3Positions[4] = XMFLOAT3(+fx, +fy, -fz);
			m_pxmf3Positions[5] = XMFLOAT3(+fx, +fy, +fz);

			m_pxmf2TextureCoords0[0] = XMFLOAT2(1.0f, 0.0f);
			m_pxmf2TextureCoords0[1] = XMFLOAT2(1.0f, 1.0f);
			m_pxmf2TextureCoords0[2] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[3] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[4] = XMFLOAT2(0.0f, 0.0f);
			m_pxmf2TextureCoords0[5] = XMFLOAT2(1.0f, 0.0f);
		}
	}
	else if (fHeight == 0.0f)
	{
		if (fyPosition > 0.0f)
		{
			m_pxmf3Positions[0] = XMFLOAT3(+fx, +fy, -fz);
			m_pxmf3Positions[1] = XMFLOAT3(+fx, +fy, +fz);
			m_pxmf3Positions[2] = XMFLOAT3(-fx, +fy, +fz);
			m_pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
			m_pxmf3Positions[4] = XMFLOAT3(-fx, +fy, -fz);
			m_pxmf3Positions[5] = XMFLOAT3(+fx, +fy, -fz);

			m_pxmf2TextureCoords0[0] = XMFLOAT2(1.0f, 0.0f);
			m_pxmf2TextureCoords0[1] = XMFLOAT2(1.0f, 1.0f);
			m_pxmf2TextureCoords0[2] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[3] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[4] = XMFLOAT2(0.0f, 0.0f);
			m_pxmf2TextureCoords0[5] = XMFLOAT2(1.0f, 0.0f);
		}
		else
		{
			m_pxmf3Positions[0] = XMFLOAT3(+fx, +fy, +fz);
			m_pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
			m_pxmf3Positions[2] = XMFLOAT3(-fx, +fy, -fz);
			m_pxmf3Positions[3] = XMFLOAT3(-fx, +fy, -fz);
			m_pxmf3Positions[4] = XMFLOAT3(-fx, +fy, +fz);
			m_pxmf3Positions[5] = XMFLOAT3(+fx, +fy, +fz);

			m_pxmf2TextureCoords0[0] = XMFLOAT2(1.0f, 0.0f);
			m_pxmf2TextureCoords0[1] = XMFLOAT2(1.0f, 1.0f);
			m_pxmf2TextureCoords0[2] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[3] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[4] = XMFLOAT2(0.0f, 0.0f);
			m_pxmf2TextureCoords0[5] = XMFLOAT2(1.0f, 0.0f);
		}
	}
	else if (fDepth == 0.0f)
	{
		if (fzPosition > 0.0f)
		{
			m_pxmf3Positions[0] = XMFLOAT3(+fx, +fy, +fz);
			m_pxmf3Positions[1] = XMFLOAT3(+fx, -fy, +fz);
			m_pxmf3Positions[2] = XMFLOAT3(-fx, -fy, +fz);
			m_pxmf3Positions[3] = XMFLOAT3(-fx, -fy, +fz);
			m_pxmf3Positions[4] = XMFLOAT3(-fx, +fy, +fz);
			m_pxmf3Positions[5] = XMFLOAT3(+fx, +fy, +fz);

			m_pxmf2TextureCoords0[0] = XMFLOAT2(1.0f, 0.0f);
			m_pxmf2TextureCoords0[1] = XMFLOAT2(1.0f, 1.0f);
			m_pxmf2TextureCoords0[2] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[3] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[4] = XMFLOAT2(0.0f, 0.0f);
			m_pxmf2TextureCoords0[5] = XMFLOAT2(1.0f, 0.0f);
		}
		else
		{
			m_pxmf3Positions[0] = XMFLOAT3(-fx, +fy, fz);
			m_pxmf3Positions[1] = XMFLOAT3(-fx, -fy, fz);
			m_pxmf3Positions[2] = XMFLOAT3(+fx, -fy, fz);
			m_pxmf3Positions[3] = XMFLOAT3(+fx, -fy, fz);
			m_pxmf3Positions[4] = XMFLOAT3(+fx, +fy, fz);
			m_pxmf3Positions[5] = XMFLOAT3(-fx, +fy, fz);

			m_pxmf2TextureCoords0[0] = XMFLOAT2(1.0f, 0.0f);
			m_pxmf2TextureCoords0[1] = XMFLOAT2(1.0f, 1.0f);
			m_pxmf2TextureCoords0[2] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[3] = XMFLOAT2(0.0f, 1.0f);
			m_pxmf2TextureCoords0[4] = XMFLOAT2(0.0f, 0.0f);
			m_pxmf2TextureCoords0[5] = XMFLOAT2(1.0f, 0.0f);
		}
	}

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3)*m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2)*m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);
	m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
}

CBillboardMesh::~CBillboardMesh()
{
	if (m_pd3dTextureCoord0Buffer)
		m_pd3dTextureCoord0Buffer->Release();
	if (m_pxmf2TextureCoords0)
		delete[] m_pxmf2TextureCoords0;
}

void CBillboardMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_pd3dTextureCoord0UploadBuffer)
		m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = NULL;
}

void CBillboardMesh::OnPreRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 2, pVertexBufferViews);
}

void CBillboardMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	OnPreRender(pd3dCommandList, nullptr);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}