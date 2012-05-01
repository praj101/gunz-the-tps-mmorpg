#include "stdafx.h"
#include "CCPacketCrypter.h"
#include "CCPacket.h"

int CCPacketCrypter::m_nSHL = (MCOMMAND_VERSION % 6) + 1;
unsigned char CCPacketCrypter::m_ShlMask = 0;


////////////////////////////////////////////////////////////////////////
bool CCPacketCrypter::InitKey(CCPacketCrypterKey* pKey)
{
	memcpy(&m_Key, pKey, sizeof(CCPacketCrypterKey));
	return true;
}

bool CCPacketCrypter::Encrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen)
{
	CCPacketCrypter::Encrypt(pSource, nSrcLen, pTarget, nTarLen, &m_Key);
	return true;
}

bool CCPacketCrypter::Decrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen)
{
	CCPacketCrypter::Decrypt(pSource, nSrcLen, pTarget, nTarLen, &m_Key);
	return true;
}

bool CCPacketCrypter::Encrypt(char* pSource, int nSrcLen)
{
	CCPacketCrypter::Encrypt(pSource, nSrcLen, &m_Key);
	return true;
}

bool CCPacketCrypter::Decrypt(char* pSource, int nSrcLen)
{
	CCPacketCrypter::Decrypt(pSource, nSrcLen, &m_Key);
	return true;
}

bool CCPacketCrypter::Encrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen, CCPacketCrypterKey* pKey)
{
	int nKeyIndex = 0;
	for (int i = 0; i < nSrcLen; i++)
	{
		*pTarget = _Enc(*pSource, pKey->szKey[nKeyIndex]);

		nKeyIndex++;
		if (nKeyIndex >= PACKET_CRYPTER_KEY_LEN) nKeyIndex = 0;
		pTarget++;
		pSource++;
	}

	return true;
}

bool CCPacketCrypter::Decrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen, CCPacketCrypterKey* pKey)
{
	int nKeyIndex = 0;
	for (int i = 0; i < nSrcLen; i++)
	{
		*pTarget = _Dec(*pSource, pKey->szKey[nKeyIndex]);

		nKeyIndex++;
		if (nKeyIndex >= PACKET_CRYPTER_KEY_LEN) nKeyIndex = 0;
		pTarget++;
		pSource++;
	}

	return true;
}


char CCPacketCrypter::_Enc(char s, char key)
{
	WORD w;
	BYTE b, bh;
	b = s ^ key;
	w = b << m_nSHL;
	bh = (w&0xFF00)>>8;
	b = w&0xFF;
	return( BYTE( b | bh ) ^ 0xF0 );
}

char CCPacketCrypter::_Dec(char s, char key)
{
	BYTE b, bh, d;

	b = s^0xF0;
	bh = b&m_ShlMask;
	d = (bh<<(8-m_nSHL))|(b>>m_nSHL);

	return ( d ^ key );
}


bool CCPacketCrypter::Encrypt(char* pSource, int nSrcLen, CCPacketCrypterKey* pKey)
{
	int nKeyIndex = 0;
	for (int i = 0; i < nSrcLen; i++)
	{
		*pSource = _Enc(*pSource, pKey->szKey[nKeyIndex]);

		nKeyIndex++;
		if (nKeyIndex >= PACKET_CRYPTER_KEY_LEN) nKeyIndex = 0;
		pSource++;
	}
	return true;
}

bool CCPacketCrypter::Decrypt(char* pSource, int nSrcLen, CCPacketCrypterKey* pKey)
{
	int nKeyIndex = 0;
	for (int i = 0; i < nSrcLen; i++)
	{
		*pSource = _Dec(*pSource, pKey->szKey[nKeyIndex]);

		nKeyIndex++;
		if (nKeyIndex >= PACKET_CRYPTER_KEY_LEN) nKeyIndex = 0;
		pSource++;
	}

	return true;
}

CCPacketCrypter::CCPacketCrypter()
{
	InitConst();
	memset(&m_Key, 0, sizeof(CCPacketCrypterKey));
}

void CCPacketCrypter::InitConst()
{
	m_nSHL = (MCOMMAND_VERSION % 6) + 1;

	m_ShlMask=0;
	for (int i = 0; i < m_nSHL; i++)
	{
		m_ShlMask += (1 << i);
	}
}