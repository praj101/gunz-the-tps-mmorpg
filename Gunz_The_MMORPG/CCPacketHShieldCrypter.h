#pragma once
#include "../HShield/HsCryptLib.h"
#include "CCPacket.h"

class CCPacketHShieldCrypter
{
public:
	CCPacketHShieldCrypter(void);
	~CCPacketHShieldCrypter(void);

	static DWORD Init();
	static DWORD Encrypt(PBYTE pbyInput, UINT nInLength);
	static DWORD Decrypt(PBYTE pbyInput, UINT nInLength);
	static DWORD Decrypt(PBYTE pbyInput, UINT nInLength, PBYTE pbyOutput);

private:
	static HSCRYPT_KEYINFO m_HsKeyInfo;
	static unsigned char m_OutputBuf[MAX_PACKET_SIZE];
};
