// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"

#include "include/mem_mgr.h"


#ifdef _WIN32

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
        
        break;
	case DLL_PROCESS_DETACH:
        auto str = mem_mgr::instance()->stats();
        OutputDebugStringA(str.c_str());
		break;
	}
	return TRUE;
}


#endif