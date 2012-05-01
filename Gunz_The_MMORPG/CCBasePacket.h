#pragma once
/////////////////////////////////////////////////////////////
//	BasePacket.h
//								 Programmed by Kim Young-Ho 
//								    LastUpdate : 2000/07/20
/////////////////////////////////////////////////////////////


#define SAFEUDP_FLAG_SAFE_PACKET		1
#define SAFEUDP_FLAG_CONTROL_PACKET	1 << 1
#define SAFEUDP_FLAG_ACK_PACKET		1 << 2
#define SAFEUDP_FLAG_LIGHT_PACKET	1 << 3	// NetNode 없이 송수신 가능


#pragma pack(1)


struct CCBasePacket {
	BYTE	nFlags;

	CCBasePacket()					{ nFlags = 0; }
	~CCBasePacket()					{ }
	BOOL GetFlag(BYTE nTFlag)		{ return (nFlags & nTFlag); }
	void SetFlag(BYTE nTFlag)		{ nFlags |= nTFlag; }
	void ResetFlags(BYTE nTFlag)	{ nFlags &= (0xffffffff ^ nTFlag); }
};

struct CCACKPacket : CCBasePacket {
	BYTE	nSafeIndex;

	CCACKPacket()		{ SetFlag(SAFEUDP_FLAG_ACK_PACKET); }
	~CCACKPacket()		{ }
};

struct CCNormalPacket : CCBasePacket {
	WORD	wMsg;
};

struct CCSafePacket : CCBasePacket {
	WORD	wMsg;
	BYTE	nSafeIndex;		// Using for SafePacket, Ignore on NormalPacket

	CCSafePacket()		{ SetFlag(SAFEUDP_FLAG_SAFE_PACKET); }
	~CCSafePacket()		{ }
};

struct CCLightPacket : CCBasePacket {
	WORD	wMsg;

	CCLightPacket()		{ SetFlag(SAFEUDP_FLAG_LIGHT_PACKET); }
	~CCLightPacket()		{ }
};

struct CCControlPacket : CCSafePacket {
public:
	enum CONTROL {
		CONTROL_SYN,
		CONTROL_SYN_RCVD,
		CONTROL_FIN,
		CONTROL_FIN_RCVD,
		CONTROL_ACK
	};

	CONTROL	nControl;

	CCControlPacket()	{ SetFlag(SAFEUDP_FLAG_CONTROL_PACKET); wMsg = 0; }
	~CCControlPacket()	{ }
};


#pragma pack()
