#include "../../Stdafx/stdafx.h"
#include "IDInputSystem.h"

#ifdef _WITH_SERVER_
#ifdef _WITH_DIRECT2D_
CIDInput::CIDInput()
{

}

CIDInput::~CIDInput()
{

}

void CIDInput::Initialize(IDWriteTextFormat* pFont, IDWriteTextLayout* pTextLayout, ID2D1SolidColorBrush* pFontColor)
{
	m_pFont = pFont;
	m_pTextLayout = pTextLayout;
	m_pFontColor = pFontColor;
}

TCHAR* CIDInput::StringToTCHAR(string& s)
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

string CIDInput::TCHARToString(const TCHAR* ptsz)
{
	size_t len = (int)wcslen((wchar_t*)ptsz);
	const size_t charlen = 2 * len + 1;

	char* psz = new char[charlen];
	wcstombs(psz, (wchar_t*)ptsz, charlen);

	string s = psz;
	delete[] psz;
	return s;
}

//프로그램 내에서 한영 전환
void CIDInput::SetIMEMode(HWND hWnd,bool bHanMode)
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

bool CIDInput::ComposeHangeul(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HIMC hImc = ImmGetContext(hWnd);

	if (hImc == nullptr)
		return false;

	
	int nLength = 0;
	TCHAR wszComp[4] = { 0, };

	//IME 문자 조합이 완료
	if(lParam & GCS_RESULTSTR)
	{		
		nLength = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, nullptr, 0);
		if(nLength > 0)
		{
			ImmGetCompositionStringW(hImc, GCS_RESULTSTR, wszComp, nLength);

			for(int i = 0;i<nLength ;++i)
			{
				if(wszComp[i]!=0)
				{
				
					//조합된 문자 보여지게 하기
					wstring s(wszComp);
					if (m_wsID.size() > 0 && m_composeCount > 0) 
					{
							m_wsID.pop_back();
					}
					m_wsID += s;

				//	m_iter = m_wsChat.rbegin();
					
					m_composeCount = 0;
					//return wszComp;
				}
			}
		}
	}
	else if(lParam & GCS_COMPSTR)
	{
		nLength = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);

		
		if(nLength > 0)
		{
			ImmGetCompositionStringW(hImc, GCS_COMPSTR, wszComp, nLength);


			for(int i=0;i<nLength;++i)
			{
				if(wszComp[i] != 0)
				{
				
					wstring s(wszComp);
					if (m_composeCount > 0 && m_wsID.size() > 0)
					{
						m_wsID.pop_back();
						m_composeCount--;
					}
					m_wsID += s;

					m_composeCount++;
					//조합 중인 문자가 보여지는 코드 삽입
					
					
				}				
			}
		}
	}

	//IME핸들 해제
	ImmReleaseContext(hWnd,hImc);
	return true;
	
}

TCHAR* CIDInput::GetPlayerName()
{
	return (wchar_t *)m_wsID.c_str();
	
}
void CIDInput::ProcessIDInput(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	
	if (wParam == VK_RETURN)
	{
		m_wsID.clear();
		m_wsID.shrink_to_fit();
	}

	//현재 영문이 활성화 됐을 경우
	if (GetIMEMode() == IME_CMODE_ALPHANUMERIC)
	{
		if (wParam != VK_RETURN && wParam != VK_BACK)
		{
		
			if (m_wsID.size() <= (size_t)LENGTH::ENG)
			{
				m_wsID += (TCHAR)wParam;
			}

		}
	}
	else if(GetIMEMode() == IME_CMODE_NATIVE)
	{
		if (wParam != VK_RETURN)
		{
		
			if (m_wsID.size() <= (size_t)LENGTH::KOR)
			{
				//m_wsChat += (TCHAR)wParam;
				ComposeHangeul(hWnd, wParam, lParam);
			}
		
		}
	}
	if( GetKeyState(VK_BACK) & 0x8000)
	{
	
		if (m_wsID.size() > 0)
		{
			m_wsID.pop_back();
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

void CIDInput::ShowIDInput(ID2D1DeviceContext2* pd2dDeviceContext)
{
	D2D1_RECT_F idText{ 0,0,0,0 };
	wstring wstr = m_wsID.c_str();


	//420 은 1200프레임에서의 left값 
	UINT originX = 1280;
	UINT originY = 780;

	//바뀐 현재 프레임 크기 
	UINT currFrameX = FRAME_BUFFER_WIDTH;
	UINT currFrameY = FRAME_BUFFER_HEIGHT;

	RECT nameRect{ 0, };
	nameRect.left = (480 *  FRAME_BUFFER_WIDTH) / originX;
	nameRect.top = (650 *  FRAME_BUFFER_HEIGHT) / originY;
	nameRect.right = (750 *  FRAME_BUFFER_WIDTH) / originX;
	nameRect.bottom = (650 *  FRAME_BUFFER_HEIGHT) / originY;

	idText = D2D1::RectF(nameRect.left, nameRect.top , nameRect.right, nameRect.bottom);
	pd2dDeviceContext->DrawTextW(wstr.c_str(), wstr.length(), m_pFont, &idText, m_pFontColor);
}

void CIDInput::Destroy()
{
	m_wsID.clear();
	m_wsID.shrink_to_fit();
}

#endif
#endif