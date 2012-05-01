#pragma once
#include "CCCrypt.h"

#define PACKET_CRYPTER_KEY_LEN		32

struct CCPacketCrypterKey
{
	char szKey[PACKET_CRYPTER_KEY_LEN];
};

class CCPacketCrypter
{
private:
	CCPacketCrypterKey	m_Key;
	static int				m_nSHL;
	static unsigned char	m_ShlMask;

	static char _Enc(char s, char key);
	static char _Dec(char s, char key);
public:
	CCPacketCrypter();
	virtual ~CCPacketCrypter() {}
	virtual bool InitKey(CCPacketCrypterKey* pKey);
	virtual bool Encrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen);
	virtual bool Decrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen);
	virtual bool Encrypt(char* pSource, int nSrcLen);
	virtual bool Decrypt(char* pSource, int nSrcLen);
	const CCPacketCrypterKey* GetKey() { return &m_Key; }

	static void InitConst();
	static bool Encrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen, CCPacketCrypterKey* pKey);
	static bool Decrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen, CCPacketCrypterKey* pKey);
	static bool Encrypt(char* pSource, int nSrcLen, CCPacketCrypterKey* pKey);
	static bool Decrypt(char* pSource, int nSrcLen, CCPacketCrypterKey* pKey);
};
