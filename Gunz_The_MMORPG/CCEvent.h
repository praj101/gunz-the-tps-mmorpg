#pragma once
/* 
	CCEvent.h
		
		This header file contains defines for keyboard and mouse events. Everything
		in this file is focused on registering events such as key press, mouse
		movement, wheel scrolling, shift key, alt key, enter press, etc...

		CLASSES
		--------------------------------
			- CCEvent
				\_	Contains functions that grab the state of the mouse and 
					keyboard.

		FUNCTIONS
		--------------------------------
			- CCEvent
				/_ static bool		GetShiftState()
				|_ static bool		GetCtrlState()
				|_ static bool		GetAltState()
				|_ static bool		IsKeyDown(int key)
				|_ static bool		GetLButtonState()
				|_ static bool		GetCCButtonState()
				|_ static bool		GetRButtonState()
				|_ static sPoint	GetMousePos()
				|_ static void		CCEvent::ForceSetIME(DWORD conversion, DWORD sentence)
				|_ static void		SetIMESupport(bool bSupport)
				|_ static bool		GetIMESupport()
				\_ int				TranslateEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
*/
#include "CCTypes.h"

#ifdef WIN32
	#include <windows.h>
	#pragma comment(lib, "Imm32.lib")
#endif

/*
	Message types
*/
#define CCWM_LBUTTONDOWN			0x0010
#define CCWM_LBUTTONUP				0x0011
#define CCWM_LBUTTONDBLCLK			0x0012
#define CCWM_RBUTTONDOWN			0x0013
#define CCWM_RBUTTONUP				0x0014
#define CCWM_RBUTTONDBLCLK			0x0015
#define CCWM_CCBUTTONDOWN			0x0016
#define CCWM_CCBUTTONUP				0x0017
#define CCWM_CCBUTTONDBLCLK			0x0018
#define CCWM_MOUSEMOVE				0x0019
#define CCWM_MOUSEWHEEL				0x0020
#define CCWM_CHAR					0x0030
#define CCWM_KEYDOWN				0x0031
#define CCWM_KEYUP					0x0032
#define CCWM_SYSCHAR				0x0033
#define CCWM_SYSKEYDOWN				0x0034
#define CCWM_SYSKEYUP				0x0035
#define CCWM_HOTKEY					0x0040
#define CCWM_ACTIONKEYDOWN			0x0041
#define CCWM_ACTIONKEYUP			0x0042
#define CCWM_ACTIONPRESSED			0x0043
#define CCWM_ACTIONRELEASED			0x0044
#define CCWM_IMECOMPOSE				0x0050
#define CCWM_IMECONVERSION			0x0051

#define COMPOSITIONSTRING_LENGTH	256

class CCEvent{
public:
	int				iMessage;
	sPoint			sPos;
	uint			uKey;	// WM_CHAR keycode
	int				iDelta;	// Wheel mouse delta
	bool			bShift;
	bool			bAlt;
	bool			bCtrl;

	static bool		bLButton;
	static bool		bCCButton;
	static bool		bRButton;

	static sPoint	LatestPos;

	char			szIMECompositionString[COMPOSITIONSTRING_LENGTH];
	char			szIMECompositionResultString[COMPOSITIONSTRING_LENGTH];
	static bool		bIMESupport;

public:
	// Async Functions
	static bool		GetShiftState();
	static bool		GetCtrlState();
	static bool		GetAltState();
	static bool		IsKeyDown(int key);

	static bool		GetLButtonState();
	static bool		GetCCButtonState();
	static bool		GetRButtonState();

	static sPoint	GetMousePos();
	
	static void		CCEvent::ForceSetIME(DWORD conversion, DWORD sentence);

	static void		SetIMESupport(bool bSupport) { bIMESupport = bSupport; };
	static bool		GetIMESupport() { return bIMESupport; };

	#define EVENT_NOT_PROCESSED		0x00
	#define	EVENT_PROCESSED			0x01
	#define	EVENT_CORE_TRANSLATED	0x02

	int TranslateEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};