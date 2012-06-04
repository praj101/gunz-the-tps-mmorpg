#include "stdafx.h"
#include "CCEvent.h"
#include <zmouse.h>
#include "Core.h"

sPoint CCEvent::LatestPos = sPoint(0, 0);	// 마지막 위치
bool CCEvent::bIMESupport = false;	// Default IME Support Enabled
bool CCEvent::bLButton = false;
bool CCEvent::bCCButton = false;
bool CCEvent::bRButton = false;

bool CCEvent::GetShiftState(void)
{
	if((GetKeyState(VK_SHIFT)&0x8000)==0x8000) return true;
	return false;
}

bool CCEvent::GetCtrlState(void)
{
	if((GetKeyState(VK_CONTROL)&0x8000)==0x8000) return true;
	return false;
}

bool CCEvent::GetAltState(void)
{
	if((GetKeyState(VK_MENU)&0x8000)==0x8000) return true;
	return false;
}

bool CCEvent::IsKeyDown(int key)
{
	return ((GetAsyncKeyState(key) & 0x8000)!=0);
}

bool CCEvent::GetLButtonState()
{
	return bLButton;
}

bool CCEvent::GetRButtonState()
{
	return bRButton;
}

bool CCEvent::GetCCButtonState()
{
	return bCCButton;
}

sPoint CCEvent::GetMousePos(void)
{
	POINT p;
	GetCursorPos(&p);
	return sPoint(p.x, p.y);
}

#include "CCDebug.h"
void CCEvent::ForceSetIME(DWORD fdwConversion,DWORD fdwSentence)
{
	HWND hWnd = Core::GetInstance()->GetHWND();
	HIMC hImc = ImmGetContext(hWnd);
	if (hImc)
	{
		ImmSetConversionStatus(hImc,fdwConversion,fdwSentence);
		ImmReleaseContext(hWnd, hImc);
	}
}

int CCEvent::TranslateEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	//bShift = GetShiftState();
	bCtrl = GetCtrlState();
	//bAlt = GetAltState();

	switch(message){
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_MOUSEMOVE:
		sPos.x = LOWORD(lparam);
		sPos.y = HIWORD(lparam);

		LatestPos = sPos;
		break;
	case WM_MOUSEWHEEL:	// 휠은 절대좌표로 날아옴
		{
			POINT pos;
			pos.x = LOWORD(lparam);
			pos.y = HIWORD(lparam);
			ScreenToClient(Core::GetInstance()->GetHWND(),&pos);

			sPos.x = pos.x;
			sPos.y = pos.y;

			LatestPos = sPos;
		}
		break;
	}

	switch(message){
	case WM_LBUTTONDOWN:
		iMessage = CCWM_LBUTTONDOWN;
		bLButton=true;
		return EVENT_CORE_TRANSLATED;
	case WM_LBUTTONUP:
		bLButton=false;
		iMessage = CCWM_LBUTTONUP;
		return EVENT_CORE_TRANSLATED;
	case WM_RBUTTONDOWN:
		bRButton=true;
		iMessage = CCWM_RBUTTONDOWN;
		return EVENT_CORE_TRANSLATED;
	case WM_RBUTTONUP:
		bRButton=false;
		iMessage = CCWM_RBUTTONUP;
		return EVENT_CORE_TRANSLATED;
	case WM_MBUTTONDOWN:
		bCCButton=true;
		iMessage = CCWM_CCBUTTONDOWN;
		return EVENT_CORE_TRANSLATED;
	case WM_MBUTTONUP:
		bCCButton=false;
		iMessage = CCWM_CCBUTTONUP;
		return EVENT_CORE_TRANSLATED;
	case WM_LBUTTONDBLCLK:
		iMessage = CCWM_LBUTTONDBLCLK;
		bLButton=true;
		return EVENT_CORE_TRANSLATED;
	case WM_RBUTTONDBLCLK:
		iMessage = CCWM_RBUTTONDBLCLK;
		bRButton=true;
		return EVENT_CORE_TRANSLATED;
	case WM_MBUTTONDBLCLK:
		iMessage = CCWM_CCBUTTONDBLCLK;
		bCCButton=true;
		return EVENT_CORE_TRANSLATED;
	case WM_MOUSEMOVE:
		iMessage = CCWM_MOUSEMOVE;
		return EVENT_CORE_TRANSLATED;
	case WM_MOUSEWHEEL:
		iMessage = CCWM_MOUSEWHEEL;
		iDelta = (short)HIWORD(wparam);
		return EVENT_CORE_TRANSLATED;
		/*
	case WM_RESIZE_EVENT:
		iMessage = CCWM_RESIZE;
		return EVENT_CORE_TRANSLATED;

	case WM_MOVIE_NOTIFY:
		iMessage = CCWM_MOVIE_NOTIFY;
		return EVENT_CORE_TRANSLATED;
		*/

	case WM_KEYDOWN:
		iMessage = CCWM_KEYDOWN;
		uKey = wparam;
		return EVENT_CORE_TRANSLATED;

		/*
		switch(wparam){
		case VK_DELETE:
			iMessage = CCWM_SCHAR;
			nKey = wparam;
			return EVENT_CORE_TRANSLATED;
		case VK_LEFT:
		case VK_RIGHT:
		case VK_HOME:
		case VK_END:
			iMessage = CCWM_CHAR;
			nKey = wparam;
			return EVENT_CORE_TRANSLATED;
		default:
			return EVENT_NOT_PROCESSED;
		}
		*/
		return EVENT_CORE_TRANSLATED;
	case WM_KEYUP:
		iMessage = CCWM_KEYUP;
		uKey = wparam;
		return EVENT_CORE_TRANSLATED;
	case WM_SYSCHAR:
		iMessage = CCWM_SYSCHAR;
		uKey = wparam;
		bAlt = true;
		return EVENT_CORE_TRANSLATED;
	case WM_SYSKEYDOWN:
		iMessage = CCWM_SYSKEYDOWN;
		uKey = wparam;
		return EVENT_CORE_TRANSLATED;
	case WM_SYSKEYUP:
		iMessage = CCWM_SYSKEYUP;
		uKey = wparam;
		return EVENT_CORE_TRANSLATED;
	case WM_CHAR:
		iMessage = CCWM_CHAR;
		uKey = wparam;
		return EVENT_CORE_TRANSLATED;
	case WM_HOTKEY:
		iMessage = CCWM_HOTKEY;
		uKey = wparam;
		return EVENT_CORE_TRANSLATED;

	// IME 관련
	case WM_INPUTLANGCHANGE:
		// 원래는 여기에 Input Language 이벤트 변화를 받아야 하지만, Core에서는 폴링 방식으로
		// GetKeyboardLayout()을 이용하여 필요한 때 얻어서 사용한다.
		// Core::IsNativeIME() 참고
		// 2005-08-18, 이장호
		return (EVENT_PROCESSED|EVENT_CORE_TRANSLATED);
	case WM_IME_STARTCOMPOSITION:
		if(bIMESupport==true){
			//iMessage = CCWM_IMECOMPOSE;
			szIMECompositionString[0] = NULL;
			szIMECompositionResultString[0] = NULL;
//			OutputDebugString("WM_IME_STARTCOMPOSITION");
			return (EVENT_PROCESSED|EVENT_CORE_TRANSLATED);
		}
		return EVENT_PROCESSED;	// 처리가 되므로
	case WM_IME_COMPOSITION:
		if(bIMESupport==true){
//			OutputDebugString("WM_IME_COMPOSITION\n");
			iMessage = CCWM_IMECOMPOSE;

			Core* pCore = Core::GetInstance();

			HIMC hIMC = ImmGetContext(hwnd);
			if (hIMC){
				if(lparam&GCS_RESULTSTR){
					LONG i = ImmGetCompositionString(hIMC, GCS_RESULTSTR, szIMECompositionResultString, sizeof(szIMECompositionResultString));
					szIMECompositionResultString[i] = NULL;
					//				OutputDebugString("GCS_RESULTSTR\n");
					pCore->m_iCompositionAttributeSize = 0;
					memset(pCore->m_iCompositionAttributes, 0, sizeof(BYTE)*(COMPOSITIONSTRING_LENGTH));
					//memset(pCore->m_dwCompositionClauses, 0, sizeof(DWORD)*(MIMECOMPOSITIONSTRING_LENGTH));
				}
				else{
					szIMECompositionResultString[0] = NULL;
				}
				if(lparam&GCS_COMPSTR){
					LONG i = ImmGetCompositionString(hIMC, GCS_COMPSTR, szIMECompositionString, sizeof(szIMECompositionString));
					szIMECompositionString[i] = NULL;
					//				OutputDebugString("GCS_COMPSTR\n");

				}
				else{
					szIMECompositionString[0] = NULL;
				}

				/*
				// 절 정보 얻기(일어)
				if(lparam & GCS_COMPCLAUSE)
					ImmGetCompositionString(hIMC, GCS_COMPCLAUSE, pCore->m_dwCompositionClauses, sizeof(pCore->m_dwCompositionClauses));
				*/

				// 속성 얻기(각 절이 어떤 값을 가지는가)
				if(lparam & GCS_COMPATTR)
					pCore->m_iCompositionAttributeSize = ImmGetCompositionString(hIMC, GCS_COMPATTR, pCore->m_iCompositionAttributes, sizeof(pCore->m_iCompositionAttributes));

				if(lparam & GCS_CURSORPOS)
					pCore->m_iCompositionCaretPosition = ImmGetCompositionString(hIMC, GCS_CURSORPOS, NULL, 0);
			}
			
//			OutputDebugString("Comp - ");
//			OutputDebugString(szIMECompositionString);
//			OutputDebugString("\n");
//			OutputDebugString("Resl - ");
//			OutputDebugString(szIMECompositionResultString);
//			OutputDebugString("\n");
			return (EVENT_PROCESSED|EVENT_CORE_TRANSLATED);
		}
		return EVENT_PROCESSED;	// 처리가 되므로
	case WM_IME_ENDCOMPOSITION:
		if(bIMESupport==true){
			//iMessage = CCWM_IMECOMPOSE;
			szIMECompositionString[0] = NULL;
			szIMECompositionResultString[0] = NULL;
//			OutputDebugString("WM_IME_ENDCOMPOSITION");
			Core* pCore = Core::GetInstance();
			pCore->m_iCompositionCaretPosition = 0;
			return (EVENT_PROCESSED|EVENT_CORE_TRANSLATED);
		}
		return EVENT_PROCESSED;	// 처리가 되므로
	case WM_IME_NOTIFY:
		{
			Core* pCore = Core::GetInstance();
			if(bIMESupport==true && pCore->IsEnableIME()==true){	// IsEnableIME() 체크는 창 밖에서 Candidate List가 안나오게 하기 위해
				/*
				if(wparam==IMN_SETCONVERSIONMODE){
					iMessage = CCWM_IMECONVERSION;

					if(Core::GetInstance()->m_dwIMEConvMode==IME_CMODE_ALPHANUMERIC)
						Core::GetInstance()->m_dwIMEConvMode=IME_CMODE_NATIVE;
					else
						Core::GetInstance()->m_dwIMEConvMode=IME_CMODE_ALPHANUMERIC;
					return EVENT_CORE_TRANSLATED;
				} else if (wparam == IMN_OPENCANDIDATE) {
					OutputDebugString("IMEDEBUG: IMN_OPENCANDIDATE \n");
				} else if (wparam == IMN_CLOSECANDIDATE) {
					OutputDebugString("IMEDEBUG: IMN_CLOSECANDIDATE \n");
				} else if (wparam == IMN_CHANGECANDIDATE) {
					OutputDebugString("IMEDEBUG: IMN_CHANGECANDIDATE \n");
				} else if (wparam == IMN_SETCANDIDATEPOS) {
					OutputDebugString("IMEDEBUG: IMN_SETCANDIDATEPOS \n");
				}
				*/
				if(wparam==IMN_OPENCANDIDATE || wparam==IMN_CHANGECANDIDATE){
					// IMN_CHANGECANDIDATE 또한 Open과 마찬가지로 CandidateList를 얻어야 함
					pCore->OpenCandidateList();
				}
				else if(wparam==IMN_CLOSECANDIDATE){
					pCore->CloseCandidateList();
				}
				return (EVENT_PROCESSED|EVENT_CORE_TRANSLATED);
			}
		}
		return EVENT_PROCESSED;
	case WM_IME_SETCONTEXT:
		// 일본어에서 F5에 입력창 뜨는 명령어 차단
		return EVENT_PROCESSED;
	default:
		return EVENT_NOT_PROCESSED;
	}

	return EVENT_NOT_PROCESSED;
}

