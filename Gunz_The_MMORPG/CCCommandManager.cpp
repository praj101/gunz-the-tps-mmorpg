#include "stdafx.h"
#include "CCCommandManager.h"
#include "CCCLexicalAnalyzer.h"
#include <algorithm>
#include "CCStrEx.h"
#include <Windows.h>	// for test

void CCCommandManager::InitializeCommandDesc()
{
	for(CCCommandDescMap::iterator i=m_CommandDescs.begin(); i!=m_CommandDescs.end(); i++){
		delete (*i).second;
	}
	m_CommandDescs.clear();
}

CCCommandManager::CCCommandManager()
{
	InitializeCommandMemPool();
	InitializeCommandDesc();
}

CCCommandManager::~CCCommandManager()
{
	InitializeCommandDesc();
	while(PeekCommand()) {
		delete GetCommand();
	}

	FinalizeCommandMemPool();
}

void CCCommandManager::Initialize()
{
	for(CCCommandList::iterator i=m_CommandQueue.begin(); i!=m_CommandQueue.end(); i++){
		delete (*i);
	}
	m_CommandQueue.clear();
}

int CCCommandManager::GetCommandDescCount() const
{
	return (int)m_CommandDescs.size();
}

int CCCommandManager::GetCommandQueueCount() const
{
	return (int)m_CommandQueue.size();
}


CCCommandDesc* CCCommandManager::GetCommandDesc(int i)
{
	if(i<0 || i>=(int)m_CommandDescs.size()) return NULL;

	CCCommandDescMap::iterator itor = m_CommandDescs.begin();

	for (int t=0; t < i; t++)
	{
		itor++;
		if (itor == m_CommandDescs.end()) return NULL;
	}

	return (*itor).second;
}


void CCCommandManager::AssignDescs(CCCommandManager* pTarCM)
{
	for(CCCommandDescMap::iterator i=m_CommandDescs.begin(); i!=m_CommandDescs.end(); i++)
	{
		CCCommandDesc* pDesc = (*i).second;
		pTarCM->AddCommandDesc(pDesc->Clone());
	}
}

CCCommandDesc* CCCommandManager::GetCommandDescByID(int nID)
{
	CCCommandDescMap::iterator itor = m_CommandDescs.find(nID);
	if (itor != m_CommandDescs.end())
	{
		return (*itor).second;
	}
	
	return NULL;
}

void CCCommandManager::AddCommandDesc(CCCommandDesc* pCD)
{
	_ASSERT(m_CommandDescs.find(pCD->GetID())==m_CommandDescs.end());	// 커맨드는 중복되면 안된다
	m_CommandDescs.insert(CCCommandDescMap::value_type(pCD->GetID(), pCD));
}

bool CCCommandManager::Post(CCCommand* pCmd)
{
	bool bCheckRule = pCmd->CheckRule();
	_ASSERT(bCheckRule==true);
	if(bCheckRule==false) return false;

	m_CommandQueue.push_back(pCmd);


	return true;
}

CCCommand* CCCommandManager::GetCommand()
{
	if(m_CommandQueue.size()==0) return NULL;

	CCCommand* pCmd = *m_CommandQueue.begin();
	
	m_CommandQueue.erase(m_CommandQueue.begin());

	return pCmd;
}

CCCommand* CCCommandManager::PeekCommand()
{
	if(m_CommandQueue.size()==0) return NULL;

	CCCommand* pCmd = *m_CommandQueue.begin();
	return pCmd;
}

void CCCommandManager::GetSyntax(char* szSyntax, const CCCommandDesc* pCD)
{
	sprintf(szSyntax, "%s ", pCD->GetName());
	for(int i=0; i<pCD->GetParameterDescCount(); i++){
		CCCommandParameterDesc* pPD = pCD->GetParameterDesc(i);
		sprintf(szSyntax, "%s %s", szSyntax, pPD->GetDescription());
	}
}

bool CCCommandManager::ParseMessage(CCCommand* pCmd, char* szErrMsg, int nErrMsgMaxLength, const char* szMsg)
{
//#define USE_SLASH
#ifdef USE_SLASH
	if(!(szMsg[0]=='/' && szMsg[1]!=0)){
		MStrNCpy(szErrMsg, nErrMsgMaxLength, "Use Slash('/') First");
		return false;
	}
#endif

	CCLexicalAnalyzer la;
#ifdef USE_SLASH
	la.Create(szMsg+1);
#else
	la.Create(szMsg);
#endif
	
	if(la.GetCount()==0){
		MStrNCpy(szErrMsg, nErrMsgMaxLength, "Syntax Error");
		return false;
	}

#define ASMESSAGE_LENGTH	256
	char szTemp[ASMESSAGE_LENGTH];
	strcpy(szTemp, la.GetByStr(0));

	CCCommandAliasMap::iterator itor = m_CommandAlias.find(string(szTemp));
	if (itor != m_CommandAlias.end())
	{
		strcpy(szTemp, (*itor).second.c_str());
	}

	//for(int i=0; i<(int)m_CommandDescs.size(); i++){
	for (CCCommandDescMap::iterator itor = m_CommandDescs.begin(); itor != m_CommandDescs.end(); ++itor)
	{
		CCCommandDesc* pCD = (*itor).second;
		//CCCommandDesc* pCD = m_CommandDescs[i];

		if(stricmp(szTemp, pCD->GetName())==0){
			//if(pCD->IsFlag(ASCDF_CHEAT)==true && EnableDevDebug()==false) return false;	// 개발자 전용 커맨드이면... Debug가 Enable되어 있어야 한다.

			pCmd->SetID(pCD);

			int nLAMaxCount = la.GetCount();
			int nLACount = 1;

			for(int j=0; j<pCD->GetParameterDescCount(); j++){
				CCCommandParameterDesc* pPD = pCD->GetParameterDesc(j);

				bool bSyntaxError = false;
				CCCommandParameter* pParam = NULL;
				switch(pPD->GetType()){
				case MPT_INT:
					if(nLACount+1>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new CCCommandParameterInt(la.GetByInt(nLACount));
					nLACount++;
					break;
				case MPT_UINT:
					{
						if(nLACount+1>nLAMaxCount){
							bSyntaxError = true;
							break;
						}
						pParam = new CCCommandParameterUInt(la.GetByLong(nLACount));
						nLACount++;
						break;
					}
					break;
				case MPT_FLOAT:
					if(nLACount+1>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new CCCommandParameterFloat(la.GetByFloat(nLACount));
					nLACount++;
					break;
				case MPT_STR:
					if(nLACount+1>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new CCCommandParameterString(la.GetByStr(nLACount));
					nLACount++;
					break;
				case MPT_VECTOR:
					if(nLACount+3>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new CCCommandParameterVector(la.GetByFloat(nLACount), la.GetByFloat(nLACount+1), la.GetByFloat(nLACount+2));
					nLACount+=3;
					break;
				case MPT_POS:
					if(nLACount+3>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new CCCommandParameterPos(la.GetByFloat(nLACount), la.GetByFloat(nLACount+1), la.GetByFloat(nLACount+2));
					nLACount+=3;
					break;
				case MPT_DIR:
					if(nLACount+3>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new CCCommandParameterDir(la.GetByFloat(nLACount), la.GetByFloat(nLACount+1), la.GetByFloat(nLACount+2));
					nLACount+=3;
					break;
				case MPT_COLOR:
					if(nLACount+3>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new CCCommandParameterColor(la.GetByFloat(nLACount), la.GetByFloat(nLACount+1), la.GetByFloat(nLACount+2));
					nLACount+=3;
					break;
				case MPT_BOOL:
					if(nLACount+1>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					pParam = new CCCommandParameterBool(la.GetByInt(nLACount)>0?true:false);
					nLACount++;
					break;
					
				case MPT_UID:
					if(nLACount+2>nLAMaxCount){
						bSyntaxError = true;
						break;
					}
					// UINT를 만들어야 한다.
					pParam = new CCCommandParameterUID(CCUID(la.GetByInt(nLACount), la.GetByInt(nLACount+1)));
					nLACount+=2;
					break;
					
				default:
					_ASSERT(false);		// 아직 핸들링할 코드가 준비 안된 파라미터
					return false;
					//break;
				}

				if(bSyntaxError==true){
					char szSyntax[256];
					static char temp[512];
					GetSyntax(szSyntax, pCmd->m_pCommandDesc);
					sprintf(temp, "Sytax Error: [Syntax] %s", szSyntax);
					MStrNCpy(szErrMsg, nErrMsgMaxLength, temp);
					return false;
				}

				if(pParam!=NULL) pCmd->AddParameter(pParam);
			}

			return true;
		}
	}

	MStrNCpy(szErrMsg, nErrMsgMaxLength, "Unknown Command");

	return false;
}

void CCCommandManager::AddAlias(string szName, string szText)
{
	m_CommandAlias.insert(CCCommandAliasMap::value_type(szName, szText));
}

void CCCommandManager::InitializeCommandMemPool()
{

	InitMemPool(CCCommand);
	InitMemPool(CCCommandParameterInt);
	InitMemPool(CCCommandParameterUInt);
	InitMemPool(CCCommandParameterFloat);
	InitMemPool(CCCommandParameterPos);
	InitMemPool(CCCommandParameterDir);
	InitMemPool(CCCommandParameterColor);
	InitMemPool(CCCommandParameterBool);
	InitMemPool(CCCommandParameterUID);
	InitMemPool(CCCommandParameterChar);
	InitMemPool(CCCommandParameterUChar);
	InitMemPool(CCCommandParameterShort);
	InitMemPool(CCCommandParameterUShort);
	InitMemPool(CCCommandParameterInt64);
	InitMemPool(CCCommandParameterUInt64);
	InitMemPool(CCCommandParameterShortVector);

}

void CCCommandManager::FinalizeCommandMemPool()
{

	ReleaseMemPool(CCCommandParameterInt);
	ReleaseMemPool(CCCommandParameterUInt);
	ReleaseMemPool(CCCommandParameterFloat);
	ReleaseMemPool(CCCommandParameterPos);
	ReleaseMemPool(CCCommandParameterDir);
	ReleaseMemPool(CCCommandParameterColor);
	ReleaseMemPool(CCCommandParameterBool);
	ReleaseMemPool(CCCommandParameterUID);
	ReleaseMemPool(CCCommandParameterChar);
	ReleaseMemPool(CCCommandParameterUChar);
	ReleaseMemPool(CCCommandParameterShort);
	ReleaseMemPool(CCCommandParameterUShort);
	ReleaseMemPool(CCCommandParameterInt64);
	ReleaseMemPool(CCCommandParameterUInt64);
	ReleaseMemPool(CCCommandParameterShortVector);
	ReleaseMemPool(CCCommand);


	UninitMemPool(CCCommandParameterInt);
	UninitMemPool(CCCommandParameterUInt);
	UninitMemPool(CCCommandParameterFloat);
	UninitMemPool(CCCommandParameterPos);
	UninitMemPool(CCCommandParameterDir);
	UninitMemPool(CCCommandParameterColor);
	UninitMemPool(CCCommandParameterBool);
	UninitMemPool(CCCommandParameterUID);
	UninitMemPool(CCCommandParameterChar);
	UninitMemPool(CCCommandParameterUChar);
	UninitMemPool(CCCommandParameterShort);
	UninitMemPool(CCCommandParameterUShort);
	UninitMemPool(CCCommandParameterInt64);
	UninitMemPool(CCCommandParameterUInt64);
	UninitMemPool(CCCommandParameterShortVector);
	UninitMemPool(CCCommand);

}
