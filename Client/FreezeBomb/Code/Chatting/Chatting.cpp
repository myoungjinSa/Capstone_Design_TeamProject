#include "../Stdafx/Stdafx.h"
#include <string>
#include "Chatting.h"

#ifdef _WITH_DIRECT2D_

ChattingSystem::ChattingSystem()
{
}

ChattingSystem::~ChattingSystem()
{
	cout << "Chattinng System 소멸" << endl;

}
void ChattingSystem::Initialize(IDWriteFactory* writeFactory,ID2D1DeviceContext2* pd2dDeviceContext)
{
	HRESULT hResult;
	m_pdwChattingFont = new IDWriteTextFormat*[m_maxChatSentenceCount];
	m_pd2dbrChatText = new ID2D1SolidColorBrush*[m_maxChatSentenceCount];

	for (int i = 0; i < m_maxChatSentenceCount; ++i)
	{
		hResult = writeFactory->CreateTextFormat(L"고딕", nullptr, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30.0f, L"en-US", &m_pdwChattingFont[i]);
		hResult = m_pdwChattingFont[i]->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_LEADING);
		hResult = m_pdwChattingFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		hResult = writeFactory->CreateTextLayout(L"텍스트 레이아웃", 64, m_pdwChattingFont[i], 4096.0f, 4096.0f, &m_pdwChattingLayout);
		pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &m_pd2dbrChatText[i]);
	
	}
}

void ChattingSystem::ProcessChatting(UCHAR* key)
{
	TCHAR		localChat[512];
	static bool bCapital = false;	// true : 대문자 , false  = 소문자
	
	//n_tcscpy_s()
	//현재 영문이 활성화 됐을 경우
	if (GetIMEMode() == IME_CMODE_ALPHANUMERIC)
	{
		if (GetKeyState(VK_CAPITAL) & 0x8001 && bCapital == true)
		{
			if (GetKeyState(VK_C) & 0x8000)
			{
				m_sChat += 'C';
			}
		}
		else
		{
			if(GetKeyState(VK_C) & 0x8000)
			{
				m_sChat += 'c';
			}
		}
		

	}
	else if(GetIMEMode() == IME_CMODE_NATIVE)
	{
		//현재 한글이 활성화 됐을 경우
		if (key[VK_C] & 0xF0)
		{

		}
	}

	
}

void ChattingSystem::Destroy()
{
	for (UINT i = 0; i < m_maxChatSentenceCount; ++i)
	{
		if (m_pdwChattingFont[i]) m_pdwChattingFont[i]->Release();
		if (m_pd2dbrChatText[i]) m_pd2dbrChatText[i]->Release();
	}
	delete[]m_pdwChattingFont;
	m_pdwChattingFont = nullptr;
	delete[]m_pd2dbrChatText;
	m_pd2dbrChatText = nullptr;
}
#endif

//프로그램 내에서 한영 전환
void ChattingSystem::SetIMEMode(HWND hWnd,bool bHanMode)
{
	//해당 윈도우의 Input Context 를 가져오는 함수.
	//리턴값으로 Input Context의 핸들을 반환한다.
	HIMC hIMC = ImmGetContext(hWnd);
	DWORD dwSent;
	DWORD dwTemp;

	//hIMC => 정보를 알고싶은 input context의 핸들
	//lpfwdConversion => IME Conversion Mode 값
	//lpfwdSentence => Sentence mode 값
	ImmGetConversionStatus(hIMC, &m_conv, &dwSent);

	dwTemp = m_conv & ~IME_CMODE_LANGUAGE;

	//상태를 바꾼다
	if (bHanMode)
		dwTemp |= IME_CMODE_NATIVE;	//한글
	else
		dwTemp |= IME_CMODE_ALPHANUMERIC;	// 영문

	m_conv = dwTemp;

	//hIMC - input context 핸들, NULL이면 현재 active 상태인 context를 설정

	ImmSetConversionStatus(hIMC, m_conv, dwSent);
	
	//Input Context를 릴리즈 하고, Context에 할당된 메모리를 unlock 한다.
	//ImmGetContext()를 사용했다면, 반드시 ImmReleaseContext()를 사용해주어야 한다.
	ImmReleaseContext(hWnd, hIMC);

	
}

