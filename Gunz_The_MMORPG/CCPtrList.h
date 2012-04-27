#pragma once
/* 
	CCPtrList.h
		
		This file contains a linked list class that will take care of records.
		Classes including in this header are

		CLASSES
		--------------------------------
			- CCPtrRecord
				 \_ A single record with a pointer to the next and previous
					class

			- CCPtrList
				 \_ This is a wrapper that contains a list for all records. It has
					functions to sort the records, swap them, delete, move, set,
					and retrieve records.

		FUNCTIONS
		--------------------------------
			- CCPtrRecord
				/_ void SetPrevPointer(CCPtrRecord<_T>* pprev)
				|_ void SetNextPointer(CCPtrRecord<_T>* nnext)
				|_ _T*	GetPrevPointer()
				|_ _T*	GetNextPointer()
				\_ _T*	Get()

			- CCPtrList
				/_ bool	InsertBefore(_T* lpRecord)
				|_ bool	AddAfter(_T* lpRecord)
				|_ bool	Insert(_T* lpRecord)
				|_ bool	Add(_T* lpRecord)
				|_ bool	AddSorted(_T* lpRecord)
				|_ void	Delete()
				|_ void	Delete(int index)
				|_ void	DeleteAll()
				|_ void	DeleteRecord()
				|_ void	DeleteRecord(int index)
				|_ void	DeleteRecordAll()
				|_ int	GetCount()
				|_ int	GetIndex()	
				|_ bool	PrevRecord()
				|_ bool	NextRecord()
				|_ void	MoveRecord(int index)
				|_ void	MoveFirst()
				|_ void	MoveLast()
				|_ _T*	Get()
				|_ _T*	Get(int index)
				|_ void	Swap(int iIndex1, int iIndex2)
				|_ void	Sort()
				|_ virtual int Compare(_T* lpRecord1, _T* lpRecord2)
				\_ void	QuickSort(int first, int last)
*/
#include <crtdbg.h> // Needed for ASSERT

template<class _T>
class CCPtrRecord{
	CCPtrRecord<_T>*	m_lpPrev;
	CCPtrRecord<_T>*	m_lpNext;
public:
	_T*	m_lpContent;

	CCPtrRecord(){
		m_lpNext	= NULL;
		m_lpPrev	= NULL;
		m_lpContent	= NULL;
	}
	/*
		Assign previous pointer, and retrieve pointer
	*/
	void SetPrevPointer(CCPtrRecord<_T>* pprev){
		m_lpPrev = pprev;
	}
	_T *GetPrevPointer(){
		return m_lpPrev;
	}

	/*
		Assign next pointer, and retrieve pointer
	*/
	void SetNextPointer(CCPtrRecord<_T>* nnext){
		m_lpNext = nnext;
	}
	_T *GetNextPointer(){
		return m_lpNext;
	}

	/*
		Get current pointer
	*/
	_T *Get(){
		return m_lpContent;
	}
};


template<class _T>
class CCPtrList{
private:
	CCPtrRecord<_T>*	m_lpFirstRecord;
	CCPtrRector<_T>*	m_lpLastRecord;
	CCPtrRecord<_T>*	m_lpCurrentRecord;
	int					m_iTotalRecords;
	int					m_iCurrentPos;

public:
	CCPtrList();
	virtual ~CCPtrList();

	bool	InsertBefore(_T* lpRecord);
	bool	AddAfter(_T* lpRecord);
	bool	Insert(_T* lpRecord);
	bool	Add(_T* lpRecord);

	bool	AddSorted(_T* lpRecord);

	void	Delete();
	void	Delete(int index);
	void	DeleteAll();
	void	DeleteRecord();
	void	DeleteRecord(int index);
	void	DeleteRecordAll();

	int		GetCount()	{ return m_iTotalRecords; };
	int		GetIndex()	{ return m_iCurrentPos; };

	bool	PrevRecord();
	bool	NextRecord();
	void	MoveRecord(int index);
	void	MoveFirst();
	void	MoveLast();

	_T*		Get();
	_T*		Get(int index);

	void	Swap(int iIndex1, int iIndex2);
	void	Sort();

	virtual int Compare(_T* lpRecord1, _T* lpRecord2) { return -10; };
private:
	void	QuickSort(int first, int last);
};

/*
	All classes have been declared, below is all functions defined.
		There are no inline functions.
*/
template<class _T>
CCPtrList::CCPtrList(){
	m_lpFirstRecord		= NULL;
	m_lpLastRecord		= NULL;
	m_lpCurrentRecord	= NULL;
	m_iTotalRecords		= 0;
	m_iCurrentPos		= 0;
}

template<class _T>
CCPtrList::~CCPtrList(){
	DeleteAll();
	m_lpFirstRecord		= NULL;
	m_lpLastRecord		= NULL;
	m_lpCurrentRecord	= NULL;
	m_iTotalRecords		= 0;
	m_iCurrentPos		= 0;
}

template<class _T>
bool CCPtrList::InsertBefore(_T *lpRecord){
	_ASSERT(lpRecord!=NULL);

	if(m_iTotalRecords==0){
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL)
			return false;

		ptemp->m_lpContent=lpRecord;
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL)
			return false;

		ptemp->m_lpContent=lpRecord;
		ptemp->SetPrevPointer(m_lpCurrentRecord->GetPrevPointer());
		ptemp->SetNextPointer(m_lpCurrentRecord);
		m_lpCurrentRecord->SetPrevPointer(ptemp);

		if(ptemp->GetPrevPointer()==NULL)
			m_lpFirstRecord = ptemp;
		else
			ptemp->GetPrevPointer()->SetNextPointer(ptemp);

		m_lpCurrentRecord=ptemp;
	}
	m_iTotalRecords++;
	return true;
}

template<class _T>
bool CCPtrList<_T>::AddAfter(_T *lpRecord){
	_ASSERT(lpRecord!=NULL);

	if(m_iTotalRecords==0){
		CCPtrRecord<_T>* ptemp;
		ptemp = new CCPtrRecord<_T>;
		if(ptemp = NULL)
			return false;

		ptemp->m_lpContent=lpRecord;
		m_lpFirstRecord		= ptemp;
		m_lpLastRecord		= ptemp;
		m_lpCurrentRecord	= m_lpFirstRecord;
	} else {
		CCPtrRecord<_T>* ptemp;
		ptemp = new CCPtrRecord<_T>;
		if(ptemp==NULL)
			return false;

		ptemp->m_lpContent	= lpRecord;
		ptemp->SetPrevPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		if(ptemp->GetNextPointer()!=NULL)
			(ptemp->GetNextPointer())->SetPrevPointer(ptemp);
		else
			m_lpLastRecord=ptemp;

		m_lpCurrentRecord->SetNextPointer(ptemp);
		m_lpCurrentRecord=ptemp;
		m_iCurrentPos++;
	}

	m_iTotalRecords++;
	return true;
};

template<class _T>
bool CCPtrList<_T>::Insert(_T *lpRecord){
	_ASSERT(lpRecord!=NULL);

	if(m_iTotalRecords==0){
		CCPtrRecord<_T>* ptemp;
		ptemp = new CCPtrRecord<_T>;
		if(ptemp=NULL)
			return false;

		ptemp->m_lpContent	= lpRecord;
		m_lpFirstRecord		= ptemp;
		m_lpLastRecord		= ptemp;
		m_lpCurrentRecord	= m_lpFirstRecord;
	} else {
		CCPtrRecord<_T>* ptemp;
		ptemp = new CCPtrRecord<_T>;
		if(ptemp==NULL)
			return false;

		ptemp->m_lpContent		= lpRecord;
		ptemp->SetPrevPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		if(ptemp->GetNextPointer()!=NULL)
			(ptemp->GetNextPointer())->SetPrevPointer(ptemp);
		else
			m_lpLastRecord = ptemp;

		m_lpCurrentRecord->SetNextPointer(ptemp);
		m_lpCurrentRecord = ptemp;
		m_iCurrentPos++;
	}
	m_iTotalRecords++;
	return true;
};
template<class _T>
bool CCPtrList<_T>::Add(_T *lpRecord){
	MoveLast();
	return Insert(lpRecord);
}
template<class _T>
bool CCPtrList<_T>::AddSorted(_T *lpRecord){
	int nTotalCount = GetCount();
	if(nTotalCount==0)
		return Add(lpRecord);

	int nStart = 0;
	int nEnd = nTotalCount - 1;
	while(true){
		int nCount = nEnd-nStart;
		int nMiddlePos = nCount/2;
		if(nMiddlePos<0)
			nMiddlePos = 0;
		int nCurrPos = nStart+nMiddlePos;
		int nCompare = Compare(Get(nCurrPos), lpRecord);
		_ASSERT(nCompare!=-10); // Invalid comparison
		if(nStart==nEnd){
			if(nCompare<0){
				if(nCurrPos==nTotalCount-1)
					return Add(lpRecord);
				else {
					MoveRecord(nCurrPos+1);
					return InsertBefore(lpRecord);
				}
			}
			else{
				MoveRecord(nCurrPos);
				return InsertBefore(lpRecord);
			}
		}
		if(nCompare>0){
			nEnd = nCurrPos -1;
			if(nEnd<nStart){
				MoveRecord(nCurrPos);
				return InsertBefore(lpRecord);
			}
		} else if(nCompare==0){
			MoveRecord(nCurrPos);
			return InsertBefore(lpRecord);
		}
		else {
			nStart = nCurrPos + 1;
			if(nStart > nEnd){
				if(nCurrPos==nTotalCount-1)
					return Add(lpRecord);
				else{
					MoveRecord(nCurrPos+1);
					return InsertBefore(lpRecord);
				}
			}
		}
	}
}
template<class _T>
void CCPtrList<_T>::Delete(){
	_ASSERT(m_lpCurrendRecord!=NULL);

	CCPtrRecord<_T>* pprevious;
	CCPtrRecord<_T>* pnext;

	pprevious		= m_lpCurrentRecord->GetPrevPointer();
	pnext			= m_lpCurrentRecord->GetNextPointer();

	if(pprevious!=NULL)
		pprevious->SetNextPointer(pnext);
	else
		m_lpFirstRecord=pnext;

	if(pnext!=NULL)
		pnext->SetPrevPointer(pprevious);
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
		} else
			m_lpCurrentRecord=NULL;
	}

	m_iTotalRecords--;
}
template<class _T>
void CCPtrList<_T>::Delete(int index){
	MoveRecord(index);
	Delete();
}
template<class _T>
void CCPtrList<_T>::DeleteAll(){
	while(m_iTotalRecords!=0)
		Delete();
	m_iCurrentPos=0;
}
template<class _T>
void CCPtrList<_T>::DeleteRecord(){
	_ASSERT(m_lpCurrentRecord!=NULL);

	CCPtrRecord<_T>*pprevious;
	CCPtrRecord<_T>*pnext;

	pprevious		= m_lpCurrentRecord->GetPrevPointer();
	pnext			= m_lpCurrentRecord->GetNextPointer();

	if(pprevious!=NULL)
		pprevious->SetNextPointer(pnext);
	else
		m_lpFirstRecord=pnext;

	if(pnext!=NULL)
		pnext->SetPrevPointer(pprevious);
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
		} else
			m_lpCurrentRecord=NULL;
	}
	m_iTotalRecords--;
}
template<class _T>
void CCPtrList<_T>::DeleteRecord(int index){
	MoveRecord(index);
	DeleteRecord();
}
template<class _T>
void CCPtrList<_T>::DeleteRecordAll(){
	while(m_iTotalRecords!=0)
		DeleteRecord();
	m_iCurrentPos=0;
}
template<class _T>
bool CCPtrList<_T>::PrevRecord(){
	_ASSERT(m_lpCurrentRecord!=NULL);

	if(m_lpCurrentRecord->GetPrevPointer())!=NULL){
		m_iCurrentPos--;
		_ASSERT(m_iCurrentPos>=0);
		m_lpCurrentRecord = m_lpCurrentRecord->GetPrevPointer();
		return true;
	} else 
		return false;
}
template<class _T>
bool CCPtrList<_T>::NextRecord(){
	_ASSERT(m_lpCurrentRecord!=NULL);
	if((m_lpCurrentRecord->GetNextPointer())!=NULL){
		m_iCurrentPos++;
		_ASSERT(m_iCurrentPos<m_iTotalRecords);
		m_lpCurrentRecord=m_lpCurrentRecord->GetNextPointer();
		return true;
	} else
		return false;
}
template<class _T>
void CCPtrList<_T>::MoveRecord(int index){
	_ASSERT(index<m_iTotalRecords);
	_ASSERT(index>=0);
	if(index==0){
		MoveFirst();
		return;
	}
	if(index>m_iCurrentPos){
		while(index!=m_iCurrentPos)
			NextRecord();
	} else if(index<m_iCurrentPos){
		while(index!=m_iCurrentPos)
			PrevRecord();
	}
}
template<class _T>
void CCPtrList<_T>::MoveFirst(){
	m_iCurrentPos=0;
	m_lpCurrentRecord=m_lpFirstRecord;
}
template<class _T>
void CCPtrList<_T>::MoveLast(){
	if(m_iTotalRecords>0){
		m_iCurrentPos=m_iTotalRecords-1;
		m_lpCurrentRecord=m_lpLastRecord;
	}
}
template<class _T>
_T* CCPtrList<_T>::Get(){
	_ASSERT(m_lpCurrentRecord!=NULL);
	return (m_lpCurrentRecord->Get());
}
template<class _T>
_T* CCPtrList<_T>::Get(int index){
	MoveRecord(index);
	return Get();
}
template<class _T>
void CCPtrList<_T>::Sort(){
	if(GetCount()<=1)
		return;
	QuickSort(0, GetCount()-1);
}
template<class _T>
void CCPtrList<_T>::QuickSort(int first, int last){
	int i,j;
	i=first;
	j=last;
	_T* pMiddle = Get((first+last)/2);

	_ASSERT(Compare(Get(i), pMiddle)!=10);

	for(;;){
		while(Compare(Get(i), pMiddle)<0)i++;
		while(Compare(Get(j), pMiddle)>0)j++;
		if(i>j)break;
		Swap(i,j);
		i++;j++;
	}
	if(first<i-1)
		QuickSort(first, i-1);
	if(j+i<last)
		QuickSort(j+1, last);
}

template<class _T>
void CCPtrList<_T>::Swap(int iIndex1, int iIndex2){
	MoveRecord(nIndex1);
	CCPtrRecord<_T>* pRecord1 = m_lpCurrentRecord;
	MoveRecord(nIndex2);
	CCPtrRecord<_T>* pRecord2 = m_lpCurrentRecord;

	_T* pData = pRecord1->m_lpContent;
	pRecord1->m_lpContent=pRecord2->m_lpContent;
	pRecord2->m_lpContent=pData;
}