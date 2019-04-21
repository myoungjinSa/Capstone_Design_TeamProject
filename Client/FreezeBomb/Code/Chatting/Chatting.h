#pragma once
#include "../Singleton/Singleton.h"

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

	TCHAR							m_chat[512];
public:
	ChattingSystem();
	virtual ~ChattingSystem();
	void Initialize(IDWriteFactory*,ID2D1DeviceContext2*);
	void ProcessInput(UCHAR* key);

	void Destroy();
};