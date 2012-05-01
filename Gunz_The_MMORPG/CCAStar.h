/*

  A Star Search
  -------------

  1999/10/8, lee jang ho
  All copyright (c) 1998, MAIET entertainment software, inc

*/
#pragma once

#include "CCPtrList.h"
#include "CCList.h"

class CCNodeModel{
public:
	float		m_fCostFromStart;
	float		m_fCostToGoal;
	CCNodeModel*	m_pParent;
	int			m_nVisitID;
public:
	CCNodeModel(void);
	virtual ~CCNodeModel(void);
	// 이웃 Node 얻기
	virtual int GetSuccessorCount(void) = 0;
	virtual CCNodeModel* GetSuccessor(int i) = 0;

	// 이웃 Node들의 Cost 얻기
	float GetSuccessorCost(int i){
		return GetSuccessorCost(GetSuccessor(i));
	}
	virtual float GetSuccessorCost(CCNodeModel* pSuccessor) = 0;

	// 시작점부터 이웃 Node들까지의 Cost 얻기
	virtual float GetSuccessorCostFromStart(CCNodeModel* pSuccessor){
		return m_fCostFromStart + GetSuccessorCost(pSuccessor);
	}

	// Heuristic Estimation에 의한 pNode까지의 Cost 얻기
	virtual float GetHeuristicCost(CCNodeModel* pNode) = 0;

	float GetTotalCost(void){
		return m_fCostFromStart+m_fCostToGoal;
	}
	virtual float GetTotalCostPriority(void){	// 우선순위를 결정하기 위한 Total Cost
		return GetTotalCost();	// 대부분 GetTotalCost()와 같다.
	}
};

struct CCFORCEDNODE{
	CCNodeModel* pNode;
	CCNodeModel* pFromNode;
};

class MAStar{
	CCPtrList<MNodeModel>	m_ShortestPath;
	class CMSortedList : public CCPtrList<CCNodeModel>{
	public:
		int Compare(CCNodeModel *lpRecord1, CCNodeModel *lpRecord2){
			float fCompare = lpRecord1->GetTotalCostPriority()-lpRecord2->GetTotalCostPriority();
			if(fCompare>0.0f) return 1;
			else if(fCompare==0.0f) return 0;
			else return -1;
		}
	} m_OpenStack;

	CCLinkedList<CCFORCEDNODE>	m_ForcedCloseNodes;
	int		m_nVisitID;	/// 노드 검색 ID
	
public:
	int		m_nNodeTraverseCount;		// 노드 탐색 횟수

	//CCPtrList<MNodeModel>	m_CloseStack;
protected:
	void PushOpen(CCNodeModel* pNode);
	CCNodeModel* PopOpen(void);
	CCNodeModel* PopLowestCostOpen(void);
	bool IsOpenEmpty(void);
	int IsOpenNode(CCNodeModel* pNode);
	/*
	void PushClose(MNodeModel* pNode);
	MNodeModel* PopClose(void);
	int IsCloseNode(MNodeModel* pNode);
	void RemoveClose(int i);
	*/
	bool IsForcedClose(CCNodeModel* pNode, CCNodeModel* pFromNode);

public:
	CCAStar(void);

	void AddForcedClose(CCNodeModel* pNode, CCNodeModel* pFromNode);	// 강제로 Close 정의
	void RemoveForcedClose(void);

	bool FindPath(CCNodeModel* pStartNode, CCNodeModel* pGoalNode, int nLimit=-1, float fMinHeuristicValue=-1, bool bIncludeStartNode=false);

	int GetShortestPathCount(void);
	CCNodeModel* GetShortestPath(int i);
	void RemoveShortestPath(int i);

	CCPtrList<MNodeModel>* GetShortestPath(void);

	void Reset(void);
};
