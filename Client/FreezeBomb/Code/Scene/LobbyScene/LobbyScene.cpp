#include "../../Stdafx/Stdafx.h"
#include "LobbyScene.h"
#include "../../Texture/Texture.h"
#include "../../Shader/BillboardShader/UIShader/CharacterSelectUIShader/CharacterSelectUIShader.h"
#include "../../Direct2D/Direct2D.h"
#include "../../GameFramework/GameFramework.h"

array<char, MAX_USER> CLobbyScene::m_ClientsCharacter;

CLobbyScene::CLobbyScene()
{
}

CLobbyScene::~CLobbyScene()
{
}

ID3D12RootSignature *CLobbyScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 21; // t21: gtxtSelectTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[0].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 10;	
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[1];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 2;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CLobbyScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	m_shaderMap.clear();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->ReleaseShaderVariables();
		m_ppShaders[i]->ReleaseObjects();
	}
	delete[] m_ppShaders;
}

void CLobbyScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	int index = 0;
	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	CCharacterSelectUIShader* pSelectShader = new CCharacterSelectUIShader;
	pSelectShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, nullptr);
	pSelectShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppShaders[index++] = pSelectShader;
	m_shaderMap.emplace("Select", pSelectShader);

	// 재질 초기화
	for (int i = 0; i < m_ClientsCharacter.size(); ++i)
		m_ClientsCharacter[i] = -1;
}

void CLobbyScene::AnimateObjects(ID3D12GraphicsCommandList* pd3dCommandList, float elapsedTime)
{
}

void CLobbyScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pd3dGraphicsRootSignature)
		pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	
	for(int i=0;i<m_nShaders;++i)
		m_ppShaders[i]->Render(pd3dCommandList,0 );
}

XMFLOAT3 CLobbyScene::ScreenPosition(int x, int y)
{
	D3D12_VIEWPORT d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };

	XMFLOAT3 screenPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	screenPosition.x = (((2.0f * x) / d3dViewport.Width) - 1) / 1;
	screenPosition.y = - (((2.0f * y) / d3dViewport.Height) - 1) / 1;

	screenPosition.z = 0.0f;

	return screenPosition;
}

void CLobbyScene::OnProcessingMouseMessage(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam)
{
	int mouseX = LOWORD(lParam);
	int mouseY = HIWORD(lParam);

	switch(nMessageID)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		{
			auto iter = m_shaderMap.find("Select");
			if (iter != m_shaderMap.end())
			{
				XMFLOAT3 position = ScreenPosition(mouseX, mouseY);
				reinterpret_cast<CCharacterSelectUIShader*>(m_ppShaders[CHARACTER_SELECT])->CallbackMouse(nMessageID, position.x, position.y);
			}
			break;
		}
	}
}

void CLobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYUP:
			reinterpret_cast<CCharacterSelectUIShader*>(m_ppShaders[CHARACTER_SELECT])->CallbackKeyboard(wParam);
			break;

		default:
			break;
	}
}

void CLobbyScene::UIRender()
{
	UIClientsNameTextRender();
	UIChoiceCharacterRender();
	UIClientsReadyTextRender();
}

void CLobbyScene::UIClientsNameTextRender()
{
	UINT originX = 1200;
	UINT originY = 800;

	D2D1_RECT_F pos[6] =
	{
		D2D1::RectF((80 * FRAME_BUFFER_WIDTH) / originX, (50 * FRAME_BUFFER_HEIGHT) / originY, (200 * FRAME_BUFFER_WIDTH) / originX, (60 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((305 * FRAME_BUFFER_WIDTH) / originX, (50 * FRAME_BUFFER_HEIGHT) / originY, (425 * FRAME_BUFFER_WIDTH) / originX, (60 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((532 * FRAME_BUFFER_WIDTH) / originX, (50 * FRAME_BUFFER_HEIGHT) / originY, (652 * FRAME_BUFFER_WIDTH) / originX, (60 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((80 * FRAME_BUFFER_WIDTH) / originX, (235 * FRAME_BUFFER_HEIGHT) / originY, (200 * FRAME_BUFFER_WIDTH) / originX, (245 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((305 * FRAME_BUFFER_WIDTH) / originX, (235 * FRAME_BUFFER_HEIGHT) / originY, (425 * FRAME_BUFFER_WIDTH) / originX, (245 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((532 * FRAME_BUFFER_WIDTH) / originX, (235 * FRAME_BUFFER_HEIGHT) / originY, (652 * FRAME_BUFFER_WIDTH) / originX, (245 * FRAME_BUFFER_HEIGHT) / originY),
	};

#ifndef _WITH_SERVER_

#else
	for (auto client : CGameFramework::GetClientsInfo())
	{
		wchar_t name[32] = { 0, };
		int nLen = MultiByteToWideChar(CP_ACP, 0, client.second.name, strlen(client.second.name), NULL, NULL);
		MultiByteToWideChar(CP_ACP, 0, client.second.name, strlen(client.second.name), name, nLen);
		name[nLen] = '\0';
		CDirect2D::GetInstance()->Render("피오피동글", "검은색", name, pos[client.second.id]);
	}
#endif
}

void CLobbyScene::UIChoiceCharacterRender()
{
	CDirect2D::GetInstance()->Render("Characters");

	UINT originX = 1280;
	UINT originY = 720;

	D2D1_RECT_F pos[6] =
	{
		D2D1::RectF((89 * FRAME_BUFFER_WIDTH) / originX, (66 * FRAME_BUFFER_HEIGHT) / originY, (209 * FRAME_BUFFER_WIDTH) / originX, (187 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((330 * FRAME_BUFFER_WIDTH) / originX, (66 * FRAME_BUFFER_HEIGHT) / originY, (450 * FRAME_BUFFER_WIDTH) / originX, (187 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((571 * FRAME_BUFFER_WIDTH) / originX, (66 * FRAME_BUFFER_HEIGHT) / originY, (691 * FRAME_BUFFER_WIDTH) / originX, (187 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((89 * FRAME_BUFFER_WIDTH) / originX, ((187 + 46) * FRAME_BUFFER_HEIGHT) / originY, (209 * FRAME_BUFFER_WIDTH) / originX, ((187 + 46 + 120) * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((330 * FRAME_BUFFER_WIDTH) / originX, ((187 + 46) * FRAME_BUFFER_HEIGHT) / originY, (450 * FRAME_BUFFER_WIDTH) / originX, ((187 + 46 + 120) * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((571 * FRAME_BUFFER_WIDTH) / originX, ((187 + 46) * FRAME_BUFFER_HEIGHT) / originY, (691 * FRAME_BUFFER_WIDTH) / originX, ((187 + 46 + 120) * FRAME_BUFFER_HEIGHT) / originY),
	};

#ifndef _WITH_SERVER_
	if (CCharacterSelectUIShader::GetChoiceCharacter() >= 0)
	{
		pos = D2D1::RectF((89 * FRAME_BUFFER_WIDTH) / originX, (66 * FRAME_BUFFER_HEIGHT) / originY, (209 * FRAME_BUFFER_WIDTH) / originX, (187 * FRAME_BUFFER_HEIGHT) / originY);
		CDirect2D::GetInstance()->Render("ChoiceCharacter", pos[0], CCharacterSelectUIShader::GetChoiceCharacter(), 0);
	}

#else
	for (auto client : CGameFramework::GetClientsInfo())
	{
		char character = m_ClientsCharacter[client.second.id];
		if (character == -1)		continue;
		CDirect2D::GetInstance()->Render("ChoiceCharacter", pos[client.second.id], (int)m_ClientsCharacter[client.second.id], 0);
	}

#endif
}

void CLobbyScene::UIClientsReadyTextRender()
{
	wstring wstr = L"READY";

	UINT originX = 1200;
	UINT originY = 800;

	D2D1_RECT_F pos[6] =
	{
		D2D1::RectF((80 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY, (200 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((305 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY, (425 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((532 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY, (652 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((80 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY, (200 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((305 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY, (425 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY),
		D2D1::RectF((532 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY, (652 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY)
	};

#ifndef _WITH_SERVER_
	bool isReady = CCharacterSelectUIShader::GetIsReady();
	if (isReady == true)
	{
		pos = D2D1::RectF((80 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY, (200 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY);
		CDirect2D::GetInstance()->Render("피오피동글", "빨간색", wstr, pos);

		pos = D2D1::RectF((305 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY, (425 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY);
		CDirect2D::GetInstance()->Render("피오피동글", "빨간색", wstr, pos);

		pos = D2D1::RectF((532 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY, (652 * FRAME_BUFFER_WIDTH) / originX, (180 * FRAME_BUFFER_HEIGHT) / originY);
		CDirect2D::GetInstance()->Render("피오피동글", "빨간색", wstr, pos);

		pos = D2D1::RectF((80 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY, (200 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY);
		CDirect2D::GetInstance()->Render("피오피동글", "빨간색", wstr, pos);

		pos = D2D1::RectF((305 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY, (425 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY);
		CDirect2D::GetInstance()->Render("피오피동글", "빨간색", wstr, pos);

		pos = D2D1::RectF((532 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY, (652 * FRAME_BUFFER_WIDTH) / originX, (365 * FRAME_BUFFER_HEIGHT) / originY);
		CDirect2D::GetInstance()->Render("피오피동글", "빨간색", wstr, pos);
	}

#else
	for (auto client : CGameFramework::GetClientsInfo())
	{
		char id = client.second.id;
		if (id == CGameFramework::GetHostID())
		{
			CDirect2D::GetInstance()->Render("피오피동글", "황금색", L"▶방장◀", pos[id]);
			continue;
		}

		if (client.second.isReady == false)	continue;

		CDirect2D::GetInstance()->Render("피오피동글", "빨간색", wstr, pos[id]);
	}
#endif
}

void CLobbyScene::LobbySceneClear()
{
	for (auto& characters : m_ClientsCharacter)
		characters = -1;

	CCharacterSelectUIShader::LobbyUIClear();
}