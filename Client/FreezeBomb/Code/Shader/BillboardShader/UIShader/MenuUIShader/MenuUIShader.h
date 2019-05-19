#pragma once

#include "../UIShader.h"

class CTexture;
class CUI;
class CSoundSystem;
class CMenuUIShader : public CUIShader
{
public:
	CMenuUIShader();
	virtual ~CMenuUIShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int UIType);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CTexture*>& Context, void* pContext);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);
	virtual void ReleaseObjects();

	bool getIsRender()	const { return m_IsRender; }
	void setIsRender(bool value) { m_IsRender = value; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	bool UICollisionCheck(XMFLOAT2& mousePos, XMFLOAT2& minUIPos, XMFLOAT2& maxUIPos);

	enum MESSAGE_TPYE { MOUSE, KEYBOARD };
	void ProcessMessage(bool isMouse, UINT message, XMFLOAT2& mousePos);
	void ProcessMouseMessage(UINT message, XMFLOAT2& mousePos);
	void ProcessKeyBoardMessage();

private:
	enum MENU_TYPE { None = - 1, MenuBoard, Menu_ICON, Menu_Option, Menu_GameOver };
	bool m_IsRender = false;

	struct MenuUIInfo
	{
		XMFLOAT4 m_MenuBoard_MinMaxPos;
		XMFLOAT4 m_MenuICON_MinMaxPos;
		XMFLOAT4 m_MenuBoard_MenuICON_UV;

		XMFLOAT4 m_MenuOption_MinMaxPos;
		XMFLOAT4 m_MenuGameOver_MinMaxPos;
		XMFLOAT4 m_MenuOption_MenuGameOver_UV;
	};

	struct CB_UI
	{
		XMFLOAT4 m_MenuBoard_MinMaxPos;
		XMFLOAT4 m_MenuICON_MinMaxPos;
		XMFLOAT4 m_MenuBoard_MenuICON_UV;

		XMFLOAT4 m_MenuOption_MinMaxPos;
		XMFLOAT4 m_MenuGameOver_MinMaxPos;
		XMFLOAT4 m_MenuOption_MenuGameOver_UV;
	};

	MenuUIInfo m_MenuInfo;

	ID3D12Resource*		m_pd3dUIData{ nullptr };
	CB_UI*						m_pMappedUIData{ nullptr };

	enum MUSIC_TYPE { MENU_INPUT};
	static bool				m_IsPlay;
	CSoundSystem*		m_pSound{ nullptr };

};