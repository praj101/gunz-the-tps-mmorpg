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
#include <crtdbg.h>			// _ASSERT

// 리스트를 구성할 레코드
template<class _T>
class CCPtrRecord{
	CCPtrRecord<_T>*	m_lpPrevious;
	CCPtrRecord<_T>*	m_lpNext;
public:
	_T*			m_lpContent;

	// 멤버 초기화
	CCPtrRecord(){
		m_lpNext		=NULL;
		m_lpPrevious	=NULL;
		m_lpContent		=NULL;
	}
	// 이전 노드 세팅
	void SetPreviousPointer(CCPtrRecord<_T>* pprevious){
		m_lpPrevious=pprevious;
	}
	// 이전 노드 얻기
	CCPtrRecord<_T>* GetPreviousPointer(){
		return m_lpPrevious;
	}
	// 다음 노드 세팅
	void SetNextPointer(CCPtrRecord<_T>* pnext){
		m_lpNext=pnext;
	}
	// 다음 노드 얻기
	CCPtrRecord<_T>* GetNextPointer(){
		return m_lpNext;
	}
	// 내용 얻기
	_T *Get(){
		return m_lpContent;
	}
	/*
	레퍼런스는 모호함을 유발함으로, 사용하지 않는다.
	_T &Get(){
		return *m_lpContent;
	}
	*/
};

// 리스트 클래스
template<class _T>
class CCPtrList{
private:
	CCPtrRecord<_T>*		m_lpFirstRecord;	// 첫번째 레코드
	CCPtrRecord<_T>*		m_lpLastRecord;		// 첫번째 레코드
	CCPtrRecord<_T>*		m_lpCurrentRecord;	// 현재 레코드
	int						m_nListCount;		// 레코드의 개수
	int						m_nCurrentPosition;	// 현재 위치

	//MString					m_lpszErrorString;	// 에러 스트링
public:
	// 변수 초기화
	CCPtrList();
	// 리스트 해제(알아서 레코드를 모두 삭제한다)
	virtual ~CCPtrList();

	// 에러 메세지 얻기
	//MString GetErrorMessage(){return m_lpszErrorString;}

	// 현재 위치 이전에 레코드 삽입(new로 생성된 객체가 그대로 박힌다.)
	bool InsertBefore(_T *lpRecord);
	// 현재 위치 다음에 레코드 삽입(new로 생성된 객체가 그대로 박힌다.)
	bool AddAfter(_T *lpRecord);
	// 현재 위치 다음에 레코드 삽입(new로 생성된 객체가 그대로 박힌다.)
	bool Insert(_T *lpRecord);
	// 맨 뒤에 레코드 추가(new로 생성된 객체가 그대로 박힌다.)
	bool Add(_T *lpRecord);
	// 소트해 가면서 추가(단, Compare루틴이 있어야 한다.)
	bool AddSorted(_T *lpRecord);
	
	// 현재 위치의 레코드를 삭제
	void Delete();
	// 지정된 위치의 레코드를 삭제
	void Delete(int iindex);
	// 모든 레코드 삭제
	void DeleteAll();

	// Added by Kim young ho
	// 현재 위치의 레코드를 삭제, but Record의 content는 삭제하지 않음 
	void DeleteRecord();
	// 지정된 위치의 레코드를 삭제, but Record의 content는 삭제하지 않음 
	void DeleteRecord(int iindex);
	// 모든 레코드 삭제, but Record의 content는 삭제하지 않음 
	void DeleteRecordAll();

	// 레코드의 개수 얻어내기
	int GetCount(){return m_nListCount;}
	// 레코드의 현재 위치 얻어내기
	int GetIndex(){return m_nCurrentPosition;}

	////////////////////////////////////////////
	// 리스트내의 포인터를 이용한 이동및 값 얻기
	// 이전 레코드로
	bool PreviousRecord();
	// 다음 레코드로
	bool NextRecord();
	// 지정된 위치로
	void MoveRecord(int iindex);
	// 맨 처음으로
	void MoveFirst();
	// 맨 마지막으로
	void MoveLast();

	// 현재 레코드의 내용을 얻어낸다.
	_T *Get();

	// 지정된 인덱스의 값을 얻어낸다.
	_T *Get(int iindex);

	// 지정된 인덱스의 값을 얻어낸다.
	// 레퍼런스에 의한 모호함으로 사용하지 않는다.
	//_T &operator[](int iindex);

	// 두개의 레코드의 값을 Swap한다.
	void Swap(int nIndex1,int nIndex2);

	// Record Compare Function. Sort를 위해 virtual로 계승받아야 한다.
	virtual int Compare(_T *lpRecord1,_T *lpRecord2){return -10;/*false*/}

	// Quick Sort를 실행한다.
	void Sort();
private:
	// Quick Sort의 서브 함수
	void QuickSort(int first,int last);
};

// 변수 초기화
template<class _T>
CCPtrList<_T>::CCPtrList()
{
	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_nListCount=0;
	m_nCurrentPosition=0;
}

// 리스트 해제(알아서 레코드를 모두 삭제한다)
template<class _T>
CCPtrList<_T>::~CCPtrList()
{
	DeleteAll();

	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_nListCount=0;
	m_nCurrentPosition=0;
}

template<class _T>
bool CCPtrList<_T>::InsertBefore(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// 리스트의 첫 레코드이면
	if(m_nListCount==0){
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL){
			//m_lpszErrorString="CCPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// 단순히 데이타 포인터 대입. 데이타는 기존에 동적으로 할당되어 있어야 한다.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		// 현재 레코드과 다음 레코드 사이에 삽입
		// 새 레코드
		ptemp->SetPreviousPointer(m_lpCurrentRecord->GetPreviousPointer());
		ptemp->SetNextPointer(m_lpCurrentRecord);
		// 다음 레코드

		// 현재(앞) 레코드
		m_lpCurrentRecord->SetPreviousPointer(ptemp);

		if(ptemp->GetPreviousPointer()==NULL)
			m_lpFirstRecord = ptemp;
		else
			ptemp->GetPreviousPointer()->SetNextPointer(ptemp);

		// 현재 포인터를 새로 생성한 레코드으로 대치
		m_lpCurrentRecord=ptemp;
	}

	// 리스트 카운트 증가
	m_nListCount++;
	return true;
}

// 레코드 삽입
template<class _T>
bool CCPtrList<_T>::AddAfter(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// 리스트의 첫 레코드이면
	if(m_nListCount==0){
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL){
			//m_lpszErrorString="CCPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// 단순히 데이타 포인터 대입. 데이타는 기존에 동적으로 할당되어 있어야 한다.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		// 현재 레코드과 다음 레코드 사이에 삽입
		// 새 레코드
		ptemp->SetPreviousPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		// 다음 레코드
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else{
			// 마지막에 추가하는 것이므로
			m_lpLastRecord=ptemp;
		}

		// 현재(앞) 레코드
		m_lpCurrentRecord->SetNextPointer(ptemp);

		// 현재 포인터를 새로 생성한 레코드으로 대치
		m_lpCurrentRecord=ptemp;

		m_nCurrentPosition++;
	}

	// 리스트 카운트 증가
	m_nListCount++;
	return true;
}

// 레코드 삽입
template<class _T>
bool CCPtrList<_T>::Insert(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// 리스트의 첫 레코드이면
	if(m_nListCount==0){
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL){
			//m_lpszErrorString="CCPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// 단순히 데이타 포인터 대입. 데이타는 기존에 동적으로 할당되어 있어야 한다.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		// 현재 레코드과 다음 레코드 사이에 삽입
		// 새 레코드
		ptemp->SetPreviousPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		// 다음 레코드
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else{
			// 마지막에 추가하는 것이므로
			m_lpLastRecord=ptemp;
		}

		// 현재(앞) 레코드
		m_lpCurrentRecord->SetNextPointer(ptemp);

		// 현재 포인터를 새로 생성한 레코드으로 대치
		m_lpCurrentRecord=ptemp;

		m_nCurrentPosition++;
	}

	// 리스트 카운트 증가
	m_nListCount++;
	return true;
}

// 레코드 추가
template<class _T>
bool CCPtrList<_T>::Add(_T *lpRecord)
{
	MoveLast();
	return Insert(lpRecord);
}

template<class _T>
bool CCPtrList<_T>::AddSorted(_T *lpRecord)
{
	int nTotalCount = GetCount();
	if(nTotalCount==0) return Add(lpRecord);

	int nStart = 0;
	int nEnd = nTotalCount - 1;
	while(1){
		int nCount = nEnd - nStart;
		int nMiddlePos = nCount / 2;
		if(nMiddlePos<0) nMiddlePos = 0;
		int nCurrPos = nStart+nMiddlePos;
		int nCompare = Compare(Get(nCurrPos), lpRecord);
		_ASSERT(nCompare!=-10);	// Invalide Comparision
		if(nStart==nEnd){
			if(nCompare<0){
				if(nCurrPos==nTotalCount-1) return Add(lpRecord);
				else{
					MoveRecord(nCurrPos+1);
					return InsertBefore(lpRecord);
				}
			}
			else{
				MoveRecord(nCurrPos);
				return InsertBefore(lpRecord);
			}
		}

		if( nCompare > 0 ){
			nEnd = nCurrPos - 1;
			if(nEnd<nStart){
				MoveRecord(nCurrPos);
				return InsertBefore(lpRecord);
			}
		}
		else if( nCompare == 0 ){
			MoveRecord(nCurrPos);
			return InsertBefore(lpRecord);
		}
		else{
			nStart = nCurrPos + 1;
			if(nStart>nEnd){
				if(nCurrPos==nTotalCount-1) return Add(lpRecord);
				else{
					MoveRecord(nCurrPos+1);
					return InsertBefore(lpRecord);
				}
			}
		}
	}
}

// 현재 위치의 레코드를 삭제
template<class _T>
void CCPtrList<_T>::Delete()
{
	// 현재 레코드의 포인터는 NULL일수 없다.
	_ASSERT(m_lpCurrentRecord!=NULL);

	CCPtrRecord<_T>* pprevious;
	CCPtrRecord<_T>* pnext;

	pprevious	=m_lpCurrentRecord->GetPreviousPointer();
	pnext		=m_lpCurrentRecord->GetNextPointer();

	// 앞 레코드
	if(pprevious!=NULL){
	// 앞 레코드가 처음이 아니면...
		pprevious->SetNextPointer(pnext);
	}
	else{
	// 앞 레코드가 첫 레코드일 경우
		m_lpFirstRecord=pnext;
	}
	
	// 뒤 레코드
	if(pnext!=NULL){
	// 이 레코드가 마지막이 아니면...
		pnext->SetPreviousPointer(pprevious);
	}
	else{
	// 뒤 레코드가 마지막이면
		m_lpLastRecord=pprevious;
	}

	// 현재 레코드의 내용 메모리에서 삭제
	//_ASSERT(m_lpCurrentRecord->m_lpContent!=NULL);
	//delete m_lpCurrentRecord->m_lpContent;
	// 현재 레코드 메모리에서 삭제
	delete m_lpCurrentRecord;

	// 현재 포인터 지정
	if(pnext!=NULL)
		m_lpCurrentRecord=pnext;
	else{
		if(pprevious!=NULL){
			m_lpCurrentRecord=pprevious;
			m_nCurrentPosition--;
			_ASSERT(m_nCurrentPosition>=0);
		}
		else
			m_lpCurrentRecord=NULL;
	}

	// 리스트 카운트 감소
	m_nListCount--;
}

// 원하는 인덱스를 지움
template<class _T>
void CCPtrList<_T>::Delete(int iindex)
{
	MoveRecord(iindex);
	Delete();
}

// 모든 레코드 삭제
template<class _T>
void CCPtrList<_T>::DeleteAll()
{
	while(m_nListCount!=0)
		Delete();
	m_nCurrentPosition=0;
}

// 현재 위치의 레코드를 삭제, but Record의 content는 삭제하지 않음  
template<class _T>
void CCPtrList<_T>::DeleteRecord()
{
	// 현재 레코드의 포인터는 NULL일수 없다.
	_ASSERT(m_lpCurrentRecord!=NULL);

	CCPtrRecord<_T>* pprevious;
	CCPtrRecord<_T>* pnext;

	pprevious	=m_lpCurrentRecord->GetPreviousPointer();
	pnext		=m_lpCurrentRecord->GetNextPointer();

	// 앞 레코드
	if(pprevious!=NULL){
	// 앞 레코드가 처음이 아니면...
		pprevious->SetNextPointer(pnext);
	}
	else{
	// 앞 레코드가 첫 레코드일 경우
		m_lpFirstRecord=pnext;
	}
	
	// 뒤 레코드
	if(pnext!=NULL){
	// 이 레코드가 마지막이 아니면...
		pnext->SetPreviousPointer(pprevious);
	}
	else{
	// 뒤 레코드가 마지막이면
		m_lpLastRecord=pprevious;
	}

	// 현재 레코드 메모리에서 삭제
	delete m_lpCurrentRecord;

	// 현재 포인터 지정
	if(pnext!=NULL)
		m_lpCurrentRecord=pnext;
	else{
		if(pprevious!=NULL){
			m_lpCurrentRecord=pprevious;
			m_nCurrentPosition--;
			_ASSERT(m_nCurrentPosition>=0);
		}
		else
			m_lpCurrentRecord=NULL;
	}

	// 리스트 카운트 감소
	m_nListCount--;
}

// 원하는 인덱스를 지움, but Record의 content� 삭제하지 않음 
template<class _T>
void CCPtrList<_T>::DeleteRecord(int iindex)
{
	MoveRecord(iindex);
	DeleteRecord();
}

// 모든 레코드 삭제, but Record의 content는 삭제하지 않음 
template<class _T>
void CCPtrList<_T>::DeleteRecordAll()
{
	while(m_nListCount!=0)
		DeleteRecord();
	m_nCurrentPosition=0;
}


// 이전 레코드로
template<class _T>
bool CCPtrList<_T>::PreviousRecord()
{
	// 현재 레코드의 포인터는 NULL일수 없다.
	// 즉, 아무것도 저장되어 있지 않은 상태
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetPreviousPointer())!=NULL){
		m_nCurrentPosition--;
		_ASSERT(m_nCurrentPosition>=0);

		m_lpCurrentRecord=m_lpCurrentRecord->GetPreviousPointer();
		return true;
	}	
	else{
		//m_lpszErrorString="CCPtrList::PreviousRecord() - Out of Range";
		return false;
	}
}

// 다음 레코드로
template<class _T>
bool CCPtrList<_T>::NextRecord()
{
	// 현재 레코드의 포인터는 NULL일수 없다.
	// 즉, 아무것도 저장되어 있지 않은 상태
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetNextPointer())!=NULL){
		m_nCurrentPosition++;
		_ASSERT(m_nCurrentPosition<m_nListCount);

		m_lpCurrentRecord=m_lpCurrentRecord->GetNextPointer();
		return true;
	}	
	else{
		//m_lpszErrorString="CCPtrList::NextRecord() - Out of Range";
		return false;
	}
}

// 지정된 위치로
template<class _T>
void CCPtrList<_T>::MoveRecord(int iindex)
{
	// 인덱스는 지정된 범위안에 있어야 한다.
	_ASSERT(iindex<m_nListCount);
	_ASSERT(iindex>=0);
	
	if(iindex==0)
	{
		MoveFirst();
		return;
	}
	if(iindex>m_nCurrentPosition){
		while(iindex!=m_nCurrentPosition)
			NextRecord();
	}
	else if(iindex<m_nCurrentPosition){
		while(iindex!=m_nCurrentPosition)
			PreviousRecord();
	}
}

// 맨 처음으로
template<class _T>
void CCPtrList<_T>::MoveFirst()
{
	m_nCurrentPosition=0;
	m_lpCurrentRecord=m_lpFirstRecord;
}

// 맨 마지막으로
template<class _T>
void CCPtrList<_T>::MoveLast()
{
	if(m_nListCount>0){
		m_nCurrentPosition=m_nListCount-1;
		m_lpCurrentRecord=m_lpLastRecord;
	}
}

// 현재 레코드의 내용을 얻어낸다.
template<class _T>
_T *CCPtrList<_T>::Get()
{
	// 현재 레코드의 포인터는 NULL일수 없다.
	// 즉, 아무것도 저장되어 있지 않은 상태
	_ASSERT(m_lpCurrentRecord!=NULL);

	return(m_lpCurrentRecord->Get());
}

// 지정된 인덱스의 값을 얻어낸다.
template<class _T>
_T *CCPtrList<_T>::Get(int iindex)
{
	MoveRecord(iindex);
	return Get();
}

// 지정된 인덱스의 값을 얻어낸다.
/*
template<class _T>
_T &CCPtrList<_T>::operator[](int iindex)
{
	MoveRecord(iindex);
	return Get();
}
*/

// Quick Sort를 실행한다.
template<class _T>
void CCPtrList<_T>::Sort()
{
	if(GetCount()<=1)return;
	QuickSort(0,GetCount()-1);
}

// Quick Sort의 서브 함수
template<class _T>
void CCPtrList<_T>::QuickSort(int first,int last)
{
	int i,j;

	i=first,j=last;
	_T *pMiddle=Get((first+last)/2);

	// Compare()를 virtual로 계승받지 않으면 Assetion을 발생시킴
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

// 두개의 레코드의 값을 Swap한다.
template<class _T>
void CCPtrList<_T>::Swap(int nIndex1,int nIndex2)
{
	MoveRecord(nIndex1);
	CCPtrRecord<_T>* pRecord1=m_lpCurrentRecord;
	MoveRecord(nIndex2);
	CCPtrRecord<_T>* pRecord2=m_lpCurrentRecord;

	_T *pData=pRecord1->m_lpContent;
	pRecord1->m_lpContent=pRecord2->m_lpContent;
	pRecord2->m_lpContent=pData;
}
