#pragma warning (disable : 4018 )
#include "stdafx.h"
#include "CCCommandParameter.h"
#include <string.h>

bool CCCommandParamConditionMinMax::Check(CCCommandParameter* pCP)
{
	switch (pCP->GetType())
	{
	case MPT_INT:
		{
			int nValue;
			pCP->GetValue(&nValue);
			if ((nValue < m_nMin) || (nValue > m_nMax)) return false;
		}
		break;
	case MPT_UINT:
		{
			unsigned int nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (unsigned int)m_nMin) || (nValue > (unsigned int)m_nMax)) return false;
		}
		break;
	case MPT_CHAR:
		{
			char nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (char)m_nMin) || (nValue > (char)m_nMax)) return false;
		}
		break;
	case MPT_UCHAR:
		{
			unsigned char nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (unsigned char)m_nMin) || (nValue > (unsigned char)m_nMax)) return false;
		}
		break;
	case MPT_SHORT:
		{
			short nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (short)m_nMin) || (nValue > (short)m_nMax)) return false;
		}
		break;
	case MPT_USHORT:
		{
			unsigned short nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (unsigned short)m_nMin) || (nValue > (unsigned short)m_nMax)) return false;
		}
		break;
	default:
		{
			_ASSERT(0); // 없는 제약조건이다.
		}
	};

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
CCCommandParameterDesc::CCCommandParameterDesc(CCCommandParameterType nType, char* szDescription)
{
	m_nType = nType;
	strcpy(m_szDescription, szDescription);
}

void CCCommandParameterDesc::InitializeConditions()
{
	for(int i=0; i<(int)m_Conditions.size(); i++){
		delete m_Conditions[i];
	}
	m_Conditions.clear();
}

CCCommandParameterDesc::~CCCommandParameterDesc()
{
	InitializeConditions();
}

void CCCommandParameterDesc::AddCondition(CCCommandParamCondition* pCondition)
{
	m_Conditions.push_back(pCondition);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
CCCommandParameterInt::CCCommandParameterInt()
 : CCCommandParameter(MPT_INT)
{
	m_Value = 0;
}
CCCommandParameterInt::CCCommandParameterInt(int Value)
 : CCCommandParameter(MPT_INT)
{
	m_Value = Value;
}
CCCommandParameter* CCCommandParameterInt::Clone()
{
	return (new CCCommandParameterInt(m_Value));
}
void CCCommandParameterInt::GetValue(void* p)
{
	*(int*)p = m_Value;
}
int CCCommandParameterInt::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterInt::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

CCCommandParameterUInt::CCCommandParameterUInt()
: CCCommandParameter(MPT_UINT)
{
	m_Value = 0;
}
CCCommandParameterUInt::CCCommandParameterUInt(unsigned int Value)
: CCCommandParameter(MPT_UINT)
{
	m_Value = Value;
}
CCCommandParameter* CCCommandParameterUInt::Clone()
{
	return (new CCCommandParameterUInt(m_Value));
}
void CCCommandParameterUInt::GetValue(void* p)
{
	*(unsigned int*)p = m_Value;
}
int CCCommandParameterUInt::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterUInt::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}


CCCommandParameterFloat::CCCommandParameterFloat()
 : CCCommandParameter(MPT_FLOAT)
{
	m_Value = 0;
}
CCCommandParameterFloat::CCCommandParameterFloat(float Value)
 : CCCommandParameter(MPT_FLOAT)
{
	m_Value = Value;
}
CCCommandParameter* CCCommandParameterFloat::Clone()
{
	return (new CCCommandParameterFloat(m_Value));
}
void CCCommandParameterFloat::GetValue(void* p)
{
	*(float*)p = m_Value;
}
int CCCommandParameterFloat::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterFloat::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}


CCCommandParameterString::CCCommandParameterString()
 : CCCommandParameter(MPT_STR)
{
	m_Value = 0;
}
CCCommandParameterString::CCCommandParameterString(const char* Value)
 : CCCommandParameter(MPT_STR)
{
	int nLen = (int)strlen(Value)+2;

	if (nLen > (USHRT_MAX-2))
	{
		m_Value = 0;
		return;
	}

	m_Value = new char[strlen(Value)+2];
	strcpy(m_Value, Value);
}
CCCommandParameterString::~CCCommandParameterString()
{
	if(m_Value!=NULL){
		delete m_Value;
		m_Value=NULL;
	}
}
CCCommandParameter* CCCommandParameterString::Clone()
{
	return (new CCCommandParameterString(m_Value));
}
void CCCommandParameterString::GetValue(void* p)
{
	strcpy((char*)p, m_Value);
}
int CCCommandParameterString::GetData(char* pData, int nSize)
{
	if(m_Value==NULL) 
	{
		unsigned short nEmptySize = 0;
		memcpy( pData, &nEmptySize, sizeof(nEmptySize) );
		return sizeof(nEmptySize);
	}

	unsigned short nValueSize = (unsigned short)strlen(m_Value)+2;
	if((int)nValueSize+(int)sizeof(nValueSize)>nSize) return 0;

	memcpy(pData, &nValueSize, sizeof(nValueSize));
	memcpy(pData+sizeof(nValueSize), m_Value, nValueSize);

	return nValueSize+sizeof(nValueSize);
}
int CCCommandParameterString::SetData(char* pData)
{
	if(m_Value!=NULL) 
	{
		delete[] m_Value;
		m_Value = 0;
	}

	unsigned short nValueSize = 0;
	memcpy(&nValueSize, pData, sizeof(nValueSize));

	m_Value = new char[nValueSize];

	memcpy(m_Value, pData+sizeof(nValueSize), nValueSize);
	return nValueSize+sizeof(nValueSize);
}

int CCCommandParameterString::GetSize()
{
	if(m_Value==NULL) return 0;
	return ((int)strlen(m_Value)+2 + sizeof(unsigned short));
}

CCCommandParameterVector::CCCommandParameterVector()
 : CCCommandParameter(MPT_VECTOR)
{
	m_fX = m_fY = m_fZ = 0;
}
CCCommandParameterVector::CCCommandParameterVector(float x ,float y, float z)
 : CCCommandParameter(MPT_VECTOR)
{
	m_fX = x;
	m_fY = y;
	m_fZ = z;
}
CCCommandParameterVector::~CCCommandParameterVector()
{
}
CCCommandParameter* CCCommandParameterVector::Clone()
{
	return (new CCCommandParameterVector(m_fX, m_fY, m_fZ));
}
void CCCommandParameterVector::GetValue(void* p)
{
	((float*)p)[0] = m_fX;
	((float*)p)[1] = m_fY;
	((float*)p)[2] = m_fZ;
}
int CCCommandParameterVector::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_fX) * 3;
	if(nValueSize>nSize) return 0;
	float v[3] = {m_fX, m_fY, m_fZ};
	memcpy(pData, v, nValueSize);
	return nValueSize;
}
int CCCommandParameterVector::SetData(char* pData)
{
	int nValueSize = sizeof(m_fX) * 3;
	float v[3];
	memcpy(v, pData, nValueSize);
	m_fX = v[0];
	m_fY = v[1];
	m_fZ = v[2];
	return nValueSize;
}

CCCommandParameter* CCCommandParameterBool::Clone()
{
	return (new CCCommandParameterBool(m_Value));
}
void CCCommandParameterBool::GetValue(void* p)
{
	*(bool*)p = m_Value;
}
int CCCommandParameterBool::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterBool::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}
void *CCCommandParameterBool::GetPointer()
{
	return &m_Value;
}

CCCommandParameterUID::CCCommandParameterUID()
 : CCCommandParameter(MPT_UID)
{
}
CCCommandParameterUID::CCCommandParameterUID(const CCUID& uid)
 : CCCommandParameter(MPT_UID)
{
	m_Value = uid;
}
CCCommandParameterUID::~CCCommandParameterUID()
{
}
CCCommandParameterUID* CCCommandParameterUID::Clone()
{
	return (new CCCommandParameterUID(m_Value));
}
void CCCommandParameterUID::GetValue(void* p)
{
	*(CCUID*)p = m_Value;
}
int CCCommandParameterUID::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterUID::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

CCCommandParameterBlob::CCCommandParameterBlob()
: CCCommandParameter(MPT_BLOB)
{
	m_Value = 0;
	m_nSize = 0;
}
CCCommandParameterBlob::CCCommandParameterBlob(const void* Value, int nSize)
: CCCommandParameter(MPT_BLOB)
{
	if (nSize >= MAX_BLOB_SIZE || nSize<= 0)
	{
		m_Value = NULL;
		m_nSize = 0;
		return;
	}

	m_Value = new unsigned char[nSize];
	memcpy(m_Value, Value, nSize);
	m_nSize = nSize;
}
CCCommandParameterBlob::~CCCommandParameterBlob()
{
	if(m_Value!=NULL){
		delete[] m_Value;
		m_Value = NULL;
	}
}

CCCommandParameterBlob* CCCommandParameterBlob::Clone()
{
	return new CCCommandParameterBlob(m_Value, m_nSize);
}
void CCCommandParameterBlob::GetValue(void* p)
{
	memcpy(p, m_Value, m_nSize);
}
int CCCommandParameterBlob::GetData(char* pData, int nSize)
{
	if(m_Value==NULL) return 0;
	if(m_nSize+(int)sizeof(m_nSize)>nSize) return 0;

	memcpy(pData, &m_nSize, sizeof(m_nSize));
	memcpy(pData+sizeof(m_nSize), m_Value, m_nSize);

	return m_nSize+sizeof(m_nSize);
}
int CCCommandParameterBlob::SetData(char* pData)
{
	if(m_Value!=NULL) delete[] m_Value;

	memcpy(&m_nSize, pData, sizeof(m_nSize));

	m_Value = new char[m_nSize];

	memcpy(m_Value, pData+sizeof(m_nSize), m_nSize);
	return m_nSize+sizeof(m_nSize);
}

int CCCommandParameterBlob::GetSize()
{
	return (m_nSize+sizeof(m_nSize));
}

///////////////////////////////////////////////////////////////////////////////
CCCommandParameterChar::CCCommandParameterChar()
 : CCCommandParameter(MPT_CHAR)
{
	m_Value = 0;
}
CCCommandParameterChar::CCCommandParameterChar(char Value)
 : CCCommandParameter(MPT_CHAR)
{
	m_Value = Value;
}

CCCommandParameter* CCCommandParameterChar::Clone()
{
	return (new CCCommandParameterChar(m_Value));
}

void CCCommandParameterChar::GetValue(void* p)
{
	*(char*)p = m_Value;
}
int CCCommandParameterChar::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterChar::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

///////////////////////////////////////////////////////////////////////////////
CCCommandParameterUChar::CCCommandParameterUChar()
 : CCCommandParameter(MPT_UCHAR)
{
	m_Value = 0;
}
CCCommandParameterUChar::CCCommandParameterUChar(unsigned char Value)
 : CCCommandParameter(MPT_UCHAR)
{
	m_Value = Value;
}

CCCommandParameter* CCCommandParameterUChar::Clone()
{
	return (new CCCommandParameterUChar(m_Value));
}

void CCCommandParameterUChar::GetValue(void* p)
{
	*(unsigned char*)p = m_Value;
}
int CCCommandParameterUChar::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterUChar::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

///////////////////////////////////////////////////////////////////////////////
CCCommandParameterShort::CCCommandParameterShort()
 : CCCommandParameter(MPT_SHORT)
{
	m_Value = 0;
}
CCCommandParameterShort::CCCommandParameterShort(short Value)
 : CCCommandParameter(MPT_SHORT)
{
	m_Value = Value;
}

CCCommandParameter* CCCommandParameterShort::Clone()
{
	return (new CCCommandParameterShort(m_Value));
}

void CCCommandParameterShort::GetValue(void* p)
{
	*(short*)p = m_Value;
}
int CCCommandParameterShort::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterShort::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

///////////////////////////////////////////////////////////////////////////////
CCCommandParameterUShort::CCCommandParameterUShort()
 : CCCommandParameter(MPT_USHORT)
{
	m_Value = 0;
}
CCCommandParameterUShort::CCCommandParameterUShort(unsigned short Value)
 : CCCommandParameter(MPT_USHORT)
{
	m_Value = Value;
}

CCCommandParameter* CCCommandParameterUShort::Clone()
{
	return (new CCCommandParameterUShort(m_Value));
}

void CCCommandParameterUShort::GetValue(void* p)
{
	*(short*)p = m_Value;
}
int CCCommandParameterUShort::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterUShort::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

///////////////////////////////////////////////////////////////////////////////
CCCommandParameterInt64::CCCommandParameterInt64()
 : CCCommandParameter(MPT_INT64)
{
	m_Value = 0;
}
CCCommandParameterInt64::CCCommandParameterInt64(int64 Value)
 : CCCommandParameter(MPT_INT64)
{
	m_Value = Value;
}

CCCommandParameter* CCCommandParameterInt64::Clone()
{
	return (new CCCommandParameterInt64(m_Value));
}

void CCCommandParameterInt64::GetValue(void* p)
{
	*(int64*)p = m_Value;
}
int CCCommandParameterInt64::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterInt64::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

///////////////////////////////////////////////////////////////////////////////
CCCommandParameterUInt64::CCCommandParameterUInt64()
 : CCCommandParameter(MPT_UINT64)
{
	m_Value = 0;
}
CCCommandParameterUInt64::CCCommandParameterUInt64(uint64 Value)
 : CCCommandParameter(MPT_UINT64)
{
	m_Value = Value;
}

CCCommandParameter* CCCommandParameterUInt64::Clone()
{
	return (new CCCommandParameterUInt64(m_Value));
}

void CCCommandParameterUInt64::GetValue(void* p)
{
	*(uint64*)p = m_Value;
}
int CCCommandParameterUInt64::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_Value);
	if(nValueSize>nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int CCCommandParameterUInt64::SetData(char* pData)
{
	int nValueSize = sizeof(m_Value);
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}



/////////////////////////////////////////////////////////////////////////////////
CCCommandParameterShortVector::CCCommandParameterShortVector()
 : CCCommandParameter(MPT_SVECTOR)
{
	m_nX = m_nY = m_nZ = 0;
}
CCCommandParameterShortVector::CCCommandParameterShortVector(short x ,short y, short z)
 : CCCommandParameter(MPT_SVECTOR)
{
	m_nX = x;
	m_nY = y;
	m_nZ = z;
}

CCCommandParameterShortVector::CCCommandParameterShortVector(float x ,float y, float z)
: CCCommandParameter(MPT_SVECTOR)
{
	m_nX = (short)floorf(x + 0.5f);
	m_nY = (short)floorf(y + 0.5f);
	m_nZ = (short)floorf(z + 0.5f);
}

CCCommandParameterShortVector::~CCCommandParameterShortVector()
{
}
CCCommandParameter* CCCommandParameterShortVector::Clone()
{
	return (new CCCommandParameterShortVector(m_nX, m_nY, m_nZ));
}
void CCCommandParameterShortVector::GetValue(void* p)
{
	((short*)p)[0] = m_nX;
	((short*)p)[1] = m_nY;
	((short*)p)[2] = m_nZ;
}
int CCCommandParameterShortVector::GetData(char* pData, int nSize)
{
	int nValueSize = sizeof(m_nX) * 3;
	if(nValueSize>nSize) return 0;
	short v[3] = {m_nX, m_nY, m_nZ};
	memcpy(pData, v, nValueSize);
	return nValueSize;
}
int CCCommandParameterShortVector::SetData(char* pData)
{
	int nValueSize = sizeof(m_nX) * 3;
	short v[3];
	memcpy(v, pData, nValueSize);
	m_nX = v[0];
	m_nY = v[1];
	m_nZ = v[2];
	return nValueSize;
}
