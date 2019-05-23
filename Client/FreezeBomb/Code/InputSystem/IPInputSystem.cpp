#include "../Stdafx/Stdafx.h"
#include "IPInputSystem.h"
#include "../Shader/BillboardShader/UIShader/LoginShader/IPShader.h"
#include "../Network/Network.h"
#include <mutex>
#pragma comment(lib,"imm32.lib")
#include <imm.h>

#ifdef _WITH_SERVER_
#ifdef _WITH_DIRECT2D_
extern  const char* g_serverIP;
mutex g_lock;
CIPInputSystem::CIPInputSystem()
{

}

CIPInputSystem::~CIPInputSystem()
{

}

void CIPInputSystem::Initialize(IDWriteFactory *writeFactory, ID2D1DeviceContext2* pd2dDeviceContext,IWICImagingFactory* pwicImagingFactory)
{
	HRESULT hResult;
	m_pdwChattingFont = new IDWriteTextFormat*[m_maxChatSentenceCount];
	m_pd2dbrChatText = new ID2D1SolidColorBrush*[m_maxChatSentenceCount];
	m_pd2dDeviceContext = pd2dDeviceContext;
	for (int i = 0; i < m_maxChatSentenceCount; ++i)
	{
		hResult = writeFactory->CreateTextFormat(L"°íµñ", nullptr, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30.0f, L"en-US", &m_pdwChattingFont[i]);
		hResult = m_pdwChattingFont[i]->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_LEADING);
		hResult = m_pdwChattingFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		hResult = writeFactory->CreateTextLayout(L"ÅØ½ºÆ® ·¹ÀÌ¾Æ¿ô", 64, m_pdwChattingFont[i], 4096.0f, 4096.0f, &m_pdwChattingLayout);
		//pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pd2dbrChatText[i]);
	}
	pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &m_pd2dbrChatText[0]);

}


size_t CIPInputSystem::ProcessIPInput(UINT sel)
{
	//static UCHAR pKeyBuffer[256];

	
	//	if(sel == CLoginShader::LoginState::IP_SELECT)
	//	{

	
	
		if (m_wsIP.size() < IP_LENGTH)
		{
			if (GetAsyncKeyState(DIGIT_0) & 0x0001 || GetAsyncKeyState(VK_NUMPAD0) & 0x0001)
				m_wsIP += '0';
			if (GetAsyncKeyState(DIGIT_1) & 0x0001 || GetAsyncKeyState(VK_NUMPAD1) & 0x0001)
				m_wsIP += '1';
			if (GetAsyncKeyState(DIGIT_2) & 0x0001 || GetAsyncKeyState(VK_NUMPAD2) & 0x0001)
				m_wsIP += '2';
			if (GetAsyncKeyState(DIGIT_3) & 0x0001 || GetAsyncKeyState(VK_NUMPAD3) & 0x0001)
				m_wsIP += '3';
			if (GetAsyncKeyState(DIGIT_4) & 0x0001 || GetAsyncKeyState(VK_NUMPAD4) & 0x0001)
				m_wsIP += '4';
			if (GetAsyncKeyState(DIGIT_5) & 0x0001 || GetAsyncKeyState(VK_NUMPAD5) & 0x0001)
				m_wsIP += '5';
			if (GetAsyncKeyState(DIGIT_6) & 0x0001 || GetAsyncKeyState(VK_NUMPAD6) & 0x0001)
				m_wsIP += '6';
			if (GetAsyncKeyState(DIGIT_7) & 0x0001 || GetAsyncKeyState(VK_NUMPAD7) & 0x0001)
				m_wsIP += '7';
			if (GetAsyncKeyState(DIGIT_8) & 0x0001 || GetAsyncKeyState(VK_NUMPAD8) & 0x0001)
				m_wsIP += '8';
			if (GetAsyncKeyState(DIGIT_9) & 0x0001 || GetAsyncKeyState(VK_NUMPAD9) & 0x0001)
				m_wsIP += '9';
			if (GetAsyncKeyState(VK_OEM_PERIOD) & 0x0001)
				m_wsIP += '.';
		}

		if (GetAsyncKeyState(VK_BACK) & 0x0001)
		{
			if (m_wsIP.size() > 0)
			{
				m_wsIP.pop_back();

			}
		}
		if(GetAsyncKeyState(VK_RETURN) & 0x0001)
		{
			if(m_wsIP.size() > 0 )
			{
				g_lock.lock();
				g_serverIP = m_wsIP.c_str();
				g_lock.unlock();
			}
		}
	
	
	//	}

	//if (GetAsyncKeyState(VK_A) & 0x8000)
	//cout << "a" << endl;
	//m_wsID += (TCHAR)'a';
	return m_wsIP.size();
}


TCHAR* CIPInputSystem::StringToTCHAR(string& s)
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



void CIPInputSystem::ShowIPInput()
{

	D2D1_RECT_F ipText{ 0,0,0,0 };

	const TCHAR* t;
		
	t = StringToTCHAR(m_wsIP);;

	ipText = D2D1::RectF(480.0f, 670.0f, 750.0f, 670.0f);
	m_pd2dDeviceContext->DrawTextW(t, (UINT32)wcslen(t), m_pdwChattingFont[0], &ipText, m_pd2dbrChatText[0]);

}
string CIPInputSystem::TCHARToString(const TCHAR* ptsz)
{
	size_t len = (int)wcslen((wchar_t*)ptsz);
	const size_t charlen = 2 * len + 1;

	char* psz = new char[charlen];
	wcstombs(psz, (wchar_t*)ptsz, charlen);

	string s = psz;
	delete[] psz;
	return s;
}


void CIPInputSystem::Destroy()
{
	m_wsIP.clear();
	m_wsIP.shrink_to_fit();

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
#endif