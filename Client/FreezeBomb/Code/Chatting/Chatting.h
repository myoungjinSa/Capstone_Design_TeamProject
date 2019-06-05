#pragma once
#include "../Singleton/Singleton.h"

constexpr int						m_maxChatSentenceCount = 13;

#ifdef _WITH_SERVER_
class ChattingSystem : public Singleton<ChattingSystem>
{
protected:

	typedef basic_string<TCHAR> tstring;
#ifdef _WITH_DIRECT2D_

	IDWriteTextFormat				**m_pdwChattingFont{ nullptr };//채팅 폰트
	
	IDWriteTextLayout				*m_pdwChattingLayout{ nullptr };
	ID2D1SolidColorBrush			**m_pd2dbrChatText{ nullptr };//

	ID2D1Effect						*m_pd2dfxBitmapSource{ nullptr };
	IWICFormatConverter				*m_pwicFormatConverter{ nullptr };
#endif
	DWORD							m_conv;
	TCHAR**							m_chat;
	int								m_nCurrentText{ 0 };
	//string							m_sChat;
	wstring							m_wsChat;
	bool							m_bActive{ false };

	int								m_composeCount{ 0 };

	
	//LOBBY채팅에서의 최대 길이
	enum class SENTENCE_LENGTH_LOBBY		{ENG=45,KOR=15};
	
	//INGAME채팅에서의 최대 길이
	enum class SENTENCE_LENGTH_INGAME		{ENG=40,KOR=20};
	
	array<pair<TCHAR*,UINT32>,m_maxChatSentenceCount> m_arrText;
public:
	ChattingSystem();
	virtual ~ChattingSystem();
	void Initialize(IDWriteFactory*,ID2D1DeviceContext2*,IWICImagingFactory*);
	void ProcessChatting(HWND hWnd,WPARAM wParam,LPARAM lParam,bool isIngame);
	void PushChattingText(char* chat);
	
	//string -> TCHAR
	TCHAR* StringToTCHAR(string& s);
	string TCHARToString(const TCHAR* ptsz);


	//TCHAR* GetChatText() { return m_chat; };
	DWORD GetIMEMode() const { return m_conv; }
	void SetIMEMode(HWND hWnd, bool bHanMode);
	//한글 조립
	bool ComposeHangeul(HWND hWnd, WPARAM wParam,LPARAM lParam);

	void SetActive(bool active) { m_bActive = active; }
	bool IsChattingActive() { return m_bActive; }
	void ShowIngameChatting(ID2D1DeviceContext2* pd2dDeviceContext);
	void ShowLobbyChatting(ID2D1DeviceContext2* pd2dDeviceContext);

	void Destroy();
};
#endif