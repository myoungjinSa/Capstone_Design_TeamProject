#pragma once

class CLoadedModelInfo;
class CStandardMesh;
class CFrameTransform
{
public:
	CFrameTransform(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CLoadedModelInfo* pModel);
	~CFrameTransform();

	void SetFrameMesh(int nIndex, CStandardMesh* pFrameMesh) { m_ppFrameMeshes[nIndex] = pFrameMesh; }

	void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void SetFrameMeshWorldConstantBuffer();

	int 							m_nFrameMeshes = 0;
	CStandardMesh**		m_ppFrameMeshes = nullptr;

	ID3D12Resource**	m_ppd3dcbWorld = nullptr;
	XMFLOAT4X4**		m_ppcbxmf4x4World = nullptr;
};