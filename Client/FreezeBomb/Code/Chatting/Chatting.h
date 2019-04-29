#pragma once
#include "../Singleton/Singleton.h"

#pragma comment(lib,"imm32.lib")
#include <imm.h>

template<class T> T* Singleton<T>::_instance = nullptr;


class ChattingSystem : public Singleton<ChattingSystem>
{
protected:
#ifdef _WITH_DIRECT2D_

	IDWriteTextFormat				**m_pdwChattingFont{ nullptr };//채팅 폰트
	const int						m_maxChatSentenceCount = 10;
	IDWriteTextLayout				*m_pdwChattingLayout{ nullptr };
	ID2D1SolidColorBrush			**m_pd2dbrChatText{ nullptr };//
#endif
	DWORD							m_conv;
	TCHAR							m_chat[512];
	string							m_sChat;
public:
	ChattingSystem();
	virtual ~ChattingSystem();
	void Initialize(IDWriteFactory*,ID2D1DeviceContext2*);
	void ProcessChatting(UCHAR* key);


	TCHAR* GetChatText() { return m_chat; };
	DWORD GetIMEMode() const { return m_conv; }
	void SetIMEMode(HWND hWnd, bool bHanMode);
	
	void Destroy();
};