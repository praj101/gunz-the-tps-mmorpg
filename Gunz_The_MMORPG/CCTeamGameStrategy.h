#pragma once
#include "CCMatchGlobal.h"
#include <vector>
using namespace std;

class CCMatchObject;
class MLadderGroup;
class CCMatchStage;

class MLadderGameStrategy;
class MClanGameStrategy;
struct CCMatchLadderTeamInfo;

class CCBaseTeamGameStrategy
{
protected:
	CCBaseTeamGameStrategy() { }
	virtual ~CCBaseTeamGameStrategy() { }
public:
	/// 도전 가능한지 체크한다.
	virtual int ValidateChallenge(CCMatchObject** ppMemberObject, int nMemberCount) = 0;

	/// 도전하자고 다른사람들한테 제안할 수 있는지 체크한다.
	virtual int ValidateRequestInviteProposal(CCMatchObject* pProposerObject, CCMatchObject** ppReplierObjects,
					const int nReplierCount) = 0;
	/// 새로운 LadderGroup ID를 생성해서 반환한다.
	virtual int GetNewGroupID(CCMatchObject* pLeaderObject, CCMatchObject** ppMemberObjects, int nMemberCount) = 0;

	/// LadderGroup의 필요한 정보를 세팅한다. ID빼고..
	virtual void SetLadderGroup(MLadderGroup* pGroup, CCMatchObject** ppMemberObjects, int nMemberCount) = 0;

	/// Stage에서 필요한 LadderInfo를 세팅한다.
	virtual void SetStageLadderInfo(CCMatchLadderTeamInfo* poutRedLadderInfo, CCMatchLadderTeamInfo* poutBlueLadderInfo,
									MLadderGroup* pRedGroup, MLadderGroup* pBlueGroup) = 0;

	/// 게임이 끝났을때 결과를 DB 저장한다.
	virtual void SavePointOnFinishGame(CCMatchStage* pStage, CCMatchTeam nWinnerTeam, bool bIsDrawGame,
		                               CCMatchLadderTeamInfo* pRedLadderInfo, CCMatchLadderTeamInfo* pBlueLadderInfo) = 0;

	virtual int GetRandomMap(int nTeamMember) = 0;

	/// 서버모드에 따라 적당한 자식 클래스를 반환한다. CSM_LADDER, CSM_CLAN만 가능
	static CCBaseTeamGameStrategy* GetInstance(CCMatchServerMode nServerMode);
};


class MLadderGameStrategy : public CCBaseTeamGameStrategy
{
protected:
	MLadderGameStrategy() { }
public:
	static MLadderGameStrategy* GetInstance()
	{
		static MLadderGameStrategy m_stInstance;
		return &m_stInstance;
	}
	virtual int ValidateChallenge(CCMatchObject** ppMemberObject, int nMemberCount);
	virtual int ValidateRequestInviteProposal(CCMatchObject* pProposerObject, CCMatchObject** ppReplierObjects,
					const int nReplierCount);
	virtual int GetNewGroupID(CCMatchObject* pLeaderObject, CCMatchObject** ppMemberObjects, int nMemberCount);
	virtual void SetLadderGroup(MLadderGroup* pGroup, CCMatchObject** ppMemberObjects, int nMemberCount) { }
	virtual void SetStageLadderInfo(CCMatchLadderTeamInfo* poutRedLadderInfo, CCMatchLadderTeamInfo* poutBlueLadderInfo,
									MLadderGroup* pRedGroup, MLadderGroup* pBlueGroup);
	virtual void SavePointOnFinishGame(CCMatchStage* pStage, CCMatchTeam nWinnerTeam, bool bIsDrawGame,
		                               CCMatchLadderTeamInfo* pRedLadderInfo, CCMatchLadderTeamInfo* pBlueLadderInfo);
	virtual int GetRandomMap(int nTeamMember);
};


class MClanGameStrategy : public CCBaseTeamGameStrategy
{
protected:
	MClanGameStrategy();
	vector<int>		m_RandomMapVec[CCLADDERTYPE_MAX];
public:
	static MClanGameStrategy* GetInstance()
	{
		static MClanGameStrategy m_stInstance;
		return &m_stInstance;
	}

	virtual int ValidateChallenge(CCMatchObject** ppMemberObject, int nMemberCount);
	virtual int ValidateRequestInviteProposal(CCMatchObject* pProposerObject, CCMatchObject** ppReplierObjects,
					const int nReplierCount);
	virtual int GetNewGroupID(CCMatchObject* pLeaderObject, CCMatchObject** ppMemberObjects, int nMemberCount);
	virtual void SetLadderGroup(MLadderGroup* pGroup, CCMatchObject** ppMemberObjects, int nMemberCount);
	virtual void SetStageLadderInfo(CCMatchLadderTeamInfo* poutRedLadderInfo, CCMatchLadderTeamInfo* poutBlueLadderInfo,
									MLadderGroup* pRedGroup, MLadderGroup* pBlueGroup);
	virtual void SavePointOnFinishGame(CCMatchStage* pStage, CCMatchTeam nWinnerTeam, bool bIsDrawGame,
		                               CCMatchLadderTeamInfo* pRedLadderInfo, CCMatchLadderTeamInfo* pBlueLadderInfo);
	virtual int GetRandomMap(int nTeamMember);
};
