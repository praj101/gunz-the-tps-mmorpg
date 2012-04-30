#include <cassert>

template <typename T> class CCSingleton
{
    static T* m_pSingleton;

public:
    CCSingleton( void )
    {
        assert( !m_pSingleton );

		/* 
		다중상속을 받았을때 포인터가 바르지 않은 문제를 해결한 싱클톤 템플릿

		gpg 에 나온 방법
        int offset = (int)((T*)1) - (int)(CCSingleton <T>*)((T*)1);
        m_pSingleton = (T*)((int)this + offset);
		*/
        
		// 이것이 표준에 가까운 방법인듯.
        m_pSingleton = static_cast<T *>(this); 

	}
   ~CCSingleton( void )
        {  assert( m_pSingleton );  m_pSingleton = 0;  }
    static T& GetInstance( void )
        {  assert( m_pSingleton );  return ( *m_pSingleton );  }
    static T* GetInstancePtr( void )
        {  return ( m_pSingleton );  }
};

template <typename T> T* CCSingleton <T>::m_pSingleton = 0;