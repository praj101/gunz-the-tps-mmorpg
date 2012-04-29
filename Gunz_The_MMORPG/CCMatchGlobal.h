#pragma once
#include "MUID.h"
#include "CCBaseLocale.h"

#define MAX_CHAR_COUNT							4		//number of characters that can be created

#define MAX_TRAP_THROWING_LIFE					10.f	//thrown if the trap is not activated within this time handled by dud

#define CYCLE_STAGE_UPDATECHECKSUM				500		//bangri show host information changes the delay - 0.5 seconds

//Game related Rule
#define NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS		3		//team won only three more than an experience bonus is applied
#define RESPAWN_DELAYTIME_AFTER_DYING			7000	//The default delay time to die, then respawn
#define RESPAWN_DELAYTIME_AFTER_DYING_MIN		2000	//The minimum delay time to die, then respawn
#define RESPAWN_DELAYTIME_AFTER_DYING_MAX		20000	//The maximum delay time to die, then respawn

#define MAX_XP_BONUS_RATIO						2.0f	//exp bonus up to 2 times.
#define MAX_BP_BONUS_RATIO						2.0f	//Bounty Bonus up to two times.

#define TRANS_STAGELIST_NODE_COUNT				8		//send to the client at a time, the number of nodes on the Stage
#define TRANS_STANDBY_CLANLIST_NODE_COUNT		4		//clan I send to the client at a time in the atmosphere, the number of clans


#define MAX_REPLIER	16			//proposal up to 16 people who answered

#define CLAN_SPONSORS_COUNT						4		//Clan Wed promoters needed to generate
#define CLAN_CREATING_NEED_BOUNTY				1000	//Clan Bounty required to generate
#define CLAN_CREATING_NEED_LEVEL				10		//clan level needed to generate



#define MAX_CHAR_LEVEL							99

#define MATCH_SIMPLE_DESC_LENGTH				64


//Action League
#define ACTIONLEAGUE_TEAM_MEMBER_COUNT			4		//Action League will have to play with all four.
#define MAX_LADDER_TEAM_MEMBER					4		//ladder team can make up to 1-4.
#define MAX_CLANBATTLE_TEAM_MEMBER				8		//clan I can make up to 8 people.

//#define LIMIT_ACTIONLEAGUE		//Netmarble Action League Dedicated dipain
//#define LEAK_TEST
#define CLAN_BATTLE					//dipain Development - Development will disappear at the end of 





enum CCMatchTeam
{
	CCMT_ALL			= 0,
	CCMT_SPECTATOR	= 1,
	CCMT_RED			= 2,
	CCMT_BLUE		= 3,
	CCMT_END
};


// 서버모드
enum CCMatchServerMode
{
	MSM_NORMAL		= 0,		//General
	MSM_CLAN		= 1,		//dedicated servers
	MSM_LADDER		= 2,		//Ladder dedicated servers
	MSM_EVENT		= 3,		//the event server
	MSM_TEST		= 4,		//test server
	MSM_MAX,

	MSM_ALL			= 100,		//event will be used only.
};

//Accept the relevant
enum CCMatchProposalMode
{
	MPROPOSAL_NONE = 0,				//not used
	MPROPOSAL_LADDER_INVITE,		//ladder game request
	MPROPOSAL_CLAN_INVITE,			//request
	MPROPOSAL_END
};


//Ladder type
enum CCLADDERTYPE {
	CCLADDERTYPE_NORMAL_2VS2		= 0,
	CCLADDERTYPE_NORMAL_3VS3,
	CCLADDERTYPE_NORMAL_4VS4,
//	CCLADDERTYPE_NORMAL_8VS8,
#ifdef _DEBUG
	CCLADDERTYPE_NORMAL_1VS1,
#endif
	CCLADDERTYPE_MAX
};

//Type is required each ladder Adults
const int g_nNeedLadderMemberCount[CCLADDERTYPE_MAX] = {	2, 3, 4/*, 8*/
#ifdef _DEBUG
, 1 
#endif
};


//Clan related.
#define DEFAULT_CLAN_POINT			1000			//Default clan points
#define DAY_OF_DELETE_CLAN			(7)				//Clan pyeswaeyocheonghu DAY_OF_DELETE_CLAN ilmankeum jinhaengdoem later, underground operations.
#define MAX_WAIT_CLAN_DELETE_HOUR	(24)			//DAY_OF_DELETE_CLAN + MAX_WAIT_CLAN_DELETE_HOUR Divi then deleted from the clan.
#define UNDEFINE_DELETE_HOUR		(2000000000)	//null for normal DeleteTime gapcheoriyong Clan.

enum CCMatchClanDeleteState
{
	CCMCDS_NORMAL = 1,
	CCMCDS_WAIT,
	CCMCDS_DELETE,

	CCMCDS_END,
};


//Option-related
enum CCBITFLAG_USEROPTION {
	CCBITFLAG_USEROPTION_REJECT_WHISPER	= 1,
	CCBITFLAG_USEROPTION_REJECT_INVITE	= 1<<1
};

//Quest related ///////////////////////////////////////////////////////////////////////////////

#define MAX_QUEST_MAP_SECTOR_COUNT				16			//quest to create the maximum number of maps that can be

#ifdef _DEBUG_QUEST
#define MAX_QUEST_NPC_INFO_COUNT				100			//Quest NPC come up to the number of kinds of
#else
#define MAX_QUEST_NPC_INFO_COUNT				14			//NPC in the quest come up to the number of kinds of
#endif



#define ALL_PLAYER_NOT_READY					1	//all users did not have a ready Could not start the game.
#define QUEST_START_FAILED_BY_SACRIFICE_SLOT	2	//expense items in the slot scan has encountered a problem and failed to start.
#define INVALID_TACKET_USER						3	//user may not have tickets.
#define INVALID_MAP								4	//not available in the current game type maepim.

#define MIN_QUESTITEM_ID							200001	//item id is a quest item 200001 Beginning
#define MAX_QUESTITEM_ID							299999

// Keeper Manager with the relevant Schedule. ////////////////////////////////////////////////////////////

enum KMS_SCHEDULE_TYPE
{
	KMST_NO = 0,
	KMST_REPEAT,
	KMST_COUNT,
	KMST_ONCE,

	KMS_SCHEDULE_TYPE_END,
};

enum KMS_COMMAND_TYPE
{
	KMSC_NO = 0,
	KMSC_ANNOUNCE,
	KMSC_STOP_SERVER,
	KMSC_RESTART_SERVER,
	
	KMS_COMMAND_TYPE_END,
};

enum SERVER_STATE_KIND
{
	SSK_OPENDB = 0,

	SSK_END,
};

enum SERVER_ERR_STATE
{
	SES_NO = 0,
	SES_ERR_DB,
    
	SES_END,
};

enum SERVER_TYPE
{
	ST_NULL = 0,
	ST_DEBUG,
	ST_NORMAL,
	ST_CLAN,
	ST_QUEST,
	ST_EVENT,
};


enum CCMatchBlockLevel
{
	CCMBL_NO = 0,
	CCMBL_ACCOUNT,
	CCMBL_LOGONLY,

	CCMBL_END,
};

/////////////////////////////////////////////////////////////////////////////////////////////
//Util function

//Return the other side
inline CCMatchTeam NegativeTeam(CCMatchTeam nTeam)
{
	if (nTeam == CCMT_RED) return CCMT_BLUE;
	else if (nTeam == CCMT_BLUE) return CCMT_RED;
	return nTeam;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// string buffer length define.

#define ANNOUNCE_STRING_LEN					64
#define MSG_STRING_LEN						64
#define CHAT_STRING_LEN						64
#define VOTE_DISCUSS_STRING_LEN				64
#define VOTE_ARG_STRING_LEN					64
// stage
#define STAGENAME_LENGTH					64	//bangyireum length  (bangyireum == name??)
#define STAGEPASSWD_LENGTH					8	//maximum length of password
#define STAGE_QUEST_MAX_PLAYER				4	//quest game mode Sleeps
#define STAGE_MAX_PLAYERCOUNT				16  //default mode (Quest, except for claims) Sleeps.
#define STAGE__MAX_ROUND					100	//maximum number of rounds that can be used on stage.
// clan
#define CLAN_NAME_LENGTH					16	//maximum length of a clan name - this changes, the protocol version must be changed.
												//This part fit in Divi inability because of the previous user
#define MIN_CLANNAME						4	//Minimum 4 jayisang 12 jayihaman can create a clan name.
#define MAX_CLANNAME						12
// character
#define MIN_CHARNAME						4	//Minimum 4 jayisang no more than 12 characters, character names can be made.
#define MAX_CHARNAME						12
#define MATCHOBJECT_NAME_LENGTH				32	//character name length - also not a choice as DB because of the replay. Modified to add a new side.
#define MAX_CHARNAME_LENGTH					24	//DB on the set length.
// chat room
#define MAX_CHATROOMNAME_STRING_LEN			64	
// #define MAX_USERID_STRING_LEN			20
#define MAX_USERID_STRING_LEN				21	//string length of 20 bytes + 1 byte terminator
#define MAX_USER_PASSWORD_STRING_LEN		20
#define USERNAME_STRING_LEN					50
// channel
#define CHANNELNAME_LEN						64
#define CHANNELRULE_LEN						64
// map
#define MAPNAME_LENGTH						32
// Auth
#define NHN_GAMEID							"cc_gunz"
#define NHN_AUTH_LENGTH						4096
#define NHN_OUTBUFF_LENGTH					1024
// item
#define MAX_ACCOUNT_ITEM					1000		//is a limit of up to 1000.
#define MAX_EXPIRED_ACCOUNT_ITEM			100
#define MAX_ITEM_COUNT						100			//that one person can have the maximum number of items (items purchased from stores)
#define MAX_QUEST_REWARD_ITEM_COUNT			500			//that one person can have the maximum number of items (for items acquired during quest)
#define MAX_SPENDABLE_ITEM_COUNT			999			//consumable items that can have the maximum number of
// gamble item 
#define MAX_GAMBLEITEMNAME_LEN				(65)		
#define MAX_GAMBLEITEMDESC_LEN				(65)		//add a simple description.
#define MAX_BUYGAMBLEITEM_ELAPSEDTIME_MIN	(5)			//duration of the event considering the restrictions of the time to buy in a store and not to exceed two hours.
// time define.
#define MIN_REQUEST_STAGESTART_TIME			(1 * 1000)			//1 seconds have passed after the start of the last stage can start again.
#define MIN_REQUEST_SUICIDE_TIME			(1000 * 60 * 3)		//3 minutes he was dead once the request can be requested.

//#define UPDATE_KILLTRACKER_ELAPSED_TIME	(1000 * 60 * 5)
//#define TRACKERINFO_LIFETIME				(1000 * 60 * 20)	
//#define KILLCOUNT_TRACE_TIME				(1000 * 60 * 10)
//#define MAX_KILLCOUNT_ON_TRACETIME		(40)

// md5 
#define MAX_MD5LENGH						(16)
/////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////
//Survival-related ranking
#define MAX_SURVIVAL_SCENARIO_COUNT 3
#define MAX_SURVIVAL_RANKING_LIST 10

typedef struct _RankingInfo
{
	char szCharName[MATCHOBJECT_NAME_LENGTH];
	DWORD dwCID;
	DWORD dwRanking;
	DWORD dwRankingPoint;
} RANKINGINFO;

//////////////////////////////////////////////////////////////////////////////////////////////
//Duel Tournament related
#define DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH 8

enum CCDUELTOURNAMENTTYPE
{
	CCDUELTOURNAMENTTYPE_FINAL				= 0,		//from the final
	CCDUELTOURNAMENTTYPE_SEMIFINAL,						//4 from Round
	CCDUELTOURNAMENTTYPE_QUATERFINAL,					//8 from Round
	CCDUELTOURNAMENTTYPE_MAX	
};

enum CCDUELTOURNAMENTROUNDSTATE {
	CCDUELTOURNAMENTROUNDSTATE_FINAL = 0,	
	CCDUELTOURNAMENTROUNDSTATE_SEMIFINAL,
	CCDUELTOURNAMENTROUNDSTATE_QUATERFINAL,
	CCDUELTOURNAMENTROUNDSTATE_MAX
};

enum CCDUELTOURNAMENTMATCHMAKINGFACTOR			//match is made based
{
	CCDUELTOURNAMENTMATCHMAKINGFACTOR_TPGAP = 0,			//TP gap suitable
	CCDUELTOURNAMENTMATCHMAKINGFACTOR_OVERWAIT,				//Wait Timeout
};

//Free time in the countdown to the state how to use chop (that combined full-time)
#define DUELTOURNAMENT_PRECOUNTDOWN_WINLOSE_SHOWTIME	4000
#define DUELTOURNAMENT_PRECOUNTDOWN_NEXTMATCH_SHOWTIME	6000

inline int GetDTPlayerCount(CCDUELTOURNAMENTTYPE nType)
{
	switch(nType) {
		case CCDUELTOURNAMENTTYPE_QUATERFINAL :		return 8;
		case CCDUELTOURNAMENTTYPE_SEMIFINAL :		return 4;
		case CCDUELTOURNAMENTTYPE_FINAL :			return 2;			
	}

	return 0;
}

inline int GetDTRoundCount(CCDUELTOURNAMENTROUNDSTATE nRoundState)
{
	switch(nRoundState) {
		case CCDUELTOURNAMENTROUNDSTATE_QUATERFINAL :		return 4;
		case CCDUELTOURNAMENTROUNDSTATE_SEMIFINAL :			return 2;
		case CCDUELTOURNAMENTROUNDSTATE_FINAL :				return 1;			
	}

	return 0;
}

typedef struct _DTRankingInfo
{
	char m_szCharName[MATCHOBJECT_NAME_LENGTH];
	int m_iTP;
	int m_iWins;
	int m_iLoses;
	int m_iRanking;
	int m_iRankingIncrease;
	int m_iFinalWins;
	int m_iGrade;
} DTRankingInfo;

typedef struct _DTPlayerInfo
{
	char m_szCharName[MATCHOBJECT_NAME_LENGTH];
	MUID uidPlayer;
	int m_iTP;
} DTPlayerInfo;


//////////////////////////////////////////////////////////////////////////////////////////////
//Items related to renewal

#define ACCOUNTITEM_INCREASE_LOG_MOVE			100
#define ACCOUNTITEM_INCREASE_LOG_BUY			101
#define ACCOUNTITEM_INCREASE_LOG_REWARD_BY_GM	102

#define ACCOUNTITEM_DECREASE_LOG_MOVE			200
#define ACCOUNTITEM_DECREASE_LOG_SELL			201
#define ACCOUNTITEM_DECREASE_LOG_EXPIRE			202

#define CHARITEM_INCREASE_LOG_MOVE		100
#define CHARITEM_INCREASE_LOG_BUY		101
#define CHARITEM_INCREASE_LOG_REWARD	102

#define CHARITEM_DECREASE_LOG_MOVE		200
#define CHARITEM_DECREASE_LOG_SELL		201
#define CHARITEM_DECREASE_LOG_EXPIRE	202
#define CHARITEM_DECREASE_LOG_GAMBLE	203
#define CHARITEM_DECREASE_LOG_SPEND		204

//////////////////////////////////////////////////////////////////////////////////////////////

#define	MAX_CHARACTER_SHORT_BUFF_COUNT		2

typedef struct _CCLongBuffInfoInDB
{	
	int nCBID;
	int nBuffID;
	int nBuffSecondPeriod;
	int nStartPlayTime;	
} CCLongBuffInfoInDB;

typedef struct _CCLongBuffInfo
{	
	MUID uidBuff;

	int nCBID;
	int nBuffID;
	int nBuffPeriod;
	int nBuffPeriodRemainder;	
} CCLongBuffInfo;

typedef struct _CCShortBuffInfo
{	
	MUID uidBuff;

	int nBuffID;
	int nBuffPeriod;
	int nBuffPeriodRemainder;
} CCShortBuffInfo;