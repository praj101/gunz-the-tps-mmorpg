#include "stdafx.h"
#include "CCUID.h"
#include <windows.h>


CCUID CCUID::Invalid()
{
	return CCUID(0,0);
}

CCUIDRefMap::CCUIDRefMap()
{
	m_CurrentCCUID.SetZero();
}
CCUIDRefMap::~CCUIDRefMap()
{
}

CCUID CCUIDRefMap::Generate(void* pRef)
{
	m_CurrentCCUID.Increase();
	insert(value_type(m_CurrentCCUID, pRef));

	return m_CurrentCCUID;
}
void* CCUIDRefMap::GetRef(CCUID& uid)
{
	iterator i = find(uid);
	if(i==end()) return NULL;
	return (*i).second;
}

void* CCUIDRefMap::Remove(CCUID& uid)
{
	iterator i = find(uid);
	if(i==end()) return NULL;
	void* pRef = (*i).second;
	erase(i);
	return pRef;
}

CCUIDRANGE CCUIDRefMap::Reserve(int nSize)
{
	CCUIDRANGE r;
	r.Start = m_CurrentCCUID.Increase();
	r.End = m_CurrentCCUID.Increase(nSize-1);
	return r;
}

CCUIDRANGE CCUIDRefMap::GetReservedCount()
{
	CCUIDRANGE r;
	r.Start = CCUID(0, 2);
	r.End = m_CurrentCCUID;
	return r;
}

/*
CCUIDRefArray::CCUIDRefArray()
{
	m_CurrentCCUID.SetZero();
	push_back(NULL);
}
CCUIDRefArray::~CCUIDRefArray()
{
}

CCUID CCUIDRefArray::Generate(void* pRef)
{
	_ASSERT(size()==m_CurrentCCUID.Low);
	_ASSERT(1000*1000<m_CurrentCCUID.Low);	// 1M 보다 작게 설정한다.

	m_CurrentCCUID.Increase();
	push_back(pRef);

	return m_CurrentCCUID;
}
void* CCUIDRefArray::GetRef(CCUID& uid)
{
	if(uid.Low<0 || uid.Low>=size()) return NULL;
	return at(uid.Low);
}
*/

CCUIDRefCache::CCUIDRefCache()
{
}
CCUIDRefCache::~CCUIDRefCache()
{
}


void CCUIDRefCache::Insert(const CCUID& uid, void* pRef)
{
#ifdef _DEBUG
	if (GetRef(uid)) {
		_ASSERT(0);
		OutputDebugString("CCUIDRefCache DUPLICATED Data. \n");
	}
#endif
	insert(value_type(uid, pRef));
}

void* CCUIDRefCache::GetRef(const CCUID& uid)
{
	iterator i = find(uid);
	if(i==end()) return NULL;
	return (*i).second;
}

void* CCUIDRefCache::Remove(const CCUID& uid)
{
	iterator i = find(uid);
	if(i==end()) return NULL;
	void* pRef = (*i).second;
	erase(i);
	return pRef;
}
