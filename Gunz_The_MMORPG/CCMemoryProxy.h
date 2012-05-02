#pragma once

#include "CCMath.h"
#include "CCCRC32.h"

#define MEMBER_SET_CHECKCRC(PROTECTVALUE, MEMBER, NEWVAL) { \
		PROTECTVALUE.CheckCrc(); \
		PROTECTVALUE.Ref().MEMBER = (NEWVAL); \
		PROTECTVALUE.MakeCrc(); \
	}


class CCCrc32Container
{
	std::map<void*, DWORD> m_mapCrc32;
	typedef std::map<void*, DWORD>::iterator Iterator;

public:
	void Add(void* key, DWORD crc)
	{
		m_mapCrc32[key] = crc;
	}
	void Remove(void* key)
	{
		m_mapCrc32.erase(key);
	}
	DWORD Get(void* key)
	{
		Iterator it = m_mapCrc32.find(key);
		if (it!=m_mapCrc32.end())
			return it->second;
		return 0;
	}
};

extern void (*g_fpOnCrcFail)();	//crc check failure (hacking) to invoke a function pointer

CCCrc32Container* GetCrcContainer();


#define PTR_OFFSET 0x66D6
//Note to users Korea nuclear Search is usually the range 0x15000000 ~ 0x30000000 enough heap
//(RandMaskVal.exe by changing the value before the build. Define people by modifying the parse error is displayed.)

template <typename T>
class CCProtectValue
{
	//Assign a value to the address pointer to put the value of address mask to hide itself
	typedef unsigned char* MaskedPtr;
	MaskedPtr m_pValue;

private:
	MaskedPtr	ToMaskedPtr(T* ptr) const			{ return MaskedPtr(ptr) + (PtrToUlong(this)+PTR_OFFSET); }
	T*			ToNormalPtr(MaskedPtr mptr) const	{ return (T*)(mptr - (PtrToUlong(this)+PTR_OFFSET)); }

	DWORD BuildCrc()
	{
		BYTE* pData = (BYTE*)ToNormalPtr(m_pValue);
		return CCCRC32::BuildCRC32(pData, sizeof(T));
	}

public:
	CCProtectValue()		 { m_pValue = ToMaskedPtr(new T); }

	~CCProtectValue()	 { 
		delete ToNormalPtr(m_pValue);
		::GetCrcContainer()->Remove(this);
	}

	//Get the value as a reference
			T& Ref()		{ return *ToNormalPtr(m_pValue); }
	const	T& Ref() const	{ return *ToNormalPtr(m_pValue); }

	//Value assigned
	void Set(const T& newVal)		{ Ref() = newVal; }

	//Value as a double check crc
	bool Set_CheckCrc(const T& newVal) {
		CheckCrc();
		Set(newVal);
		MakeCrc();
		return true;
	}
	//Crc value generated as a double (the initial value)
	void Set_MakeCrc(const T& newVal) {
		Set(newVal);
		MakeCrc();
	}


	//Reassign -> Copy change through
	void ShiftHeapPos()
	{
		T* p = ToNormalPtr(m_pValue);
		m_pValue = ToMaskedPtr(new T);
		Ref() = *p;
		delete p;
	}

	void ShiftHeapPos_CheckCrc() { CheckCrc(); ShiftHeapPos(); }

	void MakeCrc()
	{
		DWORD crc = BuildCrc();
		::GetCrcContainer()->Add(this, crc);
	}

	//Crc checks
	void CheckCrc()
	{
		if (BuildCrc() != ::GetCrcContainer()->Get(this))
			g_fpOnCrcFail();
	}

private:
	//Operator overloading would be convenient to write, but what happens actually forced to explicitly represent better for thinking that.

	CCProtectValue(T val) { m_pValue = ToMaskedPtr(new T(val)); }	//copy constructor.Rather confusing, let's hide him.
	CCProtectValue<T>& operator=(int) {}	//copy assignment operator, behind
};

//USAGE: the value that you want to prevent hacking into the template wraps. 
//This template as a means of two nuclei can interfere with memory.
//Periodically freeze hipwichireul moving variables to specific values ??that can be makahbol.
//In the template, however, able to track actual address, so you can check crc32 secondary.
//Crc32 value to use should be checked before updating gijongap crc and crc value of a new building must be put behind.
//This template class only when necessary, even if wrapped in a nuclear defense capabilities can be operated. For example, wrap it, even if members of ZObject
//Direct nuclear defense capabilities if you do not call a function that does not effect the nuclear defense. If the NPC is ZObject ZActor ZMyCharacter and receive an inheritance
//NPC bother hacking the target nucleus is not to accept the load without the need to conform to defend ten thousand nuclear ZMyCharacter eoyong room is the function call.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Or less template was used as a defense in the past that the nucleus
//CCMemoryBlock the known.
//CCMemoryFugitive good move for the selective effect, but in each case, and checksum is difficult to parallel

/*
#define WRAPING_TIME 1000	//memory block that turns once a second time ... ...
#define NUM_OF_MEMORY 20	//Number of memory, caught in a block of memory.

template <typename _data>
struct CCMemoryBlock
{
	_data * m_pMemory[NUM_OF_MEMORY];
};

template <typename _data>
class CCMemoryProxy
{
	CCMemoryBlock<_data> * MB1;
	CCMemoryBlock<_data> * MB2;
	CCMemoryBlock<_data> * MB3;

	CCMemoryBlock<_data> * MB;

	_data m_initial;

	DWORD m_dwCurrentTime;
	int m_iCorrectDataIndex;
	int m_iIndex;
	
public:
	CCMemoryProxy()
	{
		Init();
	}

	CCMemoryProxy(_data init)
	{
		m_initial = init;
		m_iIndex = 0;
		m_iCorrectDataIndex = 0;
		Init();
	}

	~CCMemoryProxy()
	{
		Destroy();
	}

	void Init()
	{
		MB1 = new CCMemoryBlock<_data>();
		MB2 = new CCMemoryBlock<_data>();
		MB3 = new CCMemoryBlock<_data>();
		
		for(int i=0; i<NUM_OF_MEMORY; i++)
		{
			MB1->m_pMemory[i] = new _data;
			*(MB1->m_pMemory[i]) = m_initial;

			MB2->m_pMemory[i] = new _data;
			*(MB2->m_pMemory[i]) = m_initial;

			MB3->m_pMemory[i] = new _data;
			*(MB3->m_pMemory[i]) = m_initial;
		}
		
		m_iIndex = 1;
		MB = MB1;
		m_dwCurrentTime = GetTickCount();

		SetRandomData( m_initial);
	}
	void Destroy()
	{
		for(int i=0; i<NUM_OF_MEMORY; i++)
		{
			delete MB1->m_pMemory[i];
			delete MB2->m_pMemory[i];
			delete MB3->m_pMemory[i];
		}

		delete MB1;
		delete MB2;
		delete MB3;
	}

	void SetData(const _data & data)
	{
		SetRandomData(data);
	}

	_data & GetData()
	{
		return * MB->m_pMemory[m_iCorrectDataIndex];
	}

	void SetRandomData(const _data & data)
	{
		m_iCorrectDataIndex = RandomNumber(0, NUM_OF_MEMORY-1);
		*(MB->m_pMemory[m_iCorrectDataIndex]) = data;
	}
	void SetWarpingAdd(DWORD tick)
	{
		if( tick - m_dwCurrentTime > WRAPING_TIME)
		{
			int i = RandomNumber(0, NUM_OF_MEMORY-1);
			if(m_iIndex == 1)
			{
				*(MB2->m_pMemory[i]) = *(MB1->m_pMemory[m_iCorrectDataIndex]);
				MB = MB2;
				*(MB1->m_pMemory[m_iCorrectDataIndex]) = m_initial;	//trail less than a unit
				m_iCorrectDataIndex = i;
			}
			else if(m_iIndex == 2)
			{
				*(MB3->m_pMemory[i]) = *(MB2->m_pMemory[m_iCorrectDataIndex]);
				MB = MB3;
				*(MB2->m_pMemory[m_iCorrectDataIndex]) = m_initial;
				m_iCorrectDataIndex = i;
			}
			else
			{
				*(MB1->m_pMemory[i]) = *(MB3->m_pMemory[m_iCorrectDataIndex]);
				MB = MB1;
				*(MB3->m_pMemory[m_iCorrectDataIndex]) = m_initial;
				m_iCorrectDataIndex = i;
			}
			m_iIndex++;
			if(m_iIndex>3)
				m_iIndex = 1;

			m_dwCurrentTime = tick;
		}	
	}
};




template <typename _data>
class CCMemoryFugitive
{
	_data* m_value;

public:
	CCMemoryFugitive()
	{
		m_value = NULL;
	}

	CCMemoryFugitive(_data data)
	{
		m_value = new _data;
		*m_value = data;
	}

	~CCMemoryFugitive()
	{
		Destroy();
	}

	void Destroy()
	{
		delete m_value;
	}

	CCMemoryFugitive& operator=(const _data& other)	
	{
		SetData(other);
		return *this;
	}

	CCMemoryFugitive& operator+=(const _data &other)
	{
		_data sum = *m_value + other;
		SetData(sum);
		return *this;
	}
	CCMemoryFugitive& operator-=(const _data &other)
	{
		_data sum = *m_value - other;
		SetData(sum);
		return *this;
	}
	CCMemoryFugitive& operator*=(const _data &other)
	{
		_data sum = *m_value * other;
		SetData(sum);
		return *this;
	}

	void SetData(const _data & data)
	{
		_data* pTemp = NULL;

		if (m_value != NULL)
		{
			pTemp = m_value;
		}

		m_value = new _data;
		*m_value = data;

		if (pTemp != NULL)
		{
			delete pTemp;
		}
	}

	_data & GetData()
	{
		return *m_value ;
	}

	const _data & GetData() const
	{
		return *m_value ;
	}


	void ShiftManually()
	{
		if (m_value != NULL)
		{
			_data val_copy = GetData();
			SetData(val_copy);
		}
	}
};

*/