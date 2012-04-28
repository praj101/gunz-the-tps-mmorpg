/*
	CCHyperText.h
	Programming by Joongpil Cho

	간이 파일 포맷을 사용하고 있음.	일반적인 텍스트와 유사하나 [[...]]안의 명령어를 통해서 텍스트의 형식을 
	나타내거나 그림을 삽입할 수 있다.
*/
#include <crtdbg.h>
#include <stdio.h>
#include "CCList.h"

typedef enum {
	CCHTE_PLAINTEXT = 0,	//TAG없는 일반 텍스트. (태그 아님)
	CCHTE_STD,			//MAIET Hyper Text임을 표시하는 태그
	CCHTE_IMAGE,			//이미지 태그
	CCHTE_STYLE,			//스타일 태그
	CCHTE_LINK,			//하이퍼 링크 시작
	CCHTE_LINKEND,		//하이퍼 링크의 끝
	CCHTE_BR,			//다음 줄로...
	CCHTE_DEFAULT,		//초기설정대로
}CCHT_ELEMENT;

typedef enum {
	CCHTA_TEXT = 0,		//PLAINTEXT에 대한 인자, char*를 담고 있다.
	CCHTA_BACKGROUND,	//STD에 대한 인자 컬러 값 혹은 이미지 파일이 올수 있다.
	CCHTA_COLOR,			//COLOR값, #으로 시작하는 16진수 6자리, 각 2자리가 하나의 색상정보를 표현한다. (#RGB)
	CCHTA_SIZE,			//SIZE값, 정수형()
	CCHTA_ALIGN,			//ALIGN값, 정수형()
	CCHTA_TYPE,			//TYPE값, 정수형()
	CCHTA_SRC,			//SRC값, 문자열
	CCHTA_BOLD,			//BOLD값, 정수형()
	CCHTA_HIGHLIGHT,
	CCHTA_HREF,			//링크 리퍼런스, 문자열
	CCHTA_XMARGIN,		//그림의 X축 여분
	CCHTA_YMARGIN,		//그림의 Y축 여분
}CCHT_ARGUMENT;

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

// 각각의 Text Element에 대한 보조수치들의 값
class CCHyperTextArg {
public:
	CCHT_ARGUMENT		uId;			// 엘리먼트 아규먼트
	
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
	sColor				sColor;

	CCHTA_ColorArg(sColor color) : CCHyperTextArg(Arg){
		sColor = color;
	}
};

class CCHTA_Background : public CCHyperTextArg {
	CCHTA_Background() : CCHyperTextArg(CCHTA_BACKGROUND){
		sColor = sColor(0,0,0);
		szPath = NULL;
	}
public:
	char*				szPath;		//이미지 패스
	sColor				sColor;

	CCHTA_Background(sColor color) : CCHyperTextArg(CCHTA_BACKGROUND){
		szPath = NULL;
		sColor = color;
	}

	CCHTA_Background(char *path) : CCHyperTextArg(CCHTA_BACKGROUND){
		sColor = sColor(0,0,0);
		szPath = strdup(path);
	}

	~CCHTA_Background(){
		if(szPath) free(szPath);	
	}
};




class CCHyperTextElement
{
public:
	CCHT_ELEMENT					nType;	// 엘리먼트의 타입
	CCLinkedList<CCHyperTextArg>	Args;	// 엘리먼트의 인자 리스트

	//생성자, 파괴자
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
	char*			m_pBuffer;			// Text Buffer, CCHyperText는 메모리에 있는 내용만을 파싱한다.
	int				m_nLen;				// 버퍼의 크기
	int				bp;					// Buffer의 포인터
	int				m_nOffset;			// m_szScan의 버퍼포인터
	bool			m_bTagReady;
	char			m_szScan[20480];	// Scan한 값이 저장되는 문자열 포인터, yytext와 유사한 역할을 하는 놈이다.

	void			Gather(char b){ m_szScan[m_nOffset++] = b; }
	
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
		이 클래스는 에러 리포트를 하지 않는다.
		하이퍼 텍스트가 게임내에 적용되었을 때에도 게임은 계속되어야 하므로...

		szTextBuffer : 텍스트 버퍼
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