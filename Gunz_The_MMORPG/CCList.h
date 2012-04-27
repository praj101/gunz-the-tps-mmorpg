#pragma once
/* 
	CCList.h
*/

#include <crtdbg.h>
#include <windows.h>

template<class _T>
class CCRecord{
	CCRecord<_T>*	m_lpPrev;
	CCRecord<_T>*	m_lpNext;

public:
	_T*				m_lpContent;

	CCRecord(){
		m_lpPrev	= NULL;
		m_lpNext	= NULL;
		m_lpContent	= NULL;
	}

	void SetPrevPointer(_T* pprev){ m_lpPrev = pprev; };
	CCRecord<_T>* GetPrevPointer(){ return m_lpPrev; };

	void SetNextPointer(_T* nnext){ m_lpNext = nnext; };
	CCRecrod<_T>* GetNextPointer(){ return m_lpNext; };

	_T* Get() { return m_lpContent; };
};


template<class _T>
class CCLinkedList{
private:
	CCRecord<_T>*		m_lpFirstRecord;
	CCRecord<_T>*		m_lpLastRecord;
	CCRecord<_T>*		m_lpCurrentRecord;
	int					m_iTotalRecords;
	int					m_iCurrentPos;

	
public:
	CCLinkedList();
	virtual ~CCLinkedList();
	BOOL InsertHead(_T *lpRecord);
	BOOL AddAfter(_T *lpRecord);
	BOOL Insert(_T *lpRecord);
	BOOL Add(_T *lpRecord);
	void Delete();
	void Delete(int iindex);
	void DeleteAll();
	void DeleteRecord();
	void DeleteRecord(int iindex);
	void DeleteRecordAll();

	int GetCount(){return m_iTotalRecords;}
	int GetIndex(){return m_iCurrentPos;}

	BOOL PrevRecord();
	BOOL NextRecord();
	void MoveRecord(int iindex);
	void MoveFirst();
	void MoveLast();

	_T *Get();
	_T *Get(int iindex);
	void Swap(int nIndex1,int nIndex2);
	virtual int Compare(_T *lpRecord1,_T *lpRecord2){return -10;}
	void Sort();

private:
	void QuickSort(int first,int last);
};

template<class _T>
CCLinkedList<_T>::CCLinkedList()
{
	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_iTotalRecords=0;
	m_iCurrentPos=0;
}

template<class _T>
CCLinkedList<_T>::~CCLinkedList()
{
	DeleteAll();

	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_iTotalRecords=0;
	m_iCurrentPos=0;
}

template<class _T>
BOOL CCLinkedList<_T>::AddAfter(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);
	if(m_iTotalRecords==0){
		CCRecord<_T> *ptemp;
		ptemp=new CCRecord<_T>;
		if(ptemp==NULL)
			return false;
		
		ptemp->m_lpContent=lpRecord;
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCRecord<_T> *ptemp;
		ptemp=new CCRecord<_T>;
		if(ptemp==NULL)
			return false;
		ptemp->m_lpContent=lpRecord;
		ptemp->SetPreviousPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else
			m_lpLastRecord=ptemp;
		
		m_lpCurrentRecord->SetNextPointer(ptemp);
		m_lpCurrentRecord=ptemp;
		m_iCurrentPos++;
	}

	m_iTotalRecords++;
	return false;
}

template<class _T>
BOOL CCLinkedList<_T>::Insert(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	if(m_iTotalRecords==0){
		CCRecord<_T> *ptemp;
		ptemp=new CCRecord<_T>;
		if(ptemp==NULL)
			return false;
		
		ptemp->m_lpContent=lpRecord;		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCRecord<_T> *ptemp;
		ptemp=new CCRecord<_T>;
		if(ptemp==NULL)
			return false;
		ptemp->m_lpContent=lpRecord;
		ptemp->SetPreviousPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else
			m_lpLastRecord=ptemp;

		m_lpCurrentRecord->SetNextPointer(ptemp);
		m_lpCurrentRecord=ptemp;
		m_iCurrentPos++;
	}

	m_iTotalRecords++;
	return true;
}

template<class _T>
BOOL CCLinkedList<_T>::InsertHead(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	MoveFirst();

	if(m_iTotalRecords==0){
		CCRecord<_T> *ptemp;
		ptemp=new CCRecord<_T>;
		if(ptemp==NULL)
			return false;
		ptemp->m_lpContent=lpRecord;		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCRecord<_T> *ptemp;
		ptemp=new CCRecord<_T>;
		if(ptemp==NULL)
			return false;
		ptemp->m_lpContent=lpRecord;
		ptemp->SetPreviousPointer(NULL);
		ptemp->SetNextPointer(m_lpCurrentRecord);
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else
			m_lpLastRecord=ptemp;

		m_lpFirstRecord=ptemp;
		m_lpCurrentRecord=ptemp;

	}

	m_iTotalRecords++;
	return true;
}

template<class _T>
BOOL CCLinkedList<_T>::Add(_T *lpRecord)
{
	MoveLast();
	return Insert(lpRecord);
}

template<class _T>
void CCLinkedList<_T>::Delete()
{
	_ASSERT(m_lpCurrentRecord!=NULL);

	CCRecord<_T>* pprevious;
	CCRecord<_T>* pnext;

	pprevious	=m_lpCurrentRecord->GetPrevPointer();
	pnext		=m_lpCurrentRecord->GetNextPointer();

	if(pprevious!=NULL)
		pprevious->SetNextPointer(pnext);
	else
		m_lpFirstRecord=pnext;
	
	if(pnext!=NULL)
		pnext->SetPreviousPointer(pprevious);
	else
		m_lpLastRecord=pprevious;

	_ASSERT(m_lpCurrentRecord->m_lpContent!=NULL);
	delete m_lpCurrentRecord->m_lpContent;
	delete m_lpCurrentRecord;

	if(pnext!=NULL)
		m_lpCurrentRecord=pnext;
	else{
		if(pprevious!=NULL){
			m_lpCurrentRecord=pprevious;
			m_iCurrentPos--;
			_ASSERT(m_iCurrentPos>=0);
		}
		else
			m_lpCurrentRecord=NULL;
	}

	m_iTotalRecords--;
}

template<class _T>
void CCLinkedList<_T>::Delete(int iindex)
{
	if(iindex<0 || iindex>=m_iTotalRecords) {
		_ASSERT(false);
		return ;
	}

	MoveRecord(iindex);
	Delete();
}

template<class _T>
void CCLinkedList<_T>::DeleteAll()
{
	while(m_iTotalRecords!=0)
		Delete();
	m_iCurrentPos=0;
}

template<class _T>
void CCLinkedList<_T>::DeleteRecord()
{
	_ASSERT(m_lpCurrentRecord!=NULL);

	CCRecord<_T>* pprevious;
	CCRecord<_T>* pnext;

	pprevious	=m_lpCurrentRecord->GetPrevPointer();
	pnext		=m_lpCurrentRecord->GetNextPointer();

	if(pprevious!=NULL)
		pprevious->SetNextPointer(pnext);
	else
		m_lpFirstRecord=pnext;
	
	if(pnext!=NULL)
		pnext->SetPreviousPointer(pprevious);
	else
		m_lpLastRecord=pprevious;

	delete m_lpCurrentRecord;

	if(pnext!=NULL)
		m_lpCurrentRecord=pnext;
	else{
		if(pprevious!=NULL){
			m_lpCurrentRecord=pprevious;
			m_iCurrentPos--;
			_ASSERT(m_iCurrentPos>=0);
		}
		else
			m_lpCurrentRecord=NULL;
	}

	m_iTotalRecords--;
}

template<class _T>
void CCLinkedList<_T>::DeleteRecord(int iindex)
{
	if(iindex<0 || iindex>=m_iTotalRecords) {
		_ASSERT(false);
		return;
	}

	MoveRecord(iindex);
	DeleteRecord();
}

template<class _T>
void CCLinkedList<_T>::DeleteRecordAll()
{
	while(m_iTotalRecords!=0)
		DeleteRecord();
	m_iCurrentPos=0;
}


template<class _T>
BOOL CCLinkedList<_T>::PrevRecord()
{
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetPrevPointer())!=NULL){
		m_iCurrentPos--;
		_ASSERT(m_iCurrentPos>=0);

		m_lpCurrentRecord=m_lpCurrentRecord->GetPrevPointer();
		return true;
	}	
	else
		return false;
}

template<class _T>
BOOL CCLinkedList<_T>::NextRecord()
{
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetNextPointer())!=NULL){
		m_iCurrentPos++;
		_ASSERT(m_iCurrentPos<m_iTotalRecords);

		m_lpCurrentRecord=m_lpCurrentRecord->GetNextPointer();
		return true;
	}	
	else
		return false;
}

template<class _T>
void CCLinkedList<_T>::MoveRecord(int iindex)
{
	if(iindex<0 || iindex>=m_iTotalRecords) {
		_ASSERT(false);
		return;
	}
	_ASSERT(iindex>=0);
	if(iindex==0)
	{	
		MoveFirst();
		return;
	}
	if(iindex>m_iCurrentPos)
		while(iindex!=m_iCurrentPos)
			NextRecord();
	else if(iindex<m_iCurrentPos)
		while(iindex!=m_iCurrentPos)
			PrevRecord();
}

template<class _T>
void CCLinkedList<_T>::MoveFirst()
{
	m_iCurrentPos=0;
	m_lpCurrentRecord=m_lpFirstRecord;
}

template<class _T>
void CCLinkedList<_T>::MoveLast()
{
	if(m_iTotalRecords>0){
		m_iCurrentPos=m_iTotalRecords-1;
		m_lpCurrentRecord=m_lpLastRecord;
	}
}

template<class _T>
_T *CCLinkedList<_T>::Get()
{
	_ASSERT(m_lpCurrentRecord!=NULL);

	return(m_lpCurrentRecord->Get());
}

template<class _T>
_T *CCLinkedList<_T>::Get(int iindex)
{
	if(iindex<0 || iindex>=m_iTotalRecords) {
		_ASSERT(false);
		return NULL;
	}

	MoveRecord(iindex);
	return Get();
}

template<class _T>
void CCLinkedList<_T>::Sort()
{
	if(GetCount()<=1)return;
	QuickSort(0,GetCount()-1);
}

template<class _T>
void CCLinkedList<_T>::QuickSort(int first,int last)
{
	int i,j;

	i=first,j=last;
	_T *pMiddle=Get((first+last)/2);

	_ASSERT(Compare(Get(i),pMiddle)!=-10);

	for(;;){
		while(Compare(Get(i),pMiddle)<0)i++;
		while(Compare(Get(j),pMiddle)>0)j--;
		if(i>=j)break;
		Swap(i,j);
		i++;j--;
	}
	if(first<i-1)QuickSort(first,i-1);
	if(j+1<last)QuickSort(j+1,last);
}

template<class _T>
void CCLinkedList<_T>::Swap(int nIndex1,int nIndex2)
{
	if(nIndex1<0 || nIndex1>=m_iTotalRecords) {
		_ASSERT(false);
		return;
	}

	MoveRecord(nIndex1);
	CCRecord<_T>* pRecord1=m_lpCurrentRecord;


	if(nIndex2<0 || nIndex2>=m_iTotalRecords) {
		_ASSERT(false);
		return;
	}

	MoveRecord(nIndex2);
	CCRecord<_T>* pRecord2=m_lpCurrentRecord;
	_T *pData=pRecord1->m_lpContent;
	pRecord1->m_lpContent=pRecord2->m_lpContent;
	pRecord2->m_lpContent=pData;
}