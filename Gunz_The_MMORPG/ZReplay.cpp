#include "stdafx.h"
#include "ZApplication.h"
#include "ZFile.h"
#include "ZGameClient.h"
#include "ZReplay.h"
#include "ZGame.h"
#include "ZNetCharacter.h"
#include "ZMyCharacter.h"
#include "ZPost.h"
#include "CCMatchUtil.h"
#include "ZRuleDuel.h"
#include "ZRuleDuelTournament.h"

// Added R347a
#include "ZGameInterface.h"

bool g_bTestFromReplay = false;

bool CreateReplayGame(char *filename)
{
	static char szLastFile[256] = "";
	char szBuf[256];
	if (filename != NULL) strcpy(szBuf, filename);
	else strcpy(szBuf, szLastFile);

	if (filename != NULL) strcpy(szLastFile, filename);

	ZReplayLoader loader;
	if (!loader.Load(szBuf)) return false;
	ZGetGame()->OnLoadReplay(&loader);

	return true;

}


void ConvertStageSettingNodeForReplay(const REPLAY_STAGE_SETTING_NODE* pSource, MSTAGE_SETTING_NODE* pTarget)
{
//	_ASSERT(sizeof(REPLAY_STAGE_SETTING_NODE) == sizeof(MSTAGE_SETTING_NODE));
//	memcpy(pTarget, pSource, sizeof(REPLAY_STAGE_SETTING_NODE));

	pTarget->uidStage = pSource->uidStage;
	strcpy(pTarget->szMapName, pSource->szMapName);
	pTarget->nMapIndex = pSource->nMapIndex;
	pTarget->nGameType = pSource->nGameType;
	pTarget->nRoundMax = pSource->nRoundMax;
	pTarget->nLimitTime = pSource->nLimitTime;
	pTarget->nLimitLevel = pSource->nLimitLevel;
	pTarget->nMaxPlayers = pSource->nMaxPlayers;
	pTarget->bTeamKillEnabled = pSource->bTeamKillEnabled;
	pTarget->bTeamWinThePoint = pSource->bTeamWinThePoint;
	pTarget->bForcedEntryEnabled = pSource->bForcedEntryEnabled;
}

void ConvertStageSettingNodeForRecord(const MSTAGE_SETTING_NODE* pSource, REPLAY_STAGE_SETTING_NODE* pTarget)
{
	pTarget->uidStage = pSource->uidStage;
	strcpy(pTarget->szMapName, pSource->szMapName);
	pTarget->nMapIndex = pSource->nMapIndex;
	pTarget->nGameType = pSource->nGameType;
	pTarget->nRoundMax = pSource->nRoundMax;
	pTarget->nLimitTime = pSource->nLimitTime;
	pTarget->nLimitLevel = pSource->nLimitLevel;
	pTarget->nMaxPlayers = pSource->nMaxPlayers;
	pTarget->bTeamKillEnabled = pSource->bTeamKillEnabled;
	pTarget->bTeamWinThePoint = pSource->bTeamWinThePoint;
	pTarget->bForcedEntryEnabled = pSource->bForcedEntryEnabled;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int ZReplayLoader::m_nVersion=0;

ZReplayLoader::ZReplayLoader() : m_fGameTime(0.0f)
{
	memset(&m_StageSetting, 0, sizeof(REPLAY_STAGE_SETTING_NODE));
}

bool ZReplayLoader::Load(const char* filename)
{
	ZFile* file = zfopen(filename);

	if(!file) return false;
	
	if (!LoadHeader(file)) return false;
	if (!LoadStageSetting(file)) return false;
	ChangeGameState();

	if(m_nVersion>=4)
		if (!LoadStageSettingEtc(file)) return false;

	if (!LoadCharInfo(file)) return false;
	if (!LoadCommandStream(file)) return false;

	zfclose(file);
	return true;
}

void ZReplayLoader::ChangeGameState()
{
	MSTAGE_SETTING_NODE stageSetting;
	memset(&stageSetting, 0, sizeof(MSTAGE_SETTING_NODE));
	ConvertStageSettingNodeForReplay(&m_StageSetting, &stageSetting);
	ZGetGameClient()->GetMatchStageSetting()->UpdateStageSetting(&stageSetting);
	ZApplication::GetStageInterface()->SetMapName(ZGetGameClient()->GetMatchStageSetting()->GetMapName());
	ZGetGameInterface()->SetState(GUNZ_GAME);
	ZGetCharacterManager()->Clear();
	ZGetObjectManager()->Clear();

#ifdef _REPLAY_TEST_LOG
	cclog("[Replay Start]\n");
	cclog("[Map:%s, LimitTime:%d, GemeType:%d]\n", stageSetting.szMapName, stageSetting.nLimitTime, stageSetting.nGameType);
#endif

}

bool ZReplayLoader::LoadHeader(ZFile* file)
{
	unsigned int version = 0;
	unsigned int header;
	int nRead;

	nRead = zfread(&header, sizeof(header), 1, file);
	if(nRead==0 || header!=GUNZ_REC_FILE_ID) return false;

	nRead = zfread(&version, sizeof(version), 1, file);
	if ( ( nRead == 0) || ( version > GUNZ_REC_FILE_VERSION))
		return false;

	m_nVersion = version;

	return true;
}

bool ZReplayLoader::LoadStageSetting(ZFile* file)
{
	// stage setting
	int nRead = zfread(&m_StageSetting, sizeof(REPLAY_STAGE_SETTING_NODE), 1, file);
	if(nRead==0) return false;

	return true;
}

bool ZReplayLoader::LoadStageSettingEtc(ZFile* file)
{
	// 게임룰 별 추가 세팅값 로드
	if(m_StageSetting.nGameType==CCMATCH_GAMETYPE_DUEL)
	{
		ZRuleDuel* pDuel = (ZRuleDuel*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		int nRead = zfread(&pDuel->QInfo,sizeof(CCTD_DuelQueueInfo),1,file);
		if(nRead==0) return false;
	}
	if(m_StageSetting.nGameType==CCMATCH_GAMETYPE_DUELTOURNAMENT)
	{
		int tournamentType;
		int nRead = zfread(&tournamentType,sizeof(int),1,file);
		if(nRead==0) return false;

		int nCount = 0;
		nRead = zfread(&nCount,sizeof(int),1,file);
		if(nRead==0) return false;

		vector<DTPlayerInfo> vecPlayerInfo;
		DTPlayerInfo temp;
		for (int i=0; i<nCount; ++i)
		{
			nRead = zfread(&temp,sizeof(DTPlayerInfo),1,file);
			if(nRead==0) return false;
			vecPlayerInfo.push_back(temp);
		}

		ZGetGameInterface()->SetDuelTournamentCharacterList((CCDUELTOURNAMENTTYPE)tournamentType, vecPlayerInfo);
		// 원래 게임에서는 이미 캐릭터목록이 세팅된 상태로 룰이 생성되는데, 리플레이는 순서가 반대라서 캐릭터 목록을 가져가도록 직접 지시해야한다..
		ZRuleDuelTournament* pRule = (ZRuleDuelTournament*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		pRule->InitCharacterList();

		// 녹화시작 당시의 경기는 m_DTGameInfo 커맨드를 이미 받아버린 상태였기때문에 따로 로드해서 넣어줘야 에너지바가 제대로 출력된다
		nRead = zfread(&pRule->m_DTGameInfo,sizeof(CCTD_DuelTournamentGameInfo),1,file);
		if(nRead==0) return false;
	}
	return true;
}

void ConvertCharInfo_v0_to_v2(CCTD_CharInfo_v2* v2, CCTD_CharInfo_v0* v0)
{
	// 변수 하나 추가된 것이므로 그냥 덮어씌우고 추가된 변수 초기화
	memcpy(v2, v0, sizeof(CCTD_CharInfo_v0));
	v2->nClanCLID = 0;
};

void ConvertCharInfo_v2_to_v5(CCTD_CharInfo_v5* v5, CCTD_CharInfo_v2* v2)
{
	memcpy(v5, v2, sizeof(CCTD_CharInfo_v2));
	v5->nDTLastWeekGrade = 0;
}

void ConvertCharInfo_v5_to_curr(CCTD_CharInfo* curr, CCTD_CharInfo_v5* v5)
{
	// 일단 덮어씌우고
	memcpy(curr, v5, sizeof(CCTD_CharInfo_v5));
	// nEquipedItemDesc[MMCIP_END] 에서 MMCIP_END가 변경됐으므로 직접 복사
	memset(curr->nEquipedItemDesc, 0, sizeof(curr->nEquipedItemDesc));
	for (int i=0; i<MMCIP_CUSTOM2+1; ++i)
		curr->nEquipedItemDesc[i] = v5->nEquipedItemDesc[i];
	// 그 뒤의 멤버변수 다시 복사
	curr->nUGradeID		= v5->nUGradeID;
	curr->nClanCLID		= v5->nClanCLID;
	curr->nDTLastWeekGrade	= v5->nDTLastWeekGrade;
	
	// 새로 추가된 변수들은 그냥 기본값으로 세팅
	memset(curr->uidEquipedItem, 0, sizeof(curr->uidEquipedItem));
	
	for (int i=0; i<MMCIP_END; ++i)
		curr->nEquipedItemCount[i] = 1;
}

void ConvertCharInfo(CCTD_CharInfo* currCharInfo, void* oldCharInfo, int nVerOld)
{
	CCTD_CharInfo_v2 v2;
	CCTD_CharInfo_v5 v5;

	switch (nVerOld)
	{
	case 0: case 1:
		ConvertCharInfo_v0_to_v2(&v2, (CCTD_CharInfo_v0*)oldCharInfo);
		oldCharInfo = &v2;

	case 2: case 3: case 4:
		ConvertCharInfo_v2_to_v5(&v5, (CCTD_CharInfo_v2*)oldCharInfo);
		oldCharInfo = &v5;

	case 5:
		ConvertCharInfo_v5_to_curr(currCharInfo, (CCTD_CharInfo_v5*)oldCharInfo);
	}
	// case에 break가 없는 것은 의도된 것임

	if (nVerOld == GUNZ_REC_FILE_VERSION)
		memcpy(currCharInfo, oldCharInfo, sizeof(CCTD_CharInfo));
}

bool ZReplayLoader::LoadCharInfo(ZFile* file)
{
	int nRead;

	// character info
	int nCharacterCount;
	zfread(&nCharacterCount, sizeof(nCharacterCount), 1, file);

	for(int i = 0; i < nCharacterCount; i++)
	{
		bool bHero;
		nRead = zfread(&bHero, sizeof(bHero), 1, file);
		if(nRead != 1) return false;

		CCTD_CharInfo info;

		switch (m_nVersion)
		{
		case 0: case 1:
			{
				CCTD_CharInfo_v0 old;
				nRead = zfread(&old, sizeof(old), 1, file);
				if(nRead != 1) return false;
				ConvertCharInfo(&info, &old, m_nVersion);
			}
			break;
		case 2: case 3: case 4:
			{
				CCTD_CharInfo_v2 old;
				nRead = zfread(&old, sizeof(old), 1, file);
				if(nRead != 1) return false;
				ConvertCharInfo(&info, &old, m_nVersion);
			}
			break;
		case 5:
			{
				CCTD_CharInfo_v5 old;
				nRead = zfread(&old, sizeof(old), 1, file);
				if(nRead != 1) return false;
				ConvertCharInfo(&info, &old, m_nVersion);
			}
			break;
		case GUNZ_REC_FILE_VERSION:
			{
				nRead = zfread(&info, sizeof(info), 1, file);
				if(nRead != 1) return false;
			}
		}

		ZCharacter* pChar=NULL;
		if(bHero)
		{
			ZGetGame()->m_pMyCharacter = new ZMyCharacter;			
			ZGetGame()->CreateMyCharacter(&info/*, NULL*/);			///< TodoH(상) - 리플레이 관련..
			pChar=ZGetGame()->m_pMyCharacter;
			pChar->Load(file,m_nVersion);
		}else
		{
			pChar=new ZNetCharacter;
			pChar->Load(file,m_nVersion);
			pChar->Create(&info/*, NULL*/);							///< TodoH(상) - 리플레이 관련..
		}
		ZGetCharacterManager()->Add(pChar);

		pChar->SetVisible(true);
#ifdef _REPLAY_TEST_LOG
		cclog("[Add Character %s(%d)]\n", pChar->GetUserName());
#endif

	}

	return true;
}

bool ZReplayLoader::LoadCommandStream(ZFile* file)
{
	float fGameTime;
	zfread(&fGameTime, sizeof(fGameTime), 1, file);
	m_fGameTime = fGameTime;

	int nCommandCount=0;

	int nSize;
	float fTime;
	while( zfread(&fTime, sizeof(fTime), 1, file) )
	{
		nCommandCount++;

		char CommandBuffer[1024];

		CCUID uidSender;
		zfread(&uidSender, sizeof(uidSender), 1, file);
		zfread(&nSize, sizeof(nSize), 1, file);

		if(nSize<0 || nSize>sizeof(CommandBuffer)) {
			return false;
		}
		zfread(CommandBuffer, nSize, 1, file);


		ZObserverCommandItem *pZCommand=new ZObserverCommandItem;
		pZCommand->pCommand= CreateCommandFromStream(CommandBuffer);
		pZCommand->pCommand->m_Sender=uidSender;
		pZCommand->fTime=fTime;
		ZGetGame()->GetReplayCommandList()->push_back(pZCommand);

#ifdef _DEBUG
		if( MC_PEER_HPAPINFO == pZCommand->pCommand->GetID() )
		{
		}
#endif

	}

	return true;
}


CCCommand* ZReplayLoader::CreateCommandFromStream(char* pStream)
{
	if (m_nVersion <= 2)
	{
		return CreateCommandFromStreamVersion2(pStream);
	}

	CCCommand* pCommand = new CCCommand;
	pCommand->SetData(pStream, ZGetGameClient()->GetCommandManager());
	return pCommand;
}


CCCommand* ZReplayLoader::CreateCommandFromStreamVersion2(char* pStream)
{
	CCCommandManager* pCM = ZGetGameClient()->GetCommandManager();

	CCCommand* pCommand = new CCCommand;
	
	BYTE nParamCount = 0;
	unsigned short int nDataCount = 0;

	// Get Total Size
	unsigned short nTotalSize = 0;
	memcpy(&nTotalSize, pStream, sizeof(nTotalSize));
	nDataCount += sizeof(nTotalSize);

	// Command
	unsigned short int nCommandID = 0;
	memcpy(&nCommandID, pStream+nDataCount, sizeof(nCommandID));
	nDataCount += sizeof(nCommandID);

	CCCommandDesc* pDesc = pCM->GetCommandDescByID(nCommandID);
	if (pDesc == NULL)
	{
		cclog("Error(CCCommand::SetData): Wrong Command ID(%d)\n", nCommandID);
		_ASSERT(0);

		return pCommand;
	}
	pCommand->SetID(pDesc);

	if (ParseVersion2Command(pStream+nDataCount, pCommand))
	{
		return pCommand;
	}

	// Parameters
	memcpy(&nParamCount, pStream+nDataCount, sizeof(nParamCount));
	nDataCount += sizeof(nParamCount);
	for(int i=0; i<nParamCount; i++)
	{
		BYTE nType;
		memcpy(&nType, pStream+nDataCount, sizeof(BYTE));
		nDataCount += sizeof(BYTE);

		CCCommandParameter* pParam = MakeVersion2CommandParameter((CCCommandParameterType)nType, pStream, &nDataCount);
		if (pParam == NULL) return false;
		
		pCommand->m_Params.push_back(pParam);
	}

	return pCommand;
}

bool ZReplayLoader::ParseVersion2Command(char* pStream, CCCommand* pCmd)
{
	switch (pCmd->GetID())
	{
	case MC_PEER_HPINFO:
	case MC_PEER_HPAPINFO:
	case MC_MATCH_OBJECT_CACHE:
	case MC_MATCH_STAGE_ENTERBATTLE:
	case MC_MATCH_STAGE_LIST:
	case MC_MATCH_CHANNEL_RESPONSE_PLAYER_LIST:
	case MC_MATCH_GAME_RESPONSE_SPAWN:
	case MC_PEER_DASH:
	case MC_MATCH_BRIDGEPEER:
	case MC_MATCH_SPAWN_WORLDITEM:
		{

		}
		break;
	default:
		return false;
	};

	BYTE nParamCount = 0;
	unsigned short int nDataCount = 0;
	vector<CCCommandParameter*> TempParams;

	// Count
	memcpy(&nParamCount, pStream+nDataCount, sizeof(nParamCount));
	nDataCount += sizeof(nParamCount);

	for(int i=0; i<nParamCount; i++)
	{
		BYTE nType;
		memcpy(&nType, pStream+nDataCount, sizeof(BYTE));
		nDataCount += sizeof(BYTE);

		CCCommandParameter* pParam = MakeVersion2CommandParameter((CCCommandParameterType)nType, pStream, &nDataCount);
		if (pParam == NULL) return false;
		
		TempParams.push_back(pParam);
	}


	switch (pCmd->GetID())
	{
	case MC_PEER_HPAPINFO:
		{
			void* pBlob = TempParams[1]->GetPointer();
			struct REPLAY2_HP_AP_INFO 
			{
				CCUID CCUID;
				float fHP;
				float fAP;
			};

			REPLAY2_HP_AP_INFO* pBlobData = (REPLAY2_HP_AP_INFO*)CCGetBlobArrayElement(pBlob, 0);
			pCmd->AddParameter(new CCCmdParamFloat(pBlobData->fHP));
			pCmd->AddParameter(new CCCmdParamFloat(pBlobData->fAP));
		}
		break;
	case MC_PEER_HPINFO:
		{
			void* pBlob = TempParams[1]->GetPointer();
			struct REPLAY2_HP_INFO 
			{
				CCUID CCUID;
				float fHP;
			};

			REPLAY2_HP_INFO* pBlobData = (REPLAY2_HP_INFO*)CCGetBlobArrayElement(pBlob, 0);
			pCmd->AddParameter(new CCCmdParamFloat(pBlobData->fHP));
		}
		break;
	case MC_MATCH_OBJECT_CACHE:
		{
			unsigned int nType;
			TempParams[0]->GetValue(&nType);
			CCCmdParamBlob* pBlobParam = ((CCCmdParamBlob*)TempParams[1])->Clone();

			pCmd->AddParameter(new CCCmdParamUChar((unsigned char)nType));
			pCmd->AddParameter(pBlobParam);
		}
		break;
	case MC_MATCH_STAGE_ENTERBATTLE:
		{
			CCUID uidPlayer, uidStage;
			int nParam;
			
			TempParams[0]->GetValue(&uidPlayer);
			TempParams[1]->GetValue(&uidStage);
			TempParams[2]->GetValue(&nParam);

			struct REPLAY2_ExtendInfo
			{
				char			nTeam;
				unsigned char	nPlayerFlags;
				unsigned char	nReserved1;
				unsigned char	nReserved2;
			};

			struct REPLAY2_PeerListNode
			{
				CCUID				uidChar;
				char				szIP[64];
				unsigned int		nPort;
				CCTD_CharInfo		CharInfo;
				REPLAY2_ExtendInfo	ExtendInfo;
			};


			void* pBlob = TempParams[3]->GetPointer();
			//int nCount = CCGetBlobArrayCount(pBlob);
			REPLAY2_PeerListNode* pNode = (REPLAY2_PeerListNode*)CCGetBlobArrayElement(pBlob, 0);


			void* pNewBlob = CCMakeBlobArray(sizeof(CCTD_PeerListNode), 1);
			CCTD_PeerListNode* pNewNode = (CCTD_PeerListNode*)CCGetBlobArrayElement(pNewBlob, 0);
			pNewNode->uidChar = pNode->uidChar;
			pNewNode->dwIP = inet_addr(pNode->szIP);
			pNewNode->nPort = pNode->nPort;
			memcpy(&pNewNode->CharInfo, &pNode->CharInfo, sizeof(CCTD_CharInfo));
			pNewNode->ExtendInfo.nTeam = pNode->ExtendInfo.nTeam;
			pNewNode->ExtendInfo.nPlayerFlags = pNode->ExtendInfo.nPlayerFlags;
			pNewNode->ExtendInfo.nReserved1 = pNode->ExtendInfo.nReserved1;
			pNewNode->ExtendInfo.nReserved2 = pNode->ExtendInfo.nReserved1;
			

			pCmd->AddParameter(new CCCmdParamUChar((unsigned char)nParam));
			pCmd->AddParameter(new CCCommandParameterBlob(pNewBlob, CCGetBlobArraySize(pNewBlob)));

			CCEraseBlobArray(pNewBlob);
		}
		break;
	case MC_MATCH_STAGE_LIST:
		{
			_ASSERT(0);
		}
		break;
	case MC_MATCH_CHANNEL_RESPONSE_PLAYER_LIST:
		{
			_ASSERT(0);
		}
		break;
	case MC_MATCH_GAME_RESPONSE_SPAWN:
		{
			CCUID uidChar;
			rvector pos, dir;

			TempParams[0]->GetValue(&uidChar);
			TempParams[1]->GetValue(&pos);
			TempParams[2]->GetValue(&dir);

			pCmd->AddParameter(new CCCmdParaCCUID(uidChar));
			pCmd->AddParameter(new CCCmdParamShortVector(pos.x, pos.y, pos.z));
			pCmd->AddParameter(new CCCmdParamShortVector(DirElementToShort(dir.x), DirElementToShort(dir.y), DirElementToShort(dir.z)));
		}
		break;
	case MC_PEER_DASH:
		{
			rvector pos, dir;
			int nSelType;

			TempParams[0]->GetValue(&pos);
			TempParams[1]->GetValue(&dir);
			TempParams[2]->GetValue(&nSelType);

			ZPACKEDDASHINFO pdi;
			pdi.posx = Roundf(pos.x);
			pdi.posy = Roundf(pos.y);
			pdi.posz = Roundf(pos.z);

			pdi.dirx = DirElementToShort(dir.x);
			pdi.diry = DirElementToShort(dir.y);
			pdi.dirz = DirElementToShort(dir.z);

			pdi.seltype = (BYTE)nSelType;

			pCmd->AddParameter(new CCCommandParameterBlob(&pdi,sizeof(ZPACKEDDASHINFO)));
		}
		break;
	case MC_MATCH_SPAWN_WORLDITEM:
		{
			struct REPLAY2_WorldItem
			{
				unsigned short	nUID;
				unsigned short	nItemID;
				unsigned short  nItemSubType;
				float			x;
				float			y;
				float			z;
			};


			void* pBlob = TempParams[0]->GetPointer();
			int nCount = CCGetBlobArrayCount(pBlob);

			void* pNewBlob = CCMakeBlobArray(sizeof(CCTD_WorldItem), nCount);

			for (int i = 0; i < nCount; i++)
			{
				REPLAY2_WorldItem* pNode = (REPLAY2_WorldItem*)CCGetBlobArrayElement(pBlob, i);
				CCTD_WorldItem* pNewNode = (CCTD_WorldItem*)CCGetBlobArrayElement(pNewBlob, i);

				pNewNode->nUID = pNode->nUID;
				pNewNode->nItemID = pNode->nItemID;
				pNewNode->nItemSubType = pNode->nItemSubType;
				pNewNode->x = (short)Roundf(pNode->x);
				pNewNode->y = (short)Roundf(pNode->y);
				pNewNode->z = (short)Roundf(pNode->z);
			}
			pCmd->AddParameter(new CCCommandParameterBlob(pNewBlob, CCGetBlobArraySize(pNewBlob)));
			CCEraseBlobArray(pNewBlob);

		}
		break;
	case MC_MATCH_BRIDGEPEER:
		{
			_ASSERT(0);
		}
		break;
	};


	for(int i=0; i<(int)TempParams.size(); i++){
		delete TempParams[i];
	}
	TempParams.clear();


	return true;
}


CCCommandParameter* ZReplayLoader::MakeVersion2CommandParameter(CCCommandParameterType nType, char* pStream, unsigned short int* pnDataCount)
{
	CCCommandParameter* pParam = NULL;

	switch(nType) 
	{
	case MPT_INT:
		pParam = new CCCommandParameterInt;
		break;
	case MPT_UINT:
		pParam = new CCCommandParameterUInt;
		break;
	case MPT_FLOAT:
		pParam = new CCCommandParameterFloat;
		break;
	case MPT_STR:
		{
			pParam = new CCCommandParameterString;
			CCCommandParameterString* pStringParam = (CCCommandParameterString*)pParam;

			char* pStreamData = pStream+ *pnDataCount;

			int nValueSize = 0;
			memcpy(&nValueSize, pStreamData, sizeof(nValueSize));
			pStringParam->m_Value = new char[nValueSize];
			memcpy(pStringParam->m_Value, pStreamData+sizeof(nValueSize), nValueSize);
			int nParasSize = nValueSize+sizeof(nValueSize);

			*pnDataCount += nParasSize;
			return pParam;
		}
		break;
	case MPT_VECTOR:
		pParam = new CCCommandParameterVector;
		break;
	case MPT_POS:
		pParam = new CCCommandParameterPos;
		break;
	case MPT_DIR:
		pParam = new CCCommandParameterDir;
		break;
	case MPT_BOOL:
		pParam = new CCCommandParameterBool;
		break;
	case MPT_COLOR:
		pParam = new CCCommandParameterColor;
		break;
	case MPT_UID:
		pParam = new CCCommandParameterUID;
		break;
	case MPT_BLOB:
		pParam = new CCCommandParameterBlob;
		break;
	case MPT_CHAR:
		pParam = new CCCommandParameterChar;
		break;
	case MPT_UCHAR:
		pParam = new CCCommandParameterUChar;
		break;
	case MPT_SHORT:
		pParam = new CCCommandParameterShort;
		break;
	case MPT_USHORT:
		pParam = new CCCommandParameterUShort;
		break;
	case MPT_INT64:
		pParam = new CCCommandParameterInt64;
		break;
	case MPT_UINT64:
		pParam = new CCCommandParameterUInt64;
		break;
	default:
		cclog("Error(CCCommand::SetData): Wrong Param Type\n");
		_ASSERT(false);		// Unknow Parameter!!!
		return NULL;
	}

	*pnDataCount += pParam->SetData(pStream+ *pnDataCount);

	return pParam;
}