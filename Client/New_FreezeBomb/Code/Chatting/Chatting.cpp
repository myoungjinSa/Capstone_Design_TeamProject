#include "../Stdafx/Stdafx.h"
#include <algorithm>
#include "../Network/Network.h"
#include "Chatting.h"
#include "../Direct2D/Direct2D.h"

#pragma comment(lib,"imm32.lib")
#include <imm.h>

#define NUMBER DIGIT_0 | DIGIT_1 | DIGIT_2 | DIGIT_3 | DIGIT_4
#ifdef _WITH_DIRECT2D_
ChattingSystem::ChattingSystem()
{
}

ChattingSystem::~ChattingSystem()
{
}

void ChattingSystem::Initialize()
{
	m_fontSize = CDirect2D::GetInstance()->GetFontInfo("메이플").m_FontSize;
	CreateChattingFont();
}

void ChattingSystem::CreateChattingFont()
{
	if(m_fontSize < 30)
	{
		m_maxChatSentenceCount = 10;
	}
	else if( m_fontSize < 40)
	{
		m_maxChatSentenceCount = 8;
	}
	else if(m_fontSize < 50)
	{
		m_maxChatSentenceCount = 6;
	}
	else
	{
		m_maxChatSentenceCount = 4;
	}
	m_chat = new TCHAR*[m_maxChatSentenceCount];
	for (int i = 0; i < m_maxChatSentenceCount; ++i)
		m_chat[i] = new TCHAR[256];
}

TCHAR* ChattingSystem::StringToTCHAR(string& s)
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

string ChattingSystem::TCHARToString(const TCHAR* ptsz)
{
	size_t len = (int)wcslen((wchar_t*)ptsz);
	const size_t charlen = 2 * len + 1;

	char* psz = new char[charlen];
	wcstombs(psz, (wchar_t*)ptsz, charlen);

	string s = psz;
	delete[] psz;
	return s;
}

void ChattingSystem::ShowLobbyChatting()
{
	D2D1_RECT_F pos{ 0,0,0,0 };

	UINT originX = 1280;
	UINT originY = 720;

	pos.left = (24.0f * FRAME_BUFFER_WIDTH) / originX;
	pos.top = (665.0f * FRAME_BUFFER_HEIGHT) / originY;
	pos.right = (760.0f * FRAME_BUFFER_WIDTH) / originX;
	pos.bottom = (687.0f * FRAME_BUFFER_HEIGHT) / originY;

	int boxStart = (630 * FRAME_BUFFER_HEIGHT) / originY;

	int diff = (m_fontSize * FRAME_BUFFER_HEIGHT) / originY;

	if (IsChattingActive())
		CDirect2D::GetInstance()->Render("메이플", "검은색", m_wsChat, pos);

	for (int i = 1; i < m_dequeText.size()+1; ++i) 
	{
		pos = D2D1::RectF(pos.left, boxStart - (i * diff), pos.right, boxStart- (i * diff));
		CDirect2D::GetInstance()->Render("메이플", "검은색", m_dequeText[i - 1].first, pos);
	}
}

void ChattingSystem::ShowIngameChatting(float fTimeElapsed)
{
	if (m_bActive == true)
	{		
		CDirect2D::GetInstance()->Render("ChatInput");

		UINT originX = 1280;
		UINT originY = 720;
		D2D1_RECT_F pos{ 0, 0, 0, 0 };
		pos.left = (20.0f * FRAME_BUFFER_WIDTH) / originX;
		pos.top = (680.0f * FRAME_BUFFER_HEIGHT) / originY;
		pos.right = (720.0f * FRAME_BUFFER_WIDTH) / originX;
		pos.bottom = (680.0f * FRAME_BUFFER_HEIGHT) / originY;

		CDirect2D::GetInstance()->Render("메이플", "흰색", m_wsChat, pos);

		m_showTime = 0.0f;
	}
	else
	{
		m_showTime += fTimeElapsed;
	}
	if (m_showTime <= 5.0f) 
	{
		D2D1_RECT_F pos{ 0,0,0,0 };
		UINT originX = 1280;
		UINT originY = 720;

		pos.left = (20.0f * FRAME_BUFFER_WIDTH) / originX;
		pos.top = (655.0f * FRAME_BUFFER_HEIGHT) / originY;
		pos.right = (720.0f * FRAME_BUFFER_WIDTH) / originX;
		pos.bottom = (655.0f * FRAME_BUFFER_HEIGHT) / originY;

		int diff = (m_fontSize * FRAME_BUFFER_HEIGHT) / originY;

		for (int i = 1; i < m_dequeText.size() + 1; ++i)
		{
			pos = D2D1::RectF(pos.left, pos.top - (i + diff), pos.right, pos.bottom - (i + diff));
			CDirect2D::GetInstance()->Render("메이플", "검은색", m_dequeText[i - 1].first, pos);
		}
	}
}

void ChattingSystem::ProcessSpecialCharacter(WPARAM wParam)
{
	//n_tcscpy_s()
	if(DIGIT_0 <= wParam && wParam <= DIGIT_9)
	{
		if (m_wsChat.size() <= (size_t)18)
		{
			m_wsChat += (TCHAR)wParam;
		}		
	}

	switch(wParam)
	{
	case '?':
	{
		
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"?");
		}
		break;
	}
	case '!':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"!");
		}
		break;
	}
	case '@':
	{
		if(m_wsChat.size() <= (size_t)14)
		{
			m_wsChat.append(L"@");
		}
		break;
	}
	case '_':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"_");
		}
		break;
	}
	case '-':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"-");
		}
		break;
	}
	case '%':
	{
		if(m_wsChat.size() <= (size_t)14)
		{
			m_wsChat.append(L"%");
		}
		break;
	}
	case '^':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"^");
		}
		break;
	}
	case '(':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"(");
		}
		break;
	}
	case ')':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L")");
		}
		break;
	}
	case '$':
	{
		if(m_wsChat.size() <= (size_t)14)
		{
			m_wsChat.append(L"$");
		}
		break;
	}
	case '#':
	{
		if(m_wsChat.size() <= (size_t)14)
		{
			m_wsChat.append(L"#");
		}
		break;
	}
	case '*':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"*");
		}
		break;
	}
	case '+':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"+");
		}
		break;
	}
	case '.':
	{
		if(m_wsChat.size() <= (size_t)20)
		{
			m_wsChat.append(L".");
		}
		break;
	}
	case ',':
	{
		if(m_wsChat.size() <= (size_t)20)
		{
			m_wsChat.append(L",");
		}
		break;
	}
	case '&':
	{
		if(m_wsChat.size() <= (size_t)14)
		{
			m_wsChat.append(L"&");
		}
		break;
	}
	case '~':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat.append(L"~");
		}
		break;
	}
	case '"':
	{
		if(m_wsChat.size() <= (size_t)15)
		{
			m_wsChat += L'"';
		}
		break;
	}
	case ';':
	{
		if(m_wsChat.size() <= (size_t)20)
		{
			m_wsChat.append(L";");
		}
		break;
	}
	case ':':
	{
		if(m_wsChat.size() <= (size_t)20)
		{
			m_wsChat.append(L":");
		}
		break;
	}
	default:
		break;
	}

}

void ChattingSystem::ProcessChatting(HWND hWnd, WPARAM wParam, LPARAM lParam, bool isInGame)
{
	if(wParam == VK_RETURN)
	{
		if (m_wsChat.size() > 0)
		{
#ifdef _WITH_SERVER_
			TCHAR* t;
			t = const_cast<TCHAR*>(m_wsChat.c_str());

			Network::GetInstance()->SendChattingText((char)Network::GetInstance()->GetMyID(), t);
#endif			
			m_wsChat.clear();
			m_wsChat.shrink_to_fit();
		}
	}
	if(wParam == VK_SPACE)
	{
		if(m_wsChat.size() <= (size_t)24)
		{
			m_wsChat.append(L" ");
		}
	}

	//현재 영문이 활성화 됐을 경우
	if (GetIMEMode() == IME_CMODE_ALPHANUMERIC)
	{
		if (wParam != VK_RETURN && wParam != VK_BACK)
		{
			if (isInGame) {
				if (m_wsChat.size() <= (size_t)SENTENCE_LENGTH_INGAME::ENG)
				{
					m_wsChat += (TCHAR)wParam;
				}
			}
			else
			{
				if (m_wsChat.size() <= (size_t)SENTENCE_LENGTH_LOBBY::ENG)
				{
					//m_wsChat += (TCHAR)wParam;
					m_wsChat += (TCHAR)wParam;
				}
			}

		}
	}
	else if (GetIMEMode() == IME_CMODE_NATIVE)
	{
		//한글 입력에서는 특수문자 처리를 이 함수에서 해줘야한다.
		ProcessSpecialCharacter(wParam);

		if (wParam != VK_RETURN)
		{
			if (isInGame)
			{
				if (m_wsChat.size() <= (size_t)SENTENCE_LENGTH_INGAME::KOR)
				{
					//m_wsChat += (TCHAR)wParam;
					ComposeHangeul(hWnd, wParam, lParam);
				}
			}
			else
			{
				if (m_wsChat.size() <= (size_t)SENTENCE_LENGTH_LOBBY::KOR)
				{
					//m_wsChat += (TCHAR)wParam;
					ComposeHangeul(hWnd, wParam, lParam);
				}
			}

		}

	}
	if (GetKeyState(VK_BACK) & 0x8000)
	{
		if (m_wsChat.size() > 0)
		{
			m_wsChat.pop_back();
		}
		if (GetIMEMode() == IME_CMODE_NATIVE)
		{
			SetIMEMode(hWnd, IME_CMODE_ALPHANUMERIC);
			SetIMEMode(hWnd, IME_CMODE_NATIVE);
		}
		m_composeCount = 0;
		//return;
	}
}

void ChattingSystem::Destroy()
{
	for (int i = 0; i < m_maxChatSentenceCount; ++i)
		if (m_chat[i])
			delete m_chat[i];
		delete[] m_chat;

	m_dequeText.clear();
	m_wsChat.clear();
	m_wsChat.shrink_to_fit();
}

void ChattingSystem::ChattingActive()
{
	if (KEY_DOWN(VK_RETURN))
	{
		if (m_IsKeyDown == false)
			m_IsKeyDown = true;
	}

	if (KEY_UP(VK_RETURN))
	{
		if (m_IsKeyDown == true)
		{
			m_bActive ? m_bActive = false : m_bActive = true;
			m_IsKeyDown = false;
		}
	}
}

#endif
void ChattingSystem::PushText(const string& str)
{
	ZeroMemory(m_chat[m_nCurrentText], sizeof(256));
	int chattingLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), m_chat[m_nCurrentText], chattingLen);
	m_chat[m_nCurrentText][chattingLen] = '\0';

	if(m_dequeText.size() >= m_maxChatSentenceCount - 1)
	{
		//가장 위에 위치한 텍스트를 pop한다.
		m_dequeText.pop_back();
		m_dequeText.emplace_front(make_pair(m_chat[m_nCurrentText],(UINT32)chattingLen));
		m_nCurrentText = (++m_nCurrentText) % m_maxChatSentenceCount;
	}
	else
	{
		m_dequeText.emplace_front(make_pair(m_chat[m_nCurrentText], (UINT32)chattingLen));
		m_nCurrentText = (++m_nCurrentText) % m_maxChatSentenceCount;
	}
}

void ChattingSystem::PushChattingText(const string& user, const char* chat)
{
	string s = user + " : ";
	s.append(chat);

	ZeroMemory(m_chat[m_nCurrentText], sizeof(256));
	int chattingLen = MultiByteToWideChar(CP_ACP, 0, s.c_str(), s.length(), NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), s.length(), m_chat[m_nCurrentText], chattingLen);
	m_chat[m_nCurrentText][chattingLen] = '\0';

	if(m_dequeText.size() >= m_maxChatSentenceCount - 1)
	{
		//가장 위에 위치한 텍스트를 pop한다.
		m_dequeText.pop_back();
		m_dequeText.emplace_front(make_pair(m_chat[m_nCurrentText], (UINT32)chattingLen));
		m_nCurrentText = (++m_nCurrentText) % m_maxChatSentenceCount;
	}
	else
	{
		m_dequeText.emplace_front(make_pair(m_chat[m_nCurrentText], (UINT32)chattingLen));
		m_nCurrentText = (++m_nCurrentText) % m_maxChatSentenceCount;
	}

}
//프로그램 내에서 한영 전환
void ChattingSystem::SetIMEMode(HWND hWnd, bool bHanMode)
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

void ChattingSystem::ClearChattingBox()
{
	m_dequeText.clear();
}

bool ChattingSystem::ComposeHangeul(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc = ImmGetContext(hWnd);

	if (hImc == nullptr)
		return false;

	int nLength = 0;
	TCHAR wszComp[4] = { 0, };

	//IME 문자 조합이 완료
	if (lParam & GCS_RESULTSTR)
	{
		nLength = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, nullptr, 0);
		if (nLength > 0)
		{
			ImmGetCompositionStringW(hImc, GCS_RESULTSTR, wszComp, nLength);

			for (int i = 0; i < nLength; ++i)
			{
				if (wszComp[i] != 0)
				{
					//조합된 문자 보여지게 하기
					wstring s(wszComp);
					if (m_wsChat.size() > 0 && m_composeCount > 0)
					{
						m_wsChat.pop_back();
					}
					m_wsChat += s;

					//	m_iter = m_wsChat.rbegin();

					m_composeCount = 0;
					//return wszComp;
				}
			}
		}
	}
	else if (lParam & GCS_COMPSTR)
	{
		nLength = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);

		if (nLength > 0)
		{
			ImmGetCompositionStringW(hImc, GCS_COMPSTR, wszComp, nLength);
			for (int i = 0; i < nLength; ++i)
			{
				if (wszComp[i] != 0)
				{
					wstring s(wszComp);
					if (m_composeCount > 0 && m_wsChat.size() > 0)
					{
						m_wsChat.pop_back();
						m_composeCount--;
					}
					m_wsChat += s;

					m_composeCount++;
					//조합 중인 문자가 보여지는 코드 삽입
				}
			}
		}
	}

	//IME핸들 해제
	ImmReleaseContext(hWnd, hImc);
	return true;
}

