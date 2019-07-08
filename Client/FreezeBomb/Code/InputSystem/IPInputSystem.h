#pragma once

#ifdef _WITH_SERVER_
class CIPInputSystem 
{
protected:
	typedef basic_string<TCHAR> tstring;

#ifdef _WITH_DIRECT2D_
	IDWriteTextFormat*				m_pFont{ nullptr };
	IDWriteTextLayout*				m_pTextLayout{ nullptr };
	ID2D1SolidColorBrush*			m_pFontColor{ nullptr };
	ID2D1DeviceContext2*			m_pd2dDeviceContext{ nullptr };
#endif

	DWORD							m_conv;
	TCHAR							m_chat[512];
	//string							m_sChat;
	string							m_wsIP;
	
	bool							m_bIPShow{ false };
	int								m_composeCount{ 0 };

	const int IP_LENGTH = 15;

public:
	CIPInputSystem();
	virtual ~CIPInputSystem();

	void Initialize(IDWriteTextFormat*, IDWriteTextLayout*, ID2D1SolidColorBrush*, ID2D1DeviceContext2*);

	size_t ProcessIPInput(UINT sel);

	TCHAR* StringToTCHAR(string& s);
	string TCHARToString(const TCHAR* ptsz);
	TCHAR* GetChatText() { return m_chat; };
	DWORD GetIMEMode() const { return m_conv; }
	void SetIMEMode(HWND hWnd, bool bHanMode);
	//한글 조립
	bool ComposeHangeul(HWND hWnd, WPARAM wParam,LPARAM lParam);

	//void SetActive(bool active) { m_bActive = active; }
	void SetIPActive(bool active) { m_bIPShow = active;  }
	bool IsIPInputActive() { return m_bIPShow; }
	void ShowIPInput();
	void Destroy();
};
#endif 