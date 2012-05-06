#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include "DirectX/dinput.h"
#include <windows.h>

#define KEYNAMETABLE_COUNT	256
#define ISKEYDOWN(_ScanCodeTable, _ScanCode)	\
	((_ScanCodeTable[_ScanCode]&0x80)?true:false)

struct ZDIBUFFER{
	BYTE uKey;
	bool bPressed;
};

// Immediate Mode : a mode in which you get the current status
// Buffered Mode : currently reading an input into the buffer mode


/// DirectInput Wrapper
class ZDirectInput{
protected:
	HMODULE					m_hD3DLibrary;
	BOOL					m_bInitialized;
	LPDIRECTINPUT8			m_pDI;				///< The DirectInput object         
	LPDIRECTINPUTDEVICE8	m_pKeyboard;		///< The keyboard device 
	BOOL					m_bImmediateMode;

	LPDIRECTINPUTDEVICE8	m_pMouse;			///< mouse device
	unsigned int			m_iMouseButtons;	///> mouse buttons
	bool					m_bMouseButtonStates[8];

	LPDIRECTINPUTDEVICE8	m_pJoystick;		///< joystick device
	unsigned int			m_iJoyButtons;		///< joystick buttons
	unsigned int			m_iJoyPovs;			///< joystick povs
	unsigned int			m_iFFAxis;			///< force feedback axis
	bool					m_bForceFeedback;
	LPDIRECTINPUTEFFECT		m_pFFEffect;		///< force feedback effect

	char*					m_szKeyNameTable[KEYNAMETABLE_COUNT];

	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
	static BOOL CALLBACK EnumJoyObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

public:
	ZDirectInput();
	virtual ~ZDirectInput();

	bool CreateDirectInput();
	bool Create(HWND hWnd, BOOL bExclusive=TRUE, BOOL bImmediateMode=TRUE);
	void Destroy();

	void OnActivate(bool bActive);

	BOOL IsInitialized()	{ return m_bInitialized; }

	///////////////////////////////////////////////////////////////////////////////////////
	//Keyboard

	//Load the keyboard buffer, scan the code and Down / Up flags, including the read buffer to increase the number of returns.
	DWORD GetKeyboardBufferedData(ZDIBUFFER* pBuffer,unsigned int nBuffer);

	const char* GetKeyName(unsigned long int uKey);


	////////////////////////////////////////////////////////////////////////////////////////
	//Mouse

	//Getting the number of mouse buttons
	unsigned int GetMouseButtonCount()		{ return m_iMouseButtons; }

	//Load the mouse as a buffer
	DWORD GetMouseBufferedData(int* pSumX,int* pSumY, ZDIBUFFER* pBuffer,unsigned int nBuffer);
	
	//Getting the mouse data
//	bool GetMouseImmediateData(DIMOUSESTATE2 *pdims2);



	///////////////////////////////////////////////////////////////////////////////////////
	//Joystick

	//Pov Acquiring number of
	unsigned int GetJoystickPovCount()		{ return m_iJoyPovs; }
	//Getting the number of buttons
	unsigned int GetJoystickButtonCount()	{ return m_iJoyButtons; }
	//Joystick state Acquiring
	bool GetJoystickData(DIJOYSTATE2* pjs);

	bool SetDeviceForcesXY(int nXForce, int nYForce);

};
