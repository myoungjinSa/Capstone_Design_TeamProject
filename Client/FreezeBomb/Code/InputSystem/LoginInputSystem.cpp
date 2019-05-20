#include "../Stdafx/Stdafx.h"
#include "LoginInputSystem.h"

#pragma comment(lib,"imm32.lib")
#include <imm.h>


#ifdef _WITH_DIRECT2D_


CLoginInputSystem::CLoginInputSystem()
{

}

CLoginInputSystem::~CLoginInputSystem()
{

}

void CLoginInputSystem::Initialize(IDWriteFactory *writeFactory, ID2D1DeviceContext2* pd2dDeviceContext,IWICImagingFactory* pwicImagingFactory)
{
	HRESULT hResult;
	m_pdwChattingFont = new IDWriteTextFormat*[m_maxChatSentenceCount];
	m_pd2dbrChatText = new ID2D1SolidColorBrush*[m_maxChatSentenceCount];
	m_pd2dDeviceContext = pd2dDeviceContext;
	for (int i = 0; i < m_maxChatSentenceCount; ++i)
	{
		hResult = writeFactory->CreateTextFormat(L"°íµñ", nullptr, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30.0f, L"en-US", &m_pdwChattingFont[i]);
		hResult = m_pdwChattingFont[i]->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_LEADING);
		hResult = m_pdwChattingFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		hResult = writeFactory->CreateTextLayout(L"ÅØ½ºÆ® ·¹ÀÌ¾Æ¿ô", 64, m_pdwChattingFont[i], 4096.0f, 4096.0f, &m_pdwChattingLayout);
		//pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pd2dbrChatText[i]);
	}
	pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &m_pd2dbrChatText[0]);

}


void CLoginInputSystem::ProcessIDInput(UINT sel)
{
	//static UCHAR pKeyBuffer[256];

	if (GetAsyncKeyState(VK_A) & 0x0001)
		if (sel == 0)
		{
			m_wsID += (TCHAR)'a';
			//cout << m_wsID.c_str() << endl;
		}
		else if(sel == 1)
		{
			m_wsIP += (TCHAR)'a';
			//cout << "IP" << endl;
		}
	//if (GetAsyncKeyState(VK_A) & 0x8000)
	//cout << "a" << endl;
	//m_wsID += (TCHAR)'a';

}

void CLoginInputSystem::ProcessIPInput()
{
	static UCHAR pKeyBuffer[256];

	if (pKeyBuffer['1'] & 0xF0)
		m_wsID += (TCHAR)'1';
}
TCHAR* CLoginInputSystem::StringToTCHAR(string& s)
{
	tstring tstr;
	const char* all = s.c_str();
	size_t len = strlen(all) + 1;

	wchar_t* t = new wchar_t[len];

	if (t == nullptr)
		throw bad_alloc();

	mbstowcs(t, all, len);

	return (TCHAR*)t;
	
}

void CLoginInputSystem::ShowIDInput()
{
	//if(m_bIDShow)
	//{
		D2D1_RECT_F idText{ 0,0,0,0 };

		const TCHAR* t;
		t = m_wsID.c_str();

		idText = D2D1::RectF(680.0f, 645.0f, 500.0f, 300.0f);
		m_pd2dDeviceContext->DrawTextW(t, (UINT32)wcslen(t), m_pdwChattingFont[0], &idText, m_pd2dbrChatText[0]);
	//}
}

void CLoginInputSystem::ShowIPInput()
{
	//if(m_bIPShow)
	//{
		D2D1_RECT_F ipText{ 0,0,0,0 };

		const TCHAR* t;
		t = m_wsIP.c_str();

		ipText = D2D1::RectF(680.0f, 645.0f, 500.0f, 300.0f);
		m_pd2dDeviceContext->DrawTextW(t, (UINT32)wcslen(t), m_pdwChattingFont[0], &ipText, m_pd2dbrChatText[0]);
	//}
}
string CLoginInputSystem::TCHARToString(const TCHAR* ptsz)
{
	size_t len = (int)wcslen((wchar_t*)ptsz);
	const size_t charlen = 2 * len + 1;

	char* psz = new char[charlen];
	wcstombs(psz, (wchar_t*)ptsz, charlen);

	string s = psz;
	delete[] psz;
	return s;
}


void CLoginInputSystem::Destroy()
{
	m_wsIP.clear();
	m_wsIP.shrink_to_fit();

	m_wsID.clear();
	m_wsID.shrink_to_fit();
	if (m_pd2dDeviceContext)
		m_pd2dDeviceContext = nullptr;
	for (int i = 0; i < m_maxChatSentenceCount; ++i)
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