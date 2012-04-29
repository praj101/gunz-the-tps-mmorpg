#pragma once
#pragma warning(disable: 4786)

#include <map>
#include <string>
#include <list>

using namespace std;

#include "CCTypes.h"

// 위젯 정보
struct CCWIDGETINFO{
	char	szWidgetClass[256];
	char	szWidgetName[256];
	int		nResourceID;
};

// 위젯 리소스
struct CCWIDGETRESOURCE{
	sRect				Bounds;		// 경계
	bool				bBounds;	// 경계가 바뀌었는가?

	class CCWIDGETINFOLIST : public list<CCWIDGETINFO*>{
	public:
		virtual ~CCWIDGETINFOLIST(void){
			// Delete Automatically
			while(empty()==false){
				delete (*begin());
				erase(begin());
			}
		}
	} Children;	// 자식 위젯 리스트
};

class CCResourceMap : public map<string, CCWIDGETRESOURCE*>{
public:
	virtual ~CCResourceMap(void){
		// Delete Automatically
		while(empty()==false){
			delete (*begin()).second;
			erase(begin());
		}
	}
};
