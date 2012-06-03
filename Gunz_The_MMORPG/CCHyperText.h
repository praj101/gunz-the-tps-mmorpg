#pragma once
/*
	CCHyperText.h
	Programming by Joongpil Cho

	Simple file format that you are using. Is similar to a normal text [...]]Instruction in the format of the text, 
	Show or you can insert a picture.
*/
#include <crtdbg.h>
#include <stdio.h>
#include "CCList.h"

typedef enum {
	CCHTE_PLAINTEXT = 0,	//TAG plain text.(Not tagged).
	CCHTE_STD,			//MAIET Hyper Text markup to show that
	CCHTE_IMAGE,			//image tag
	CCHTE_STYLE,			//style tags
	CCHTE_LINK,			//start a hyperlink
	CCHTE_LINKEND,		//end of hyperlinks
	CCHTE_BR,			//the next line.
	CCHTE_DEFAULT,		//as the initial setting
}CCHT_ELEMENT;

typedef enum {
	CCHTA_TEXT = 0,		//PLAINTEXT arguments for, char * may contain.
	CCHTA_BACKGROUND,	//??STD color values ??for the parameters or image file can come.
	CCHTA_COLOR,			//COLOR values, beginning with a # 6-digit hexadecimal number, one for each two-digit representation of the color information is.(# RGB)
	CCHTA_SIZE,			//SIZE value, integer ()
	CCHTA_ALIGN,			//ALIGN value, integer ()
	CCHTA_TYPE,			//TYPE value, integer ()
	CCHTA_SRC,			//SRC values, strings
	CCHTA_BOLD,			//BOLD values, integer ()
	CCHTA_HIGHLIGHT,
	CCHTA_HREF,			//Link reference, the string
	CCHTA_XMARGIN,		//extra X-axis of the figure
	CCHTA_YMARGIN,		//Y-axis of the figure an extra
}CCHT_ARGUMENT;

//Text Element for each value of the secondary figures
class CCHyperTextArg {
public:
	CCHT_ARGUMENT		uId;			//element argument
	
	CCHyperTextArg(CCHT_ARGUMENT id){
		uId = id;
	}
};

template<CCHT_ARGUMENT Arg>
class CCHTA_IntegerArg : public CCHyperTextArg {
public:
	int					nVal;

	CCHTA_IntegerArg(int value) : CCHyperTextArg(Arg){
		nVal = value;
	}
};

template<CCHT_ARGUMENT Arg>
class CCHTA_StringArg : public CCHyperTextArg {
public:
	char*				val;

	CCHTA_StringArg(char *szText):CCHyperTextArg(Arg){
		val = strdup(szText);
	}
	~CCHTA_StringArg(){
		if(val) free(val);
	}
};

template<CCHT_ARGUMENT Arg>
class CCHTA_ColorArg : public CCHyperTextArg {
public:
	sColor				m_sColor;

	CCHTA_ColorArg(sColor color) : CCHyperTextArg(Arg){
		m_sColor = color;
	}
};

class CCHTA_Background : public CCHyperTextArg {
	CCHTA_Background() : CCHyperTextArg(CCHTA_BACKGROUND){
		m_sColor = sColor(0,0,0);
		szPath = NULL;
	}
public:
	char*				szPath;		//Image path
	sColor				m_sColor;

	CCHTA_Background(sColor color) : CCHyperTextArg(CCHTA_BACKGROUND){
		szPath = NULL;
		m_sColor = color;
	}

	CCHTA_Background(char *path) : CCHyperTextArg(CCHTA_BACKGROUND){
		m_sColor = sColor(0,0,0);
		szPath = strdup(path);
	}

	~CCHTA_Background(){
		if(szPath) free(szPath);	
	}
};

typedef class CCHTA_IntegerArg<CCHTA_SIZE>		CCHTA_Size;
typedef class CCHTA_IntegerArg<CCHTA_ALIGN>		CCHTA_Align;
typedef class CCHTA_IntegerArg<CCHTA_TYPE>		CCHTA_Type;
typedef class CCHTA_IntegerArg<CCHTA_BOLD>		CCHTA_Bold;
typedef class CCHTA_IntegerArg<CCHTA_XMARGIN>	CCHTA_XMargin;
typedef class CCHTA_IntegerArg<CCHTA_YMARGIN>	CCHTA_YMargin;

typedef class CCHTA_ColorArg<CCHTA_COLOR>		CCHTA_Color;
typedef class CCHTA_ColorArg<CCHTA_HIGHLIGHT>	CCHTA_Highlight;

typedef class CCHTA_StringArg<CCHTA_TEXT>		CCHTA_Text;
typedef class CCHTA_StringArg<CCHTA_SRC>		CCHTA_Src;
typedef class CCHTA_StringArg<CCHTA_HREF>		CCHTA_HRef;



class CCHyperTextElement
{
public:
	CCHT_ELEMENT					nType;	//type of elements
	CCLinkedList<CCHyperTextArg>	Args;	//element of the argument list

	//Constructors, destructors
	CCHyperTextElement(CCHT_ELEMENT type){
		nType		= type;
	}

	virtual ~CCHyperTextElement(){
		Args.DeleteAll();
	}

	void Add(CCHyperTextArg* pNew){
		Args.Add(pNew);
	}
};

class CCHyperText
{
private:
	char*			m_pBuffer;			//Text Buffer, CCHyperText only to parse the contents of the memory.
	int				m_iLen;				//size of the buffer
	int				bp;					//Buffer pointer of
	int				m_iOffset;			//m_szScan the buffer pointer
	bool			m_bTagReady;
	char			m_szScan[20480];	//Scan a string pointer values ??are stored, yytext and Bana to play a similar role.

	void			Gather(char b){ m_szScan[m_iOffset++] = b; }
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS

	int				Scan();
	bool			Parse();
	char			Input(){ return m_pBuffer[bp++]; }
	void			Unput(){ bp--; }
	void			GetDigit(char c);

public:
	CCLinkedList<CCHyperTextElement>	Elements;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	
	CCHyperText();
	virtual ~CCHyperText(){ Close(); }

	/*	
		This class does not report an error.
		Hypertext has been applied in the game when the game should be continued, so ...

		szTextBuffer: text buffer
	*/
	bool Open(char *szTextBuffer);
	void Close();

	bool IsValid(){ return (m_pBuffer==NULL)?false:true; }
};

inline void CCHyperText::GetDigit(char c)
{
	for(;isdigit(c) ;c=Input()) Gather(c);
	Unput();
}

class CCHyperTextFile
{
private:
	char*	m_pBuffer;
public:
	CCHyperTextFile()
	{
		m_pBuffer = NULL;
	}

	virtual ~CCHyperTextFile()
	{
		Close();
	}

	bool Open(char *szPath);

	char* GetBuffer(){ return m_pBuffer; }

	void Close()
	{
		if(m_pBuffer == NULL)
		{
			delete m_pBuffer;
			m_pBuffer = NULL;
		}
	}
};
