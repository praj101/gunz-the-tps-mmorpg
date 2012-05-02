////////////////////////////////////////////////////////////////////////
////2007.01.31
////RAniEventInfo.h
////애니메이션에 추가되어 있는 이벤트에 대한 정보를 담고 있는 파일
////작성자: 홍영진
////////////////////////////////////////////////////////////////////////
#pragma once

#include "CCXml.h"
#include "CCDebug.h"
#include "CCZFileSystem.h"
#include "fileinfo.h"
#include <list>
using namespace std;

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

class RAniEventInfo
{
private:
	char m_cFileName[256];
	int m_iBeginFrame;
	char m_CCEventType[256];
	
public:
	RAniEventInfo(){}
	rvector m_vPos;

	//읽는 부분
	char* GetFileName(){return m_cFileName;}
	int GetBeginFrame(){return m_iBeginFrame;}
	char* GetEventType(){return m_CCEventType;}
	
	//셋팅하는 부분
	void SetFileName(char * filename){ strcpy(m_cFileName, filename); }
	void SetBeginFrame(int BeginFrame){m_iBeginFrame = BeginFrame;}
	void SetEventType(char* EventType){strcpy(m_CCEventType, EventType);}
};

typedef list<RAniEventInfo*> AniNameEventSet;
typedef list<RAniEventInfo*>::iterator AniNameEventSetIter;

//하나의 애니메이션에 담긴 이벤트 정보들
typedef struct _RAniNameEventSet
{
	char					m_cAnimationName[256];
	AniNameEventSet			m_AniNameEventSet;		//애니메이션 이름이 같은 애들끼리 모여있는 셋
	AniNameEventSetIter		m_AniNameEventSetIter;	

	char* GetAnimationName(){return m_cAnimationName;}
	void SetAnimationName(char* AnimationName){strcpy(m_cAnimationName, AnimationName);}
	void Destroy()
	{ 
		while( !(m_AniNameEventSet.empty()) )
		{
			SAFE_DELETE(m_AniNameEventSet.back());
			m_AniNameEventSet.pop_back();
		}
	}

	//애니메이션 이벤트 정보를 꺼내고 셋해주는 부분은 비쥬얼 메쉬에서...
}RAniNameEventSet;


typedef list<RAniNameEventSet*> AniIDEventSet;
typedef list<RAniNameEventSet*>::iterator AniIDEventSetIter;
//하나의 메쉬에 담길 이벤트 셋들..
typedef struct _RAniIDEventSet
{
	int						m_iID;	// npc ID
	AniIDEventSet			m_AniIDEventSet;	// 이벤트 정보가 담길 리스트
	AniIDEventSetIter		m_AniIDEventSetIter;	//이터레이터..	

	int GetID(){return m_iID;}
	void SetID(int id){m_iID = id;}

	RAniNameEventSet* GetAniNameEventSet(char* AnimationName);

	void Destroy()
	{ 
		while( !(m_AniIDEventSet.empty()) )
		{
			m_AniIDEventSet.back()->Destroy();
			SAFE_DELETE(m_AniIDEventSet.back());
			m_AniIDEventSet.pop_back();
		}
	}
	
}RAniIDEventSet;

//모든 이벤트 정보들을 관리하는 클래스

typedef list<RAniIDEventSet*> AniEventMgr;	//각 npc에 연결된 애니메이션들을 관리하는 리스트
typedef list<RAniIDEventSet*>::iterator AniEventMgrIter;

class RAniEventMgr
{
public:
	AniEventMgr				m_AniEventMgr;
	AniEventMgrIter			m_AniEventMgrIter;

	bool ReadXml(CCZFileSystem* pFileSystem, const char* szFileName);
	void ParseAniEvent(::CCXmlElement* PNode, RAniIDEventSet* pAnimIdEventSet);
	RAniIDEventSet* GetAniIDEventSet(int id);
	
	void Destroy()
	{ 
		while( !(m_AniEventMgr.empty()) )
		{
			m_AniEventMgr.back()->Destroy();
			SAFE_DELETE(m_AniEventMgr.back());
			m_AniEventMgr.pop_back();
		}
	}
	RAniEventMgr(){}
	~RAniEventMgr(){ Destroy(); }
};










