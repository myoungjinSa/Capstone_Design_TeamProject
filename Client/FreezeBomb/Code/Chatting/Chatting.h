#pragma once
#include "../Singleton/Singleton.h"
#include <deque>


class ChattingSystem : public Singleton<ChattingSystem>
{
protected:
	typedef basic_string<TCHAR> tstring;

#ifdef _WITH_DIRECT2D_
	IDWriteTextFormat*				m_pChattingFont{ nullptr };
	IDWriteTextLayout*				m_pChattingLayout{ nullptr };
	ID2D1SolidColorBrush*			m_pChattingFontColor{ nullptr };

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

	float							m_fontSize{0.0f};
	//LOBBY채팅에서의 최대 길이
	enum class SENTENCE_LENGTH_LOBBY		{ENG=20,KOR=12};
	
	//INGAME채팅에서의 최대 길이
	enum class SENTENCE_LENGTH_INGAME		{ENG=40,KOR=20};
	int m_maxChatSentenceCount = 12;
	deque<pair<const TCHAR*, const UINT32>> m_dequeText;
	float m_showTime{ 0.0f };

public:
	ChattingSystem();
	virtual ~ChattingSystem();
	void Initialize(IDWriteTextFormat*, IDWriteTextLayout*, ID2D1SolidColorBrush*, IWICImagingFactory*, ID2D1DeviceContext2*);
	void ProcessChatting(HWND hWnd,WPARAM wParam,LPARAM lParam,bool isIngame);
	void PushChattingText(const string& user,const char* chat);
	void ProcessSpecialCharacter(WPARAM wParam);
	//string -> TCHAR
	TCHAR* StringToTCHAR(string& s);
	string TCHARToString(const TCHAR* ptsz);

	void SetFontSize(const float& fontSize) { m_fontSize = fontSize; }
	void ResetShowTime(float t) { m_showTime = t; }

	//TCHAR* GetChatText() { return m_chat; };
	DWORD GetIMEMode() const { return m_conv; }
	void SetIMEMode(HWND hWnd, bool bHanMode);
	//한글 조립
	bool ComposeHangeul(HWND hWnd, WPARAM wParam,LPARAM lParam);

	void SetActive(bool active) { m_bActive = active; }
	bool IsChattingActive() { return m_bActive; }
	void ShowIngameChatting(ID2D1DeviceContext2* pd2dDeviceContext,float fTimeElapsed);
	void ShowLobbyChatting(ID2D1DeviceContext2* pd2dDeviceContext);
	void CreateChattingFont();
	void Destroy();
};
