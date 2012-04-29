#pragma once
#include <map>
#include <vector>
#include <crtdbg.h>

using namespace std;

/// MAIET Unique ID
struct CCUID{
	unsigned long int	High;	///< High 4 Byte
	unsigned long int	Low;	///< High 4 Byte

	CCUID(void) { SetZero(); }
	CCUID(unsigned long int h, unsigned long int l){
		High = h;
		Low = l;
	}

	/// CCUID 0으로 초기화
	void SetZero(void){
		High = Low = 0;
	}
	void SetInvalid(void){
		SetZero();
	}

	/*
	/// CCUID 1만큼 증가
	void Increase(void){
		if(Low==UINT_MAX){
			_ASSERT(High<UINT_MAX);
			High++;
			Low = 0;
		}
		else{
			Low++;
		}
	}
	/*/

	/// CCUID를 nSize만큼 증가
	CCUID Increase(unsigned long int nSize=1){
		if(Low+nSize>UINT_MAX){
			_ASSERT(High<UINT_MAX);
			Low = nSize-(UINT_MAX-Low);
			High++;
		}
		else{
			Low+=nSize;
		}
		return *this;
	}

	/// CCUID가 유효한지 검사, 0:0이면 Invalid
	bool IsInvalid(void) const {
		// 0이면 Invalid
		if(High==Low && Low==0) return true;
		return false;
	}
	/// CCUID가 유효한지 검사, 0:0이면 Invalid
	bool IsValid(void) const {
		// 0이면 Invalid
		if(High==Low && Low==0) return false;
		return true;
	}

	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator > (const CCUID& a, const CCUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>b.Low) return true;
		}
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator >= (const CCUID& a, const CCUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>=b.Low) return true;
		}
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator < (const CCUID& a, const CCUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<b.Low) return true;
		}
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator <= (const CCUID& a, const CCUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<=b.Low) return true;
		}
		return false;
	}

	/// 대입 오퍼레이터 오버로딩
	inline CCUID& operator=(int v){
		High = 0;
		Low = v;
		return *this;
	}
	/// 대입 오퍼레이터 오버로딩
	inline CCUID& operator=(const CCUID& a){
		High = a.High;
		Low = a.Low;
		return *this;
	}
	/// 비교 오퍼레이터 오버로딩
	inline friend bool operator==(const CCUID& a, const CCUID& b){
		if(a.High==b.High){
			if(a.Low==b.Low) return true;
		}
		return false;
	}
	/// 비교 오퍼레이터 오버로딩
	inline friend bool operator!=(const CCUID& a, const CCUID& b){
		if(a.Low!=b.Low) return true;
		if(a.High!=b.High) return true;
		return false;
	}
	/// 가산 오퍼레이터 오버로딩
	inline friend CCUID& operator++(CCUID& a){
		a.Increase();
		return a;
	}

	/// Invalid한 UID 얻기
	static CCUID Invalid(void);
};

/// CCUID 영역
struct CCUIDRANGE{
	CCUID	Start;
	CCUID	End;

	bool IsEmpty(void){
		return (Start==End);
	}
	void Empty(void){
		SetZero();
	}
	void SetZero(void){
		Start.SetZero();
		End.SetZero();
	}
};

/// int 두개값으로 CCUID 생성
#define MAKECCUID(_high, _low)	CCUID(_high, _low)


/// CCUID Reference Map
/// - 순차적으로 1씩 증가하는 ID를 얻어낼 수 있는 CCUID 집합
class CCUIDRefMap : protected map<CCUID, void*>{
	CCUID	m_CurrentCCUID;		///< 현재 발급된 CCUID
public:
	CCUIDRefMap(void);
	virtual ~CCUIDRefMap(void);

	/// 레퍼런스를 CCUID하나에 할당한다.
	/// @param pRef	레퍼런스 포인터
	/// @return		할당된 CCUID
	CCUID Generate(void* pRef);

	/// CCUID를 통해 레퍼런스 포인터를 얻어낸다.
	/// @param uid	CCUID
	/// @return		레퍼런스 포인터, CCUID가 존재하지 않으면 NULL을 리턴
	void* GetRef(CCUID& uid);

	/// 등록된 CCUID 삭제.
	/// @param uid	CCUID
	/// @return		등록되었던 레퍼런스 포인터, CCUID가 존재하지 않으면 NULL을 리턴
	void* Remove(CCUID& uid);

	/// nSize만큼 확보
	CCUIDRANGE Reserve(int nSize);

	/// 지금까지 발급된 UID 구간
	CCUIDRANGE GetReservedCount(void);
};

/*
/// CCUID Reference Array
/// - 비교적 작은 용량의 CCUID를 생성해내기 위한 CCUID 집합
class CCUIDRefArray : protected vector<void*>{
	CCUID	m_CurrentCCUID;		///< 현재 CCUID
public:
	CCUIDRefArray(void);
	virtual ~CCUIDRefArray(void);

	/// 레퍼런스를 CCUID하나에 할당한다.
	/// @param pRef	레퍼런스 포인터
	/// @return		할당된 CCUID
	CCUID Generate(void* pRef);
	/// CCUID를 통해 레퍼런스 포인터를 얻어낸다.
	/// @param uid	CCUID
	/// @return		레퍼런스 포인터, CCUID가 존재하지 않으면 NULL을 리턴
	void* GetRef(CCUID& uid);
};
*/

/// CCUID Reference Map Cache
/// - 전체 UID의 검색 부하를 줄이기 위한 캐쉬 클래스
class CCUIDRefCache : public map<CCUID, void*>{
public:
	CCUIDRefCache(void);
	virtual ~CCUIDRefCache(void);

	/// 레퍼런스를 CCUID하나에 할당한다.
	/// @param pRef	레퍼런스 포인터
	/// @return		할당된 CCUID
	void Insert(const CCUID& uid, void* pRef);
	/// CCUID를 통해 레퍼런스 포인터를 얻어낸다.
	/// @param uid	CCUID
	/// @return		레퍼런스 포인터, CCUID가 존재하지 않으면 NULL을 리턴
	void* GetRef(const CCUID& uid);
	/// 등록된 CCUID 삭제.
	/// @param uid	CCUID
	/// @return		등록되었던 레퍼런스 포인터, CCUID가 존재하지 않으면 NULL을 리턴
	void* Remove(const CCUID& uid);
};
