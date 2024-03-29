#pragma once
#include <list>
using namespace std;

typedef void(ZGameTimerEventCallback)(void* pParam);

class ZTimerEvent;

// 앞으로 게임틱, 타이머 관련은 여기서 관리하도록 하자
class ZTimer
{
private:
	bool						m_bInitialized;


	unsigned long int			m_iLastTime;
	unsigned long int			m_iNowTime;
	list<ZTimerEvent*>			m_EventList;

	// 타이머 갱신용 변수들 : 메모리핵으로 변조하면 스피드핵이 가능하다 그래서 힙할당하고 주기적으로 위치를 옮긴다
	BOOL* m_pbUsingQPF;
	LONGLONG* m_pllQPFTicksPerSec;
	LONGLONG* m_pllLastElapsedTime;
	DWORD* m_pThistime;
	DWORD* m_pLasttime;
	DWORD* m_pElapsed;

	void UpdateEvents();
	void ShiftFugitiveValues();

public:
	ZTimer();
	virtual ~ZTimer();

	float UpdateFrame();
	void ResetFrame();

	// 타이머 이벤트 관련
	void SetTimerEvent(unsigned long int nElapsedTime, ZGameTimerEventCallback* fnTimerEventCallback, void* pParam, bool bTimerOnce=false);
	void ClearTimerEvent(ZGameTimerEventCallback* fnTimerEventCallback);


	unsigned long int GetNowTick() { return m_iNowTime; }
};

class ZUpdateTimer
{
private:
	float		m_fUpdateTime;
	float		m_fElapsedTime;
public:
	ZUpdateTimer(float fUpdateTime) : m_fUpdateTime(fUpdateTime), m_fElapsedTime(0.0f) { }
	ZUpdateTimer() : m_fUpdateTime(0.0f), m_fElapsedTime(0.0f) { }
	bool Update(float fDelta)
	{
		m_fElapsedTime += fDelta;
		if (m_fElapsedTime < m_fUpdateTime) return false;

		m_fElapsedTime = 0.0f;
		return true;
	}
	void Init(float fUpdateTime)	{ m_fUpdateTime = fUpdateTime; m_fElapsedTime = 0.0f; }
	void Force()					{ m_fElapsedTime = m_fUpdateTime; }
	void SetUpdateTime(float fUpdateTime) { m_fUpdateTime = fUpdateTime; }
	float GetUpdateTime() const { return m_fUpdateTime; }
};
