#pragma once

// 공유 되는 텍스처들을 관리하기 위한 클래스
class CTexture;
class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Release();

	const map<string, CTexture*>& getTextureMap()	const { return m_TextureMap; }

private:
	map<string, CTexture*> m_TextureMap;
};