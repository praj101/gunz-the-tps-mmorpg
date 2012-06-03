#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCErrorTable.h"
#include "CCBlobArray.h"
#include "CCObject.h"
#include "CCMatchObject.h"
#include "Msg.h"
#include "CCMatchConfig.h"
#include "CCCommandCommunicator.h"
#include "CCDebug.h"
#include "CCMatchAuth.h"
#include "CCAsyncDBJob.h"
#include "CCAsyncDBJob_GetLoginInfo.h"
#include "CCAsyncDBJob_InsertConnLog.h"
#include "RTypes.h"
#include "CCMatchUtil.h"
#include <winbase.h>
#include "CCMatchPremiumIPCache.h"
#include "CCCommandBuilder.h"
#include "CCMatchStatus.h"
#include "CCMatchLocale.h"

bool CCMatchServer::CheckOnLoginPre(const CCUID& ComCCUID, int nCmdVersion, bool& outbFreeIP, string& strCountryCode3)
{
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(ComCCUID);
	if (pCommObj == NULL) return false;

	// 프로토콜 버전 체크
	if (nCmdVersion != MCOMMAND_VERSION)
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_COMMAND_INVALID_VERSION);
		Post(pCmd);	
		return false;
	}

	// free login ip를 검사하기전에 debug서버와 debug ip를 검사한다.
	// 서버가 debug타입인지 검사.
	if( CCGetServerConfig()->IsDebugServer() && CCGetServerConfig()->IsDebugLoginIPList(pCommObj->GetIPString()) )
	{
		outbFreeIP = true;
		return true;
	}

	// 최대인원 체크
	bool bFreeLoginIP = false;
	if (CCGetServerConfig()->CheckFreeLoginIPList(pCommObj->GetIPString()) == true) {
		bFreeLoginIP = true;
		outbFreeIP = true;
		return true;
	} else {
		outbFreeIP = false;

		if ((int)m_Objects.size() >= CCGetServerConfig()->GetMaxUser())
		{
			CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_CLIENT_FULL_PLAYERS);
			Post(pCmd);	
			return false;
		}
	}

	// 접속을 막아놓은 지역의 IP인가
	if( CheckIsValidIP(ComCCUID, pCommObj->GetIPString(), strCountryCode3, CCGetServerConfig()->IsUseFilter()) )
		IncreaseNonBlockCount();
	else
	{
		IncreaseBlockCount();

		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_FAILED_BLOCK_IP);
		Post(pCmd);	
		return false;
	}

	return true;
}

void CCMatchServer::OnMatchLogin(CCUID ComCCUID, const char* szUserID, const char* szPassword, int nCommandVersion, unsigned long nChecksumPack, char *szEncryptMd5Value)
{
//	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(ComCCUID);
//	if (pCommObj == NULL) return;

	// 초기 위치의 노드는 검색해서 얻어낸다.
	int nMapID = 0;

	unsigned int nAID = 0;
	char szDBPassword[32] = "";
	string strCountryCode3;

	bool bFreeLoginIP = false;

	// 프로토콜, 최대인원 체크
	if (!CheckOnLoginPre(ComCCUID, nCommandVersion, bFreeLoginIP, strCountryCode3)) return;


	// 원래 계정은 넷마블에 있으므로 해당 계정이 없으면 새로 생성한다. 
	if (!m_MatchDBMgr.GetLoginInfo(szUserID, &nAID, szDBPassword))
	{
#ifdef _DEBUG
		m_MatchDBMgr.CreateAccount(szUserID, szPassword, 0, szUserID, 20, 1);
		strcpy(szDBPassword, szPassword);

		m_MatchDBMgr.GetLoginInfo(szUserID, &nAID, szDBPassword);
#endif

		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_CLIENT_WRONG_PASSWORD);
		Post(pCmd);	

		return;
	}


	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(ComCCUID);
	if (pCommObj)
	{
		// 디비에 최종 접속시간을 업데이트 한다.
		if (!m_MatchDBMgr.UpdateLastConnDate(szUserID, pCommObj->GetIPString()))
		{	
			cclog("DB Query(OnMatchLogin > UpdateLastConnDate) Failed");
		}

	}


	// 패스워드가 틀렸을 경우 처리
	if (strcmp(szDBPassword, szPassword))
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_CLIENT_WRONG_PASSWORD);
		Post(pCmd);	

		return;
	}

	CCMatchAccountInfo accountInfo;
	if (!m_MatchDBMgr.GetAccountInfo(nAID, &accountInfo, CCGetServerConfig()->GetServerID()))
	{
		// Notify Message 필요 -> 로그인 관련 - 해결(Login Fail 메세지 이용)
		// Disconnect(ComCCUID);
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_FAILED_GETACCOUNTINFO);
		Post(pCmd);	
	}

	CCMatchAccountPenaltyInfo accountpenaltyInfo;
	if( !m_MatchDBMgr.GetAccountPenaltyInfo(nAID, &accountpenaltyInfo) ) 
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_FAILED_GETACCOUNTINFO);
		Post(pCmd);	
	}

#ifndef _DEBUG
	// 중복 로그인이면 이전에 있던 사람을 끊어버린다.
	CCMatchObject* pCopyObj = GetPlayerByAID(accountInfo.m_nAID);
 	if (pCopyObj != NULL) 
	{
		// 내가 로그인일때 이미 로그인 돼있는 클라이언트가 있으면 이미 로그인 클라이언트에 
		// 중복 로그인이란 메세지 보내고 접속을 끊음. - by kammir 2008.09.30
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(pCopyObj->GetUID(), MERR_MULTIPLE_LOGIN);
		Post(pCmd);	
		//Disconnect(pCopyObj->GetUID());
	}
#endif

	// 사용정지 계정인지 확인한다.
	if ((accountInfo.m_nUGrade == CCMUGBLOCKED) || (accountInfo.m_nUGrade == CCMUGPENALTY))
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_CLIENT_CCMUGBLOCKED);
		Post(pCmd);	
		return;
	}

#ifndef _DEBUG // debug에선 상관없다. 테스트가 필요하면 따로 설정을 해야 함. - by SungE 2007-05-03
	// gunz.exe 실행파일의 무결성을 확인한다. (암호화 되어 있다)
	// server.ini 파일에서 설정된 값에 따라 사용하지 않으면 검사하지 않는다.
	if (CCGetServerConfig()->IsUseMD5())				
	{
		unsigned char szMD5Value[ MAX_MD5LENGH ] = {0, };
		pCommObj->GetCrypter()->Decrypt(szEncryptMd5Value, MAX_MD5LENGH, (CCPacketCrypterKey*)pCommObj->GetCrypter()->GetKey());
		memcpy( szMD5Value, szEncryptMd5Value, MAX_MD5LENGH );

		if ((memcmp(m_szMD5Value, szMD5Value, MAX_MD5LENGH)) != 0)
		{
			// "정상적인 실행파일이 아닙니다." 이런 오류 패킷이 없어서 전송 생략
			LOG(LOG_PROG, "MD5 error : AID(%u).\n \n", accountInfo.m_nAID);
			// 접속 끊어버리자
//			Disconnect(ComCCUID);
			return;
		}
	}
#endif

	// 로그인성공하여 오브젝트(CCMatchObject) 생성
	AddObjectOnMatchLogin(ComCCUID, &accountInfo, &accountpenaltyInfo, bFreeLoginIP, strCountryCode3, nChecksumPack);

/*
	CCUID AllocUID = ComCCUID;
	int nErrCode = ObjectAdd(ComCCUID);
	if(nErrCode!=MOK){
		LOG(LOG_DEBUG, MErrStr(nErrCode) );
	}

	CCMatchObject* pObj = GetObject(AllocUID);
	pObj->AddCommListener(ComCCUID);
	pObj->SetObjectType(MOT_PC);
	memcpy(pObj->GetAccountInfo(), &accountInfo, sizeof(CCMatchAccountInfo));
	pObj->SetFreeLoginIP(bFreeLoginIP);
	pObj->SetCountryCode3( strCountryCode3 );
	pObj->UpdateTickLastPacketRecved();

	if (pCommObj != NULL)
	{
		pObj->SetPeerAddr(pCommObj->GetIP(), pCommObj->GetIPString(), pCommObj->GetPort());
	}
	
	SetClientClockSynchronize(ComCCUID);


	// 프리미엄 IP를 체크한다.
	if (CCGetServerConfig()->CheckPremiumIP())
	{
		if (pCommObj)
		{
			bool bIsPremiumIP = false;
			bool bExistPremiumIPCache = false;
			
			bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(pCommObj->GetIP(), bIsPremiumIP);

			// 만약 캐쉬에 없으면 직접 DB에서 찾도록 한다.
			if (!bExistPremiumIPCache)
			{
				if (m_MatchDBMgr.CheckPremiumIP(pCommObj->GetIPString(), bIsPremiumIP))
				{
					// 결과를 캐쉬에 저장
					MPremiumIPCache()->AddIP(pCommObj->GetIP(), bIsPremiumIP);
				}
				else
				{
					MPremiumIPCache()->OnDBFailed();
				}

			}

			if (bIsPremiumIP) pObj->GetAccountInfo()->m_nPGrade = MMPG_PREMIUM_IP;
		}		
	}


	CCCommand* pCmd = CreateCmdMatchResponseLoginOK(ComCCUID, 
												   AllocUID, 
												   pObj->GetAccountInfo()->m_szUserID,
												   pObj->GetAccountInfo()->m_nUGrade,
                                                   pObj->GetAccountInfo()->m_nPGrade);
	Post(pCmd);	

	// 접속 로그를 남긴다.
	m_MatchDBMgr.InsertConnLog(pObj->GetAccountInfo()->m_nAID, pObj->GetIPString(), pObj->GetCountryCode3() );

#ifndef _DEBUG
	// Client DataFile Checksum을 검사한다.
	unsigned long nChecksum = nChecksumPack ^ ComCCUID.High ^ ComCCUID.Low;
	if (nChecksum != GetItemFileChecksum()) {
		LOG(LOG_PROG, "Invalid ZItemChecksum(%u) , UserID(%s) ", nChecksum, pObj->GetAccountInfo()->m_szUserID);
		Disconnect(ComCCUID);
	}
#endif

*/
}

/*
void CCMatchServer::OnMatchLoginFromNetmarble(const CCUID& ComCCUID, const char* szCPCookie, const char* szSpareData, int nCmdVersion, unsigned long nChecksumPack)
{
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(ComCCUID);
	if (pCommObj == NULL) return;

	bool bFreeLoginIP = false;
	string strCountryCode3;

	// 프로토콜, 최대인원 체크
	if (!CheckOnLoginPre(ComCCUID, nCmdVersion, bFreeLoginIP, strCountryCode3)) return;


	CCMatchAuthBuilder* pAuthBuilder = GetAuthBuilder();
	if (pAuthBuilder == NULL) {
		LOG(LOG_PROG, "Critical Error : MatchAuthBuilder is not assigned.\n");
		return;
	}
	CCMatchAuthInfo* pAuthInfo = NULL;
	if (pAuthBuilder->ParseAuthInfo(szCPCookie, &pAuthInfo) == false) 
	{
		MGetServerStatusSingleton()->SetRunStatus(5);

		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_CLIENT_WRONG_PASSWORD);
		Post(pCmd);	

		LOG(LOG_PROG, "Netmarble Certification Failed\n");
		return;
	}

	const char* pUserID = pAuthInfo->GetUserID();
	const char* pUniqueID = pAuthInfo->GetUniqueID();
	const char* pCertificate = pAuthInfo->GetCertificate();
	const char* pName = pAuthInfo->GetName();
	int nAge = pAuthInfo->GetAge();
	int nSex = pAuthInfo->GetSex();
	bool bCheckPremiumIP = CCGetServerConfig()->CheckPremiumIP();
	const char* szIP = pCommObj->GetIPString();
	DWORD dwIP = pCommObj->GetIP();

	// Async DB
	CCAsyncDBJob_GetLoginInfo* pNewJob = new CCAsyncDBJob_GetLoginInfo(ComCCUID);
	pNewJob->Input(new CCMatchAccountInfo(), 
					pUserID, 
					pUniqueID, 
					pCertificate, 
					pName, 
					nAge, 
					nSex, 
					bFreeLoginIP, 
					nChecksumPack,
					bCheckPremiumIP,
					szIP,
					dwIP,
					strCountryCode3);
	PostAsyncJob(pNewJob);

	if (pAuthInfo)
	{
		delete pAuthInfo; pAuthInfo = NULL;
	}
}
*/

void CCMatchServer::OnMatchLoginFromNetmarbleJP(const CCUID& ComCCUID, const char* szLoginID, const char* szLoginPW, int nCmdVersion, unsigned long nChecksumPack)
{
	bool bFreeLoginIP = false;
	string strCountryCode3;

	// 프로토콜, 최대인원 체크
	if (!CheckOnLoginPre(ComCCUID, nCmdVersion, bFreeLoginIP, strCountryCode3)) return;

	// DBAgent에 먼저 보내고 응답을 받으면 로그인 프로세스를 진행한다.
	if (!CCGetLocale()->PostLoginInfoToDBAgent(ComCCUID, szLoginID, szLoginPW, bFreeLoginIP, nChecksumPack, GetClientCount()))
	{
		cclog( "Server user full(DB agent error).\n" );
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, MERR_CLIENT_FULL_PLAYERS);
		Post(pCmd);
		return;
	}
}

void CCMatchServer::OnMatchLoginFromDBAgent(const CCUID& ComCCUID, const char* szLoginID, const char* szName, int nSex, bool bFreeLoginIP, unsigned long nChecksumPack)
{
#ifndef LOCALE_NHNUSA
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(ComCCUID);
	if (pCommObj == NULL) return;

	string strCountryCode3;
	CheckIsValidIP( ComCCUID, pCommObj->GetIPString(), strCountryCode3, false );

	const char* pUserID = szLoginID;
	char szPassword[16] = "";			// 패스워드는 없다
	char szCertificate[16] = "";
	const char* pName = szName;
	int nAge = 20;

	bool bCheckPremiumIP = CCGetServerConfig()->CheckPremiumIP();
	const char* szIP = pCommObj->GetIPString();
	DWORD dwIP = pCommObj->GetIP();

	// Async DB
	CCAsyncDBJob_GetLoginInfo* pNewJob = new CCAsyncDBJob_GetLoginInfo(ComCCUID);
	pNewJob->Input(new CCMatchAccountInfo,
					new CCMatchAccountPenaltyInfo,
					pUserID, 
					szPassword, 
					szCertificate, 
					pName, 
					nAge, 
					nSex, 
					bFreeLoginIP, 
					nChecksumPack,
					bCheckPremiumIP,
					szIP,
					dwIP,
					strCountryCode3);
	PostAsyncJob(pNewJob);
#endif
}

void CCMatchServer::OnMatchLoginFailedFromDBAgent(const CCUID& ComCCUID, int nResult)
{
#ifndef LOCALE_NHNUSA
	// 프로토콜 버전 체크
	CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(ComCCUID, nResult);
	Post(pCmd);	
#endif
}

CCCommand* CCMatchServer::CreateCmdMatchResponseLoginOK(const CCUID& uidComm, 
													  CCUID& uidPlayer, 
													  const char* szUserID, 
													  CCMatchUserGradeID nUGradeID, 
													  CCMatchPremiumGradeID nPGradeID,
//													  const unsigned char* szRandomValue,
													  const unsigned char* pbyGuidReqMsg)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_RESPONSE_LOGIN, uidComm);
	pCmd->AddParameter(new CCCommandParameterInt(MOK));
	pCmd->AddParameter(new CCCommandParameterString(CCGetServerConfig()->GetServerName()));
	pCmd->AddParameter(new CCCommandParameterChar((char)CCGetServerConfig()->GetServerMode()));
	pCmd->AddParameter(new CCCommandParameterString(szUserID));
	pCmd->AddParameter(new CCCommandParameterUChar((unsigned char)nUGradeID));
	pCmd->AddParameter(new CCCommandParameterUChar((unsigned char)nPGradeID));
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	pCmd->AddParameter(new CCCommandParameterBool((bool)CCGetServerConfig()->IsEnabledSurvivalMode()));
	pCmd->AddParameter(new CCCommandParameterBool((bool)CCGetServerConfig()->IsEnabledDuelTournament()));
//	pCmd->AddParameter(new CCCommandParameterString(szRandomValue));

//	void* pBlob1 = CCMakeBlobArray(sizeof(unsigned char), 64);
//	unsigned char *pCmdBlock1 = (unsigned char*)CCGetBlobArrayElement(pBlob1, 0);
//	CopyMemory(pCmdBlock1, szRandomValue, 64);

//	pCmd->AddParameter(new CCCommandParameterBlob(pBlob1, CCGetBlobArraySize(pBlob1)));
//	CCEraseBlobArray(pBlob1);
	
	void* pBlob = CCMakeBlobArray(sizeof(unsigned char), SIZEOF_GUIDREQMSG);
	unsigned char* pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pCmdBlock, pbyGuidReqMsg, SIZEOF_GUIDREQMSG);

	pCmd->AddParameter(new CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);

	return pCmd;
}

CCCommand* CCMatchServer::CreateCmdMatchResponseLoginFailed(const CCUID& uidComm, const int nResult)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_RESPONSE_LOGIN, uidComm);
	pCmd->AddParameter(new CCCommandParameterInt(nResult));
	pCmd->AddParameter(new CCCommandParameterString(CCGetServerConfig()->GetServerName()));
	pCmd->AddParameter(new CCCommandParameterChar((char)CCGetServerConfig()->GetServerMode()));
	pCmd->AddParameter(new CCCommandParameterString("Ana"));
	pCmd->AddParameter(new CCCommandParameterUChar((unsigned char)CCMUGFREE));
	pCmd->AddParameter(new CCCommandParameterUChar((unsigned char)MMPG_FREE));
	pCmd->AddParameter(new CCCommandParameterUID(CCUID(0,0)));
	pCmd->AddParameter(new CCCommandParameterBool((bool)CCGetServerConfig()->IsEnabledSurvivalMode()));
	pCmd->AddParameter(new CCCommandParameterBool((bool)CCGetServerConfig()->IsEnabledDuelTournament()));
//	pCmd->AddParameter(new CCCommandParameterString("A"));
	
//	unsigned char tmp1 = 'A';
//	void* pBlob1 = CCMakeBlobArray(sizeof(unsigned char), sizeof(unsigned char));
//	unsigned char* pCmdBlock1 = (unsigned char*)CCGetBlobArrayElement(pBlob1, 0);
//	CopyMemory(pCmdBlock1, &tmp1, sizeof(unsigned char));
//	pCmd->AddParameter(new CCCommandParameterBlob(pBlob1, CCGetBlobArraySize(pBlob1)));
//	CCEraseBlobArray(pBlob1);

	unsigned char tmp = 0;
	void* pBlob = CCMakeBlobArray(sizeof(unsigned char), sizeof(unsigned char));
	unsigned char* pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pCmdBlock, &tmp, sizeof(unsigned char));

	pCmd->AddParameter(new CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);

	return pCmd;
}


bool CCMatchServer::AddObjectOnMatchLogin(const CCUID& uidComm, 
										const CCMatchAccountInfo* pSrcAccountInfo,
										const CCMatchAccountPenaltyInfo* pSrcAccountPenaltyInfo,
										bool bFreeLoginIP, string strCountryCode3, unsigned long nChecksumPack)
{
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(uidComm);
	if (pCommObj == NULL) return false;

	CCUID AllocUID = uidComm;
	int nErrCode = ObjectAdd(uidComm);
	if(nErrCode!=MOK) {
		LOG(LOG_DEBUG, MErrStr(nErrCode) );
	}

	CCMatchObject* pObj = GetObject(AllocUID);
	if (pObj == NULL) {
		// Notify Message 필요 -> 로그인 관련 - 해결(Login Fail 메세지 이용)
		// Disconnect(uidComm);
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(AllocUID, MERR_FAILED_LOGIN_RETRY);
		Post(pCmd);	
		return false;
	}

	pObj->AddCommListener(uidComm);
	pObj->SetObjectType(CCOT_PC);

	memcpy(pObj->GetAccountInfo(), pSrcAccountInfo, sizeof(CCMatchAccountInfo));
	memcpy(pObj->GetAccountPenaltyInfo(), pSrcAccountPenaltyInfo, sizeof(CCMatchAccountPenaltyInfo));
		
	pObj->SetFreeLoginIP(bFreeLoginIP);
	pObj->SetCountryCode3( strCountryCode3 );
	pObj->UpdateTickLastPacketRecved();
	pObj->UpdateLastHShieldMsgRecved();

	if (pCommObj != NULL)
	{
		pObj->SetPeerAddr(pCommObj->GetIP(), pCommObj->GetIPString(), pCommObj->GetPort());
	}
	
	SetClientClockSynchronize(uidComm);

	// 프리미엄 IP를 체크한다.
	if (CCGetServerConfig()->CheckPremiumIP())
	{
		if (pCommObj)
		{
			bool bIsPremiumIP = false;
			bool bExistPremiumIPCache = false;
			
			bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(pCommObj->GetIP(), bIsPremiumIP);

			// 만약 캐쉬에 없으면 직접 DB에서 찾도록 한다.
			if (!bExistPremiumIPCache)
			{
				if (m_MatchDBMgr.CheckPremiumIP(pCommObj->GetIPString(), bIsPremiumIP))
				{
					// 결과를 캐쉬에 저장
					MPremiumIPCache()->AddIP(pCommObj->GetIP(), bIsPremiumIP);
				}
				else
				{
					MPremiumIPCache()->OnDBFailed();
				}

			}

			if (bIsPremiumIP) pObj->GetAccountInfo()->m_nPGrade = MMPG_PREMIUM_IP;
		}		
	}

	if (!PreCheckAddObj(uidComm))
	{
		// 보안 관련 초기화 서버 설정에 문제가 생겼다고 로그인 실패를 리턴한다. //
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(uidComm, MERR_FAILED_AUTHENTICATION);
		Post(pCmd);	
		return false;
	}

	CCCommand* pCmd = CreateCmdMatchResponseLoginOK(uidComm, 
												   AllocUID, 
												   pObj->GetAccountInfo()->m_szUserID,
												   pObj->GetAccountInfo()->m_nUGrade,
                                                   pObj->GetAccountInfo()->m_nPGrade,
//												   pObj->GetAntiHackInfo()->m_szRandomValue,
												   pObj->GetHShieldInfo()->m_pbyGuidReqMsg);
	Post(pCmd);	

	// 접속 로그를 남긴다.
	//m_MatchDBMgr.InsertConnLog(pObj->GetAccountInfo()->m_nAID, pObj->GetIPString(), pObj->GetCountryCode3() );

	// 접속 로그
	CCAsyncDBJob_InsertConnLog* pNewJob = new CCAsyncDBJob_InsertConnLog(uidComm);
	pNewJob->Input(pObj->GetAccountInfo()->m_nAID, pObj->GetIPString(), pObj->GetCountryCode3() );
	PostAsyncJob(pNewJob);

	// Client DataFile Checksum을 검사한다.
	// 2006.2.20 dubble. filelist checksum으로 변경
	unsigned long nChecksum = nChecksumPack ^ uidComm.High ^ uidComm.Low;
	if( CCGetServerConfig()->IsUseFileCrc() && !CCMatchAntiHack::CheckClientFileListCRC(nChecksum, pObj->GetUID()) && 
		!CCGetServerConfig()->IsDebugLoginIPList(pObj->GetIPString()) )
	{
		LOG(LOG_PROG, "Invalid filelist crc (%u) , UserID(%s)\n ", nChecksum, pObj->GetAccountInfo()->m_szUserID);
//		pObj->SetBadFileCRCDisconnectWaitInfo();
		pObj->DisconnectHacker( CCMHT_BADFILECRC);
	}
	/*
	if (nChecksum != GetItemFileChecksum()) {
		LOG(LOG_PROG, "Invalid ZItemChecksum(%u) , UserID(%s) ", nChecksum, pObj->GetAccountInfo()->m_szUserID);
		Disconnect(uidComm);
		return false;
	}
	*/

	pObj->LoginCompleted();

	return true;
}
