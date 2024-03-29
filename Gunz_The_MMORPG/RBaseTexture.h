#pragma once
#include "DirectX/D3dx9.h"

#include <string>
#include <map>
#include <algorithm>

#include "RNameSpace.h"

class CCZFileSystem;

using namespace std;

_NAMESPACE_REALSPACE2_BEGIN

// 옵션에 따른 텍스쳐 디테일을 사용하려면~

#define RTextureType_Etc		0
#define RTextureType_Map		1<<1
#define RTextureType_Object		1<<2
#define RTextureType_All		1<<3

/////////////////////////////////////////////////////////
// texture loading base class
// bmp : jpg : tga : dds 지원
// 파일에서의 로딩과 메모리에서의 로딩지원
// 최소기능지원 필요한것은 확장해서 사용~
class RBaseTexture  
{
public:
	RBaseTexture();
	virtual ~RBaseTexture();

	void Destroy();

/*
	bool Create(const char* filename,bool bUseMipmap=false);
	bool Create(void* data,int data_size,bool bUseMipmap=false);
	*/

	void OnInvalidate();
	bool OnRestore(bool bManaged=false);//관리 되기를 원하면 true 로~

	int GetWidth()			{return m_Info.Width; }
	int GetHeight()			{return m_Info.Height;}
	int GetDepth()			{return m_Info.Depth; }
	int GetMipLevels()		{return m_Info.MipLevels;}

	int GetTexLevel()			{ return m_iTexLevel; }
	void SetTexLevel(int level) { m_iTexLevel = level;}

	int GetTexType()			{ return m_iTexType; }
	void SetTexType(int type)	{ m_iTexType = type; }

	D3DFORMAT GetFormat()	{return m_Info.Format;}

	LPDIRECT3DTEXTURE9	GetTexture();

private:

	bool SubCreateTexture();

public:
	bool	m_bManaged;
	DWORD	m_dwLastUseTime;
	char*	m_pTextureFileBuffer;
	int		m_iFileSize;
	char	m_szTextureName[256];
	int		m_iRefCount;
	bool	m_bUseMipmap;
	bool	m_bUseFileSystem;

	int		m_iTexLevel;
	DWORD	m_iTexType;

	DWORD	m_dwColorkey;		// 32bit ARGB (0 to disable)

	D3DXIMAGE_INFO m_Info;
	LPDIRECT3DTEXTURE9 m_pTex;
};


// RTextureManager 

class RTextureManager : public map<string,RBaseTexture*> 
{
public:
	virtual ~RTextureManager();

	void Destroy();

	RBaseTexture *CreateBaseTextureSub(bool mg,const char* filename,int texlevel,bool bUseMipmap=false,bool bUseFileSystem=true,DWORD colorkey=0);
	RBaseTexture *CreateBaseTexture(const char* filename,int texlevel,bool bUseMipmap=false,bool bUseFileSystem=true,DWORD colorkey=0);
	RBaseTexture *CreateBaseTextureMg(const char* filename,int texlevel,bool bUseMipmap=false,bool bUseFileSystem=true,DWORD colorkey=0);

	void DestroyBaseTexture(RBaseTexture*);
	void DestroyBaseTexture(char* szName);

	void OnInvalidate();
	void OnRestore();
	void OnChangeTextureLevel(DWORD flag);

	int UpdateTexture(DWORD max_life_time=5000);
	int CalcUsedSize();
	int CalcAllUsedSize();
	int PrintUsedTexture();
	int CalcUsedCount();
};

void RBaseTexture_Create();
void RBaseTexture_Destory();

void RBaseTexture_Invalidate();
void RBaseTexture_Restore();

RTextureManager* RGetTextureManager();


void SetObjectTextureLevel(int nLevel);
void SetMapTextureLevel(int nLevel);
void SetTextureFormat(int nLevel);	// 0 = 16 bit , 1 = 32bit

int GetObjectTextureLevel();
int GetMapTextureLevel();
int GetTextureFormat();


///////////////////// 다음의 펑션으로 텍스쳐를 만들고 지워주세요

RBaseTexture*	RCreateBaseTexture(const char* filename,DWORD nTexType = RTextureType_Etc,bool bUseMipmap=false,bool bUseFileSystem=true,DWORD colorkey=0);
RBaseTexture*	RCreateBaseTextureMg(const char* filename,DWORD nTexType = RTextureType_Etc,bool bUseMipmap=false,bool bUseFileSystem=true,DWORD colorkey=0);

void			RDestroyBaseTexture(RBaseTexture*);

void			RChangeBaseTextureLevel(DWORD flag);

_NAMESPACE_REALSPACE2_END
