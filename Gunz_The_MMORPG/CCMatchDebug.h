#pragma once
// cscommon에서 디버그에 관한 사항 정리

//#define _DEBUG_PUBLISH				// 디버그모드로 퍼블리쉬 - 테스트하기 위함
#pragma warning( disable: 4996 )
#define _CRT_SECURE_NO_WARNINGS
#if defined(_DEBUG_PUBLISH) && defined(_DEBUG)
	#ifdef _ASSERT
	#undef _ASSERT
	#endif
	#define _ASSERT(X)		(()0)

	#ifdef OutputDebugString
	#undef OutputDebugString
	#endif
	#define OutputDebugString(X)		(()0)
#endif

// 컴파일러옵션 /GS옵션을 사용할때 버퍼 오버런이 났을 경우 뜨는 메시지 박스를 가로챈다.
//void SetSecurityErrorHandler(_secerr_handler_func func); __report_gsfailure
void __cdecl ReportBufferOverrun(int code, void * unused);