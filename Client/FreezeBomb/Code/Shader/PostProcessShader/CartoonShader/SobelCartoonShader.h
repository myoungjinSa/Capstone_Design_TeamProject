#pragma once
#include "../ComputeShader/ComputeShader.h"


class CSobelCartoonShader : public CComputeShader
{
public:
	CSobelCartoonShader();
	virtual ~CSobelCartoonShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pContext);
	virtual void ReleaseObjects();
	
	void SobelFilter(ID3D12GraphicsCommandList *pd3dCommandList, ID3D12Resource* pRenderTexture, ID3D12Resource **pDestTexture, CCamera *pCamera)
	{
		D3D12_RESOURCE_BARRIER d3dResourceBarrier[9];
		::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER) * 9);

		d3dResourceBarrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[0].Transition.pResource = pRenderTexture;				//이부분 주의
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		d3dResourceBarrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[0]);

		d3dResourceBarrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[1].Transition.pResource = m_pTexture[0]->m_ppd3dTextures[0];// pDestTexture[0];				//이부분 주의
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		d3dResourceBarrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[1]);

		pd3dCommandList->CopyResource(m_pTexture[0]->m_ppd3dTextures[0], pRenderTexture);

		d3dResourceBarrier[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[2].Transition.pResource = m_pTexture[0]->m_ppd3dTextures[0];				//이부분 주의
		d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		d3dResourceBarrier[2].Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		d3dResourceBarrier[2].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[2]);

		pd3dCommandList->SetComputeRootSignature(m_pd3dComputeRootSignature);
		pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);

		if (m_pd3dCbvSrvUavDescriptorHeap) {
			pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvUavDescriptorHeap);
		}

		pd3dCommandList->SetComputeRootDescriptorTable(0, m_pTexture[0]->m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
		pd3dCommandList->SetComputeRootDescriptorTable(2, m_pTexture[0]->m_pRootArgumentInfos[1].m_d3dUavGpuDescriptorHandle);

		d3dResourceBarrier[3].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[3].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[3].Transition.pResource = m_pTexture[0]->m_ppd3dTextures[1];				//이부분 주의
		d3dResourceBarrier[3].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		d3dResourceBarrier[3].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[3].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[3]);

		UINT numGroupsX = (UINT)ceilf(FRAME_BUFFER_WIDTH / 16.0f);
		UINT numGroupsY = (UINT)ceilf(FRAME_BUFFER_HEIGHT / 16.0f);
		pd3dCommandList->Dispatch(numGroupsX, numGroupsY, 1);

		d3dResourceBarrier[4].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[4].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[4].Transition.pResource = m_pTexture[0]->m_ppd3dTextures[1];				//이부분 주의
		d3dResourceBarrier[4].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[4].Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		d3dResourceBarrier[4].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[4]);

		d3dResourceBarrier[5].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[5].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[5].Transition.pResource = pRenderTexture;				//이부분 주의
		d3dResourceBarrier[5].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		d3dResourceBarrier[5].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		d3dResourceBarrier[5].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[5]);

		d3dResourceBarrier[6].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[6].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[6].Transition.pResource = m_pTexture[0]->m_ppd3dTextures[1];				//이부분 주의
		d3dResourceBarrier[6].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		d3dResourceBarrier[6].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		d3dResourceBarrier[6].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[6]);

		pd3dCommandList->CopyResource(pRenderTexture, m_pTexture[0]->m_ppd3dTextures[1]);

		d3dResourceBarrier[7].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[7].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[7].Transition.pResource = m_pTexture[0]->m_ppd3dTextures[1];				//이부분 주의
		d3dResourceBarrier[7].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		d3dResourceBarrier[7].Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		d3dResourceBarrier[7].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[7]);


		d3dResourceBarrier[8].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier[8].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier[8].Transition.pResource = pRenderTexture;				//이부분 주의
		d3dResourceBarrier[8].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		d3dResourceBarrier[8].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier[8].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier[8]);
	}
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, ID3D12Resource *pRenderTexture, ID3D12Resource **pDestTexture, CCamera *pCamera);

protected:
	
	CTexture**						m_pTexture{ nullptr };
	int								m_nTexture;

	UINT							m_nWidth;
	UINT							m_nHeight;

};