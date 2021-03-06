#include <stdint.h>
#include <windows.h>

// This DLL does nothing except to hold a separate TLS entry (extra implicit slot)
extern "C" unsigned int _tls_index;
thread_local char dll_mt_data[32];

extern "C" __declspec(dllexport) void *GetStaticTlsData()
{
	return &dll_mt_data;
}

extern "C" __declspec(dllexport) size_t GetStaticTlsDataSize()
{
	return sizeof(dll_mt_data);
}

extern "C" __declspec(dllexport) uint32_t GetStaticTlsSlot()
{
	return _tls_index;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}