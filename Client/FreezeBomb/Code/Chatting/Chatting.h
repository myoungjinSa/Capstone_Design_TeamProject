#pragma once
#include "../Singleton/Singleton.h"

#pragma comment(lib,"imm32.lib")
#include <imm.h>

template<class T> T* Singleton<T>::_instance = nullptr;


class ChattingSystem : public Singleton<ChattingSystem>
{
protected:

	typedef basic_string<TCHAR> tstring;
#ifdef _WITH_DIRECT2D_

	IDWriteTextFormat				**m_pdwChattingFont{ nullptr };//채팅 폰트
	const int						m_maxChatSentenceCount = 10;
	IDWriteTextLayout				*m_pdwChattingLayout{ nullptr };
	ID2D1SolidColorBrush			**m_pd2dbrChatText{ nullptr };//

	ID2D1Effect						*m_pd2dfxBitmapSource{ nullptr };
	IWICFormatConverter				*m_pwicFormatConverter{ nullptr };
#endif
	DWORD							m_conv;
	TCHAR							m_chat[512];
	//string							m_sChat;
	wstring							m_wsChat;
	bool							m_bActive{ false };
public:
	ChattingSystem();
	virtual ~ChattingSystem();
	void Initialize(IDWriteFactory*,ID2D1DeviceContext2*,IWICImagingFactory*);
	void ProcessChatting(HWND hWnd,WPARAM wParam,LPARAM lParam,bool bHangeul);

	//string -> TCHAR
	TCHAR* StringToTCHAR(string& s);
	string TCHARToString(const TCHAR* ptsz);


	TCHAR* GetChatText() { return m_chat; };
	DWORD GetIMEMode() const { return m_conv; }
	void SetIMEMode(HWND hWnd, bool bHanMode);
	//한글 조립
	bool ComposeHangeul(HWND hWnd, WPARAM wParam,LPARAM lParam);

	void SetActive(bool active) { m_bActive = active; }
	bool IsChattingActive() { return m_bActive; }
	void ShowChatting(ID2D1DeviceContext2* pd2dDeviceContext);

	void Destroy();
};