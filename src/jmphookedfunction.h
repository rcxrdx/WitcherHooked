// jmphookedfunction by paulsapps@github

#pragma once

#include "memoryprotection.h"
#include <windows.h>
#include <type_traits>

template<class T>
class JmpHookedFunction
{
public:
    JmpHookedFunction(void* aOldFunc, void* aNewFunc)
        : iOldFunc( (T)aOldFunc ), iNewFunc( (T)aNewFunc ), iHooked( false )
	{
		// Copy its intro
		CopyOrignalFunctionBytes();

		// Hook it
		JmpHookedFunction::Hook();
	}

	~JmpHookedFunction()
	{
		UnHook();
	}

    T OrignalFunction()
	{
		return iOldFunc;
	}

    void UnHook()
	{
		{
            MemoryProtection memProt( (void*)iOldFunc, 0x100 );

			if ( iNewFunc && iHooked == true )
			{
				for ( int i=0; i<sizeof(iOldFuncBytes); ++i )
				{
					*((unsigned char *) ((uint32_t)iOldFunc+i)) = iOldFuncBytes[i];
				}
				iHooked = false;
			}
		}
		FlushInstructionCache( GetCurrentProcess(), (void*)iOldFunc, 6 );
	}

    void Hook()
	{
		{
            MemoryProtection memProt( (void*)iOldFunc, 0x100 );
			if ( iNewFunc && iHooked == false )
			{			
				// JMP
				*((unsigned char *) ((uint32_t)iOldFunc+0)) = 0xE9;

				// New func addr
				uint32_t addr = (uint32_t)iNewFunc;
                addr = addr - 5;

				uint32_t tmp = 0;
                if (uint32_t(iOldFunc) > addr)
                {
                    tmp = ((uint32_t)iOldFunc) - addr;
                }
                else
                {
                    tmp = addr - ((uint32_t)iOldFunc);
                }

                *((uint32_t*)((uint32_t)iOldFunc + 1)) = (uint32_t)tmp;

				iHooked = true;
			}
		}
		FlushInstructionCache( GetCurrentProcess(), (void*)iOldFunc, 6 );
	}


private:

	void CopyOrignalFunctionBytes()
	{
		for ( int i=0; i<sizeof(iOldFuncBytes)/sizeof(iOldFuncBytes[0]); ++i )
		{
			iOldFuncBytes[i] = *((unsigned char *) ((uint32_t)iOldFunc+i));
		}
	}

private:
	T iOldFunc;
	T iNewFunc;
	
	bool iHooked;

	// JMP + Target
	unsigned char iOldFuncBytes[5];
};

template < class T >
class CallOriginalJmpHookedFunction
{
public:
	CallOriginalJmpHookedFunction( JmpHookedFunction<T>& aFunc )
		: iFunc( aFunc )
	{
		iFunc.UnHook();
	}

	~CallOriginalJmpHookedFunction()
	{
		iFunc.Hook();
	}

private:
	 JmpHookedFunction<T>& iFunc;
};

