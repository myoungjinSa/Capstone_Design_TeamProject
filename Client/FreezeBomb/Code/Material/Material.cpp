#include "../Stdafx/Stdafx.h"
#include "Material.h"
#include "../Texture/Texture.h"
#include "../Shader/StandardShader/StandardShader.h"
#include "../Shader/StandardShader/SkinnedAnimationShader/SkinnedAnimationShader.h"
#include "../Shader/StandardShader/EffectShader/MagicRingEffectShader.h"
#include "../Scene/Scene.h"

CShader* CMaterial::m_pSkinnedAnimationShader{ nullptr };
CShader* CMaterial::m_pStandardShader{ nullptr };

extern volatile size_t g_FileSize;

CMaterial::CMaterial()
{
	m_nTextures = 1;
	m_ppTextures = new CTexture*[m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}

CMaterial::CMaterial(int nTextures)
{
	m_nTextures = nTextures;

	m_ppTextures = new CTexture*[m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}

CMaterial::~CMaterial()
{
	if (m_pShader != nullptr)
	{
		m_pShader->Release();
		m_pShader = nullptr;
	}
	
	if (m_nTextures > 0)
	{
		for (int i = 0; i < m_nTextures; i++) 
			if (m_ppTextures[i]) 
				m_ppTextures[i]->Release();
		delete[] m_ppTextures;

		if (m_ppstrTextureNames) 
			delete[] m_ppstrTextureNames;
	}
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::SetTexture(CTexture *pTexture, UINT nTexture)
{
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->Release();
	m_ppTextures[nTexture] = pTexture;
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->AddRef();
}

void CMaterial::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) 
			m_ppTextures[i]->ReleaseUploadBuffers();
	}
}

void CMaterial::PrepareShaders(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_pStandardShader = new CStandardShader;
	m_pStandardShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pSkinnedAnimationShader = new CSkinnedAnimationShader;
	m_pSkinnedAnimationShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);



}
void CMaterial::ReleaseShaders()
{
	if (m_pStandardShader)
		delete m_pStandardShader;
	if (m_pSkinnedAnimationShader)
		delete m_pSkinnedAnimationShader;
}
void CMaterial::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AmbientColor, 16);	// Ambient
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AlbedoColor, 20);	// Diffuse
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4SpecularColor, 24);	// Specular
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4EmissiveColor, 28);	// Emissive

	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &m_nType, 32);						//	TexturesMask

	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) 
			m_ppTextures[i]->UpdateShaderVariable(pd3dCommandList, 0);
	}
}

void CMaterial::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbMaterial = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbMaterial->Map(0, NULL, (void**)&m_pcbMappedMaterial);
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pcbMappedMaterial != nullptr)
	{
		// AmbientColor 복사
		::memcpy(&m_pcbMappedMaterial->m_xmf4AmbientColor, &m_xmf4AmbientColor, sizeof(XMFLOAT4));
		// AlbedoColor 복사
		::memcpy(&m_pcbMappedMaterial->m_xmf4AlbedoColor, &m_xmf4AlbedoColor, sizeof(XMFLOAT4));
		// SpecularColor 복사
		::memcpy(&m_pcbMappedMaterial->m_xmf4SpecularColor, &m_xmf4SpecularColor, sizeof(XMFLOAT4));
		// EmissiveColor 복사
		::memcpy(&m_pcbMappedMaterial->m_xmf4EmissiveColor, &m_xmf4EmissiveColor, sizeof(XMFLOAT4));

		// nType 복사;
		::memcpy(&m_pcbMappedMaterial->m_nType, &m_nType, sizeof(UINT));

		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbMaterial->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dGpuVirtualAddress);
	}
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i])
			m_ppTextures[i]->UpdateShaderVariable(pd3dCommandList, 0);
	}
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList,int nTexture)
{
	if (m_pcbMappedMaterial != nullptr)
	{
		// AmbientColor 복사
		::memcpy(&m_pcbMappedMaterial->m_xmf4AmbientColor, &m_xmf4AmbientColor, sizeof(XMFLOAT4));
		// AlbedoColor 복사
		::memcpy(&m_pcbMappedMaterial->m_xmf4AlbedoColor, &m_xmf4AlbedoColor, sizeof(XMFLOAT4));
		// SpecularColor 복사
		::memcpy(&m_pcbMappedMaterial->m_xmf4SpecularColor, &m_xmf4SpecularColor, sizeof(XMFLOAT4));
		// EmissiveColor 복사
		::memcpy(&m_pcbMappedMaterial->m_xmf4EmissiveColor, &m_xmf4EmissiveColor, sizeof(XMFLOAT4));

		// nType 복사;
		::memcpy(&m_pcbMappedMaterial->m_nType, &m_nType, sizeof(UINT));

		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbMaterial->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dGpuVirtualAddress);
	}
	
	if (m_ppTextures[nTexture])
		m_ppTextures[nTexture]->UpdateShaderVariable(pd3dCommandList, 0);
	

}

void CMaterial::ReleaseShaderVariables()
{

}

void CMaterial::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR *pwstrTextureName, CTexture **ppTexture, CGameObject *pParent, FILE *pInFile, CShader *pShader)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);

	g_FileSize += sizeof(BYTE) * nReads;

	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);

	g_FileSize += sizeof(char) * nReads;

	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	if (strcmp(pstrTextureName, "null"))
	{
		SetMaterialType(nType);
#define SIZE 64
		char pstrFilePath[SIZE] = { '\0' };

		strcpy_s(pstrFilePath, SIZE, "../Resource/Textures/Model/");

		bDuplicated = (pstrTextureName[0] == '@');

#define Length 27
		strcpy_s(pstrFilePath + Length, SIZE - Length, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + Length + ((bDuplicated) ? (nStrLength - 1) : nStrLength), SIZE - Length - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, pwstrTextureName, SIZE, pstrFilePath, _TRUNCATE);

		//#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', pwstrTextureName);
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			*ppTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
			(*ppTexture)->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pwstrTextureName, 0, true);
			if (*ppTexture) 
				(*ppTexture)->AddRef();

			CScene::CreateShaderResourceViews(pd3dDevice, *ppTexture, nRootParameter, false);
		}
		else
		{
			if (pParent)
			{
				while (pParent)
				{
					if (!pParent->m_pParent) break;
					pParent = pParent->m_pParent;
				}
				CGameObject *pRootGameObject = pParent;
				*ppTexture = pRootGameObject->FindReplicatedTexture(pwstrTextureName);
				if (*ppTexture) (*ppTexture)->AddRef();
			}
		}
	}
}