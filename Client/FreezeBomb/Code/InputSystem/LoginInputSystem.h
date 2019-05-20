#pragma once




class CLoginInputSystem 
{
protected:
	typedef basic_string<TCHAR> tstring;
#ifdef _WITH_DIRECT2D_
	IDWriteTextFormat				**m_pdwChattingFont{ nullptr };//채팅 폰트
	const int						m_maxChatSentenceCount = 1;
	IDWriteTextLayout				*m_pdwChattingLayout{ nullptr };
	ID2D1SolidColorBrush			**m_pd2dbrChatText{ nullptr };//

	ID2D1DeviceContext2*			m_pd2dDeviceContext{ nullptr };
#endif
	DWORD							m_conv;
	TCHAR							m_chat[512];
	//string							m_sChat;
	wstring							m_wsID;
	wstring							m_wsIP;
	bool							m_bIDShow{ false };
	bool							m_bIPShow{ false };
	int								m_composeCount{ 0 };

	//LOBBY채팅에서의 최대 길이
	enum class SENTENCE_LENGTH_LOBBY		{ENG=45,KOR=15};
	
	//INGAME채팅에서의 최대 길이
	enum class SENTENCE_LENGTH_INGAME		{ENG=40,KOR=20};

public:
	CLoginInputSystem();
	virtual ~CLoginInputSystem();

	void Initialize(IDWriteFactory*,ID2D1DeviceContext2*,IWICImagingFactory*);

	void ProcessIDInput(UINT sel);
	void ProcessIPInput();

	TCHAR* StringToTCHAR(string& s);
	string TCHARToString(const TCHAR* ptsz);
	TCHAR* GetChatText() { return m_chat; };
	DWORD GetIMEMode() const { return m_conv; }
	void SetIMEMode(HWND hWnd, bool bHanMode);
	//한글 조립
	bool ComposeHangeul(HWND hWnd, WPARAM wParam,LPARAM lParam);

	//void SetActive(bool active) { m_bActive = active; }
	void SetIDActive(bool active) { m_bIDShow = active;  }
	void SetIPActive(bool active) { m_bIPShow = active;  }
	bool IsIDInputActive() { return m_bIDShow; }
	bool IsIPInputActive() { return m_bIPShow; }
	void ShowIDInput();
	void ShowIPInput();
	void Destroy();
};