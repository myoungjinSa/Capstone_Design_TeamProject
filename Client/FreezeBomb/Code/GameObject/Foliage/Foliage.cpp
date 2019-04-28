#include "../../Stdafx/Stdafx.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../../Mesh/LODMesh/LODMesh.h.h"
#include "Foliage.h"

CFoliageObject::CFoliageObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
}

CFoliageObject::~CFoliageObject()
{
}

void CFoliageObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT lodlevel, CCamera *pCamera, int nPipelineState)
{
	OnPrepareRender();

	if (m_pMesh)
	{
		if (!m_pSkinningBoneTransforms) UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
					m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
				}
				if (m_pMesh->GetLodLevel() == lodlevel) 
					m_pMesh->Render(pd3dCommandList, i, lodlevel);
			}
		}
	}

	if (m_pSibling)
		m_pSibling->Render(pd3dCommandList, lodlevel, pCamera, nPipelineState);
	if (m_pChild)
		m_pChild->Render(pd3dCommandList, lodlevel, pCamera, nPipelineState);
}

CGameObject* CFoliageObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject* pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes, string Type)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObject *pGameObject = NULL;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObject();

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pGameObject->m_pstrFrameName, sizeof(char), nStrLength, pInFile);


			pGameObject->m_pstrFrameName[nStrLength] = '\0';

			//LOD·¹º§ ¼³Á¤
			if (strstr(pGameObject->m_pstrFrameName, "LOD0"))
			{
				pGameObject->SetLODlevel(0);
			}
			else if (strstr(pGameObject->m_pstrFrameName, "LOD1"))
			{
				pGameObject->SetLODlevel(1);
			}
			else if (strstr(pGameObject->m_pstrFrameName, "LOD2"))
			{
				pGameObject->SetLODlevel(2);
			}


		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			if (pGameObject->GetLodLevel() == LODLEVEL::LOD_LEVEL0)
			{
				CLODMesh* pMesh = new CLODMesh(pd3dDevice, pd3dCommandList);
				pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
				pGameObject->SetMesh(pMesh);
				pMesh->SetLodLevel(LODLEVEL::LOD_LEVEL0);
			}
			else if (pGameObject->GetLodLevel() == LODLEVEL::LOD_LEVEL1)
			{
				CLODMesh* pMesh = new CLODMesh(pd3dDevice, pd3dCommandList);
				pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
				pGameObject->SetMesh(pMesh);
				pMesh->SetLodLevel(LODLEVEL::LOD_LEVEL1);
			}
			else if (pGameObject->GetLodLevel() == LODLEVEL::LOD_LEVEL2)
			{
				CLODMesh* pMesh = new CLODMesh(pd3dDevice, pd3dCommandList);
				pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
				pGameObject->SetMesh(pMesh);
				pMesh->SetLodLevel(LODLEVEL::LOD_LEVEL2);
			}
			else {
				//ºôº¸µå
				//CBillboardObject* pBillboardObject = new CGrassFoliageBillboardObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				//pGameObject->SetChild(pBillboardObject, true);
				////pMesh->SetLodLevel(LODLEVEL::LOD_LEVEL2);			//ºôº¸µå 
				//pGameObject->m_pMesh->SetLodLevel(LODLEVEL::LOD_BILLBOARD);
			}

		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			CSkinnedMesh *pSkinnedMesh = new CSkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); //<Mesh>:
			pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pGameObject->SetMesh(pSkinnedMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature , pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject *pChild = CFoliageObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader, pnSkinnedMeshes, Type);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

CLoadedModelInfo *CFoliageObject::LoadGeometryAndAnimationFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader, bool bHasAnimation, string Type)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CLoadedModelInfo *pLoadedModel = new CLoadedModelInfo();
	//¸ðµ¨ °èÃþÁ¤º¸ ·Îµå
	pLoadedModel->m_pModelRootObject = CFoliageObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes, Type);
	if (bHasAnimation)
	{
		pLoadedModel->m_pAnimationSets = CGameObject::LoadAnimationFromFile(pInFile, pLoadedModel->m_pModelRootObject);
	}

	pLoadedModel->m_pModelRootObject->CacheSkinningBoneFrames(pLoadedModel->m_pModelRootObject);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, "Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pLoadedModel);
}