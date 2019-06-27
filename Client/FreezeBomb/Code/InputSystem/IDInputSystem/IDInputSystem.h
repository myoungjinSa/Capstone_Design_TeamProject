#pragma once

#ifdef _WITH_SERVER_
#ifdef _WITH_DIRECT2D_
class CIDInput 
{
protected:
	typedef basic_string<TCHAR> tstring;
	IDWriteTextFormat				**m_pdwFont{ nullptr };//채팅 폰트
	const int						m_maxFont = 1;
	IDWriteTextLayout				*m_pdwFontLayout{ nullptr };
	ID2D1SolidColorBrush			**m_pd2dbrFontText{ nullptr };//

	

	DWORD							m_conv;

	
	//string							m_sChat;
	wstring							m_wsID;
	
	bool							m_bIDShow{ false };
	
	int								m_composeCount{ 0 };


	enum LENGTH{ENG = 15,KOR = 8};

public:
	CIDInput();
	virtual ~CIDInput();

	void Initialize(IDWriteFactory*,ID2D1DeviceContext2*);

	size_t IsIDLength() { return m_wsID.size(); }
	void ProcessIDInput(HWND hWnd,WPARAM wParam,LPARAM lParam);
	TCHAR* StringToTCHAR(string& s);
	string TCHARToString(const TCHAR* ptsz);
	
	TCHAR* GetPlayerName();

	DWORD GetIMEMode() const { return m_conv; }
	void SetIMEMode(HWND hWnd, bool bHanMode);
	//한글 조립
	bool ComposeHangeul(HWND hWnd, WPARAM wParam,LPARAM lParam);


	void SetIDActive(bool active) { m_bIDShow = active;  }
	bool IsIDInputActive() { return m_bIDShow; }

	void ShowIDInput(ID2D1DeviceContext2* pd2dDeviceContext);
	void Destroy();
};
#endif
#endif