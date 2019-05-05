#include "Stdafx/Stdafx.h"
#include "Default\Resource.h"
#include "GameFramework/GameFramework.h"
#include "Chatting\Chatting.h"

#define MAX_LOADSTRING 100

HINSTANCE					ghAppInstance;
TCHAR							szTitle[MAX_LOADSTRING];
TCHAR							szWindowClass[MAX_LOADSTRING];

CGameFramework		gGameFramework;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_FREEZEBOMB, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) return(FALSE);

	hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FREEZEBOMB));

	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
			gGameFramework.FrameAdvance();
	}
	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BEARICON));
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_BEARICON));

	return ::RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	ghAppInstance = hInstance;
	//
	RECT rc = { 0, 0,FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU |WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!hMainWnd) return(FALSE);

	if (!gGameFramework.OnCreate(hInstance, hMainWnd))
	{
		::PostQuitMessage(0);
	}
	//gGameFramework.ChangeSwapChainState();
	//::ShowWindow(hMainWnd, nCmdShow);
	//::UpdateWindow(hMainWnd);

	return(TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		//Alt키 눌렀을 때, 멈추는 현상을 해결하기위해
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);

		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			::DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			::DestroyWindow(hWnd);
			break;
		default:
			return(::DefWindowProc(hWnd, message, wParam, lParam));
		}
		break;
	case WM_CHAR:
#ifdef _WITH_DIRECT2D_
		if (ChattingSystem::GetInstance()->IsChattingActive())
			ChattingSystem::GetInstance()->ProcessChatting(hWnd,wParam,lParam);
#endif
		break;
		//한글 조합 시작
	case WM_IME_STARTCOMPOSITION:
		if(ChattingSystem::GetInstance()->IsChattingActive())
			//ChattingSystem::GetInstance()-
		break;
	case WM_IME_SETCONTEXT:
		//응용 프로그램이 활성/비활성될 때 메세지 발생
		//wPram이 True이면 활성화,False이면 비활성화
		break;
	case WM_IME_NOTIFY:
		//IME 윈도우가 변경되었다는 통지 메세지. 응용 프로그램이 활성/비활성화될 때 보내짐
		//wParam에 어떤 변경인지 전달됨.
		break;

		//한글 조합중
	case WM_IME_COMPOSITION:
		if (ChattingSystem::GetInstance()->IsChattingActive()) 
		{
			//IME가 조립 상태가 변경될 때마다 보내짐.
			//한글의 경우 한 음소가 입력될 때마다 메세지가 발생함.
				//조립중인 문자를 보여주고 싶을때 사용하면 됨.
			//wParam에는 조립중인 문자의 코드가 전달됨.(2바이트의 DBSC문자로 조립중인 중간 문자 코드)
			//lParam에는 조립 상태가 어떻게 변경되었는지를 나타내는 플래그의 집합.
				//-GCS_COMPSTR : 문자 조립중
				//-GCS_RESULTSTR : 한 음절을 조립 완료
			ChattingSystem::GetInstance()->ProcessChatting(hWnd, wParam, lParam);
			//ChattingSystem::GetInstance()->ComposeHangeul(hWnd, wParam,lParam);
		}
		break;
	case WM_IME_ENDCOMPOSITION:
		//한글 입력을 종료할때 발생한다.
		//한글 입력중 한글키가 아닌 키를 누를때(숫자키,Enter키,한영 변환키,방향키 등)
		//마우스로 클릭하거나 포커스를 잃을때 발생(즉, 한글 입력중에는 검정상자가 생기는데
		//그 검정상자가 없어질때 반환)	
		break;
	case WM_IME_CHAR:
	//	//문자 하나가 완성되었을때 보내짐.
	//	//'wParam'에는 완성된 문자의 코드가 전달됨.
	//	//1바이트 문자만 전달되는 WM_CHAR메시지와는 달리 이 코드는 DBCS일 수 도 있다.
	//	if (gGameFramework.IsChattingOn())
	//		ChattingSystem::GetInstance()->ProcessChatting(hWnd,wParam,lParam,true);
		break;

	//case WM_SETFOCUS:
	//	CreateCaret(hWnd, NULL, 4, 32);
	//	SetCaretPos(10,10);
	//	ShowCaret(hWnd);
	//	return 0;
	//case WM_KILLFOCUS:
	//	DestroyCaret();
	//	return 0;
	case WM_SOCKET:
		if (WSAGETSELECTERROR(lParam))
		{
			closesocket((SOCKET)wParam);
			PostQuitMessage(0);
		}
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			gGameFramework.getNetwork()->ReadPacket();
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_PAINT:
		hdc = ::BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return(::DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return((INT_PTR)TRUE);
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			::EndDialog(hDlg, LOWORD(wParam));
			return((INT_PTR)TRUE);
		}
		break;
	}
	return((INT_PTR)FALSE);
}
