#include "stdafx.h"

#define BAD_DLL ((HCUSTOMMODULE)0xDEADBEEF)
#define BAD_IMPORT ((FARPROC)0xFEFEDEDE)

HCUSTOMMODULE GetLibrary(LPCSTR Name, void *Userdata);
FARPROC GetLibraryProcAddr(HCUSTOMMODULE Module, LPCSTR Name, void *Userdata);
LPVOID MemoryAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata);

uint32_t g_CreationKitPID;
HANDLE g_NotifyEvent;
HANDLE g_WaitEvent;

//
// Block of data required for the CK.exe base address to be at 0x400000
//
#pragma comment(linker, "/BASE:0x10000")
#pragma bss_seg(push)
#pragma bss_seg(".CKXCODE")
volatile char FORCE_LOAD_REQUIRED_DATA[0x1FC9000];
#pragma bss_seg(pop)

struct CKLipGenTunnel
{
	// Tunnel data is offset by 0x4 for some reason
	// +0x8 = int32 status value 0,1,2,3
	// +0xC = int32 number of floating point values
	// +0x10 = int32 number of expressions (text)
	char unknown[0x20];
	char InputWAVPath[MAX_PATH];
	char ResampleTempWAVPath[MAX_PATH];
	char DialogueText[MAX_PATH];
	char FonixDataPath[MAX_PATH];
	char Language[MAX_PATH];
	char data[0x2F21];					// Floating point/expression data
	bool UnknownStatus;
};
static_assert(offsetof(CKLipGenTunnel, InputWAVPath) == 0x20, "");
static_assert(offsetof(CKLipGenTunnel, ResampleTempWAVPath) == 0x124, "");
static_assert(offsetof(CKLipGenTunnel, DialogueText) == 0x228, "");
static_assert(offsetof(CKLipGenTunnel, FonixDataPath) == 0x32C, "");
static_assert(offsetof(CKLipGenTunnel, Language) == 0x430, "");
static_assert(offsetof(CKLipGenTunnel, UnknownStatus) == 0x3455, "");

void FaceFXLogCallback(const char *Text, int Type)
{
	printf("[FaceFX %02d]: %s\n", Type, Text);
}

void CKLogCallback(int Type, const char *Format, ...)
{
	char buffer[2048];
	va_list va;

	va_start(va, Format);
	int len = _vsnprintf_s(buffer, _TRUNCATE, Format, va);
	va_end(va);

	printf("[CKIT32 %02d]: %s\n", Type, buffer);
}

void InitializeCreationKit()
{
	FORCE_LOAD_REQUIRED_DATA[0] = 0;
	FORCE_LOAD_REQUIRED_DATA[sizeof(FORCE_LOAD_REQUIRED_DATA) - 1] = 0;

	// Read the exe into memory then initialize it
	HRSRC binaryResource	= FindResource(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDR_CK_LIP_BINARY1), L"CK_LIP_BINARY");
	uint32_t size			= SizeofResource(GetModuleHandle(nullptr), binaryResource);
	unsigned char *data		= (unsigned char *)LockResource(LoadResource(GetModuleHandle(nullptr), binaryResource));
	unsigned char *exeData	= new unsigned char[stb_decompress_length(data)];

	if (stb_decompress(exeData, data, size) == 0)
		__debugbreak();

	HMEMORYMODULE creationKitExe = MemoryLoadLibraryEx(exeData, stb_decompress_length(data), MemoryAlloc, MemoryDefaultFree, GetLibrary, GetLibraryProcAddr, MemoryDefaultFreeLibrary, nullptr);
	delete[] exeData;

	if (!creationKitExe)
		__debugbreak();

	// Allocate arbitrary TLS chunk
	int tlsIndex = *(int *)0x1E97DC0;
	*(void **)(__readfsdword(0x2C) + 4 * tlsIndex) = VirtualAlloc(nullptr, 64 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	//
	// Kill MemoryContextTracker ctor/dtor
	//
	PatchMemory(0x948500, (PBYTE)"\xC2\x10\x00", 3);
	PatchMemory(0x948560, (PBYTE)"\xC3", 1);
	PatchMemory(0x9484E9, (PBYTE)"\x5E\xC3", 2);

	//
	// Replace memory allocators with malloc
	//
	PatchMemory(0x4010A0, (PBYTE)"\xC3", 1);

	PatchJump(0x947D20, (uintptr_t)&MemoryManager_Alloc);
	PatchJump(0x947320, (uintptr_t)&MemoryManager_Free);

	PatchJump(0x4014D0, (uintptr_t)&ScrapHeap_Alloc);
	PatchJump(0x931990, (uintptr_t)&ScrapHeap_Free);

	//
	// Patch WinMain in order to only run CRT initialization
	//
	PatchMemory(0x48E8B0, (PBYTE)"\xC2\x10\x00", 3);
	PatchMemory(0xE84A16, (PBYTE)"\xEB\x0E", 2);
	((void(__cdecl *)())(0xE84A7B))();

	CoInitializeEx(nullptr, 0);						// COM components
	((void(*)())(0x934B90))();						// BSResource (filesystem)
	((void(*)())(0x469FE0))();						// LipSynchManager::Init()
	PatchMemory(0x46AA59, (PBYTE)"\x90\x90", 2);	// Required to force update FonixData.cdf path in TLS
	PatchMemory(0xFCA070, (PBYTE)"\x00", 1);		// Prevent saving in the wrong path under 'sound\' (corrected to 'data\sound\')

	((int(*)(void *))(0x8BF320))(&FaceFXLogCallback);
	PatchJump(0x40AFC0, (uintptr_t)&CKLogCallback);
}

DWORD WINAPI ExitNotifyThread(LPVOID Arg)
{
	// Grab handle to parent process (check for termination)
	HANDLE parentProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, g_CreationKitPID);

	if (!parentProcess)
	{
		printf("A CreationKit parent process ID (0x%X) was supplied, but wasn't able to be queried (%d).\n", g_CreationKitPID, GetLastError());
		return 1;
	}

	for (DWORD exitCode;; Sleep(2000))
	{
		if (GetExitCodeProcess(parentProcess, &exitCode) && exitCode == STILL_ACTIVE)
			continue;

		g_CreationKitPID = 0;

		CloseHandle(parentProcess);
		SetEvent(g_NotifyEvent);
		break;
	}

	return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

#if 0
	/*
	CUZJTEUgKmY7DQoNCglpZiAoZm9wZW5fcygmZiwgIkNyZWF0aW9uS2l0LnVucGF
	ja2VkLmV4ZSIsICJyYiIpID09IDApDQoJew0KCQlmc2VlayhmLCAwLCBTRUVLX0
	VORCk7DQoJCWxvbmcgc2l6ZSA9IGZ0ZWxsKGYpOw0KCQlyZXdpbmQoZik7DQoNC
	gkJY2hhciAqZXhlRGF0YSA9IG5ldyBjaGFyW3NpemVdOw0KCQlmcmVhZChleGVE
	YXRhLCBzaXplb2YoY2hhciksIHNpemUsIGYpOw0KCQlzdGJfY29tcHJlc3NfdG9
	maWxlKChjaGFyICopImNvbXByZXNzZWRfcnNyYy5iaW4iLCBleGVEYXRhLCBzaX
	plKTsNCgkJZmNsb3NlKGYpOw0KCQlkZWxldGVbXSBleGVEYXRhOw0KCX0NCg0KC
	XJldHVybiAwOw==
	*/
#endif

	// Bail immediately if this wasn't launched from the CK
	if (!getenv("Ckpid") || strlen(getenv("Ckpid")) <= 0)
		return 1;

	g_CreationKitPID = atoi(getenv("Ckpid"));

	// Establish tunnel
	char temp[128];
	sprintf_s(temp, "CkSharedMem%d", g_CreationKitPID);

	HANDLE mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, TRUE, temp);

	if (!mapping)
	{
		printf("Could not create file mapping object (%d).\n", GetLastError());
		return 1;
	}

	auto *tunnel = (CKLipGenTunnel *)MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0x40000);

	if (!tunnel)
	{
		printf("Could not map view of file (%d).\n", GetLastError());
		CloseHandle(mapping);
		return 1;
	}

	sprintf_s(temp, "CkNotifyEvent%d", g_CreationKitPID);
	g_NotifyEvent = OpenEventA(EVENT_ALL_ACCESS, TRUE, temp);

	sprintf_s(temp, "CkWaitEvent%d", g_CreationKitPID);
	g_WaitEvent = OpenEventA(EVENT_ALL_ACCESS, TRUE, temp);

	if (!g_NotifyEvent || !g_WaitEvent)
	{
		printf("Could not open event handle(s) (%d).\n", GetLastError());
		UnmapViewOfFile(tunnel);
		CloseHandle(mapping);
		return 1;
	}

	CloseHandle(CreateThread(nullptr, 0, ExitNotifyThread, nullptr, 0, nullptr));

	//
	// Wait until the creation kit asks to do something
	//
	char currentDirectory[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, currentDirectory);

	for (bool ckCodeLoaded = false;;)
	{
		DWORD waitStatus = WaitForSingleObject(g_NotifyEvent, 5000);

		if (waitStatus == WAIT_FAILED)
		{
			printf("Failed waiting for CKIT64 notification (%d).\n", GetLastError());
			break;
		}

		// If the parent process exited, bail
		if ((waitStatus == WAIT_OBJECT_0 && strlen(tunnel->InputWAVPath) <= 0) || g_CreationKitPID == 0)
			break;

		if (waitStatus == WAIT_TIMEOUT)
			continue;

		if (!ckCodeLoaded)
		{
			InitializeCreationKit();
			ckCodeLoaded = true;
		}

		uintptr_t fonixPathPtr = (uintptr_t)&tunnel->FonixDataPath;
		PatchMemory(0x469CA8, (PBYTE)&fonixPathPtr, sizeof(uintptr_t));// Manually patch Fonix.cdf lookup path

		uintptr_t languagePtr = (uintptr_t)&tunnel->Language;
		PatchMemory(0x11B0AEC, (PBYTE)&languagePtr, sizeof(uintptr_t));// Manually patch language

		printf("Attempting to create LIP file:\n");
		printf("     Language: '%s'\n", tunnel->Language);
		printf("     Input data: '%s'\n", tunnel->FonixDataPath);
		printf("     Input WAV: '%s'\n", tunnel->InputWAVPath);
		printf("     Resampled input WAV: '%s'\n", tunnel->ResampleTempWAVPath);
		printf("     Text: '%s'\n", tunnel->DialogueText);

		// Generate the lip file, then tell the CK
		auto generateLipFile = [](const char *WAVPath, const char *ResamplePath, const char *BaseDirectory, const char *DialogueText, int *FaceFxObject)
		{
			return ((void *(__cdecl *)(const char *, const char *, const char *, const char *, int *))(0x46ACD0))
				(WAVPath, ResamplePath, BaseDirectory, DialogueText, FaceFxObject);
		};

		void *lipAsset = generateLipFile(tunnel->InputWAVPath, tunnel->ResampleTempWAVPath, currentDirectory, tunnel->DialogueText, nullptr/*dword_12B64B8*/);

		// Write it to disk as a temp copy & free memory
		if (lipAsset)
		{
			bool created = ((bool(__thiscall *)(void *, const char *, int, int, int))(0x587C70))(lipAsset, "Data\\Sound\\Voice\\Processing\\Temp.lip", 16, 1, 1);

			printf("Writing temporary LIP file to 'Data\\Sound\\Voice\\Processing\\Temp.lip'\n");

			if (!created)
				printf("LIP data was generated but the file couldn't be saved!\n");

			((void(__thiscall *)(void *))(0x586A40))(lipAsset);
			MemoryManager_Free(nullptr, nullptr, lipAsset, false);
		}

		memset(tunnel->InputWAVPath, 0, sizeof(tunnel->InputWAVPath));
		tunnel->UnknownStatus = true;

		// Done
		SetEvent(g_WaitEvent);
		WaitForSingleObject(g_NotifyEvent, INFINITE);
		SetEvent(g_WaitEvent);
	}

	CloseHandle(g_NotifyEvent);
	CloseHandle(g_WaitEvent);
	UnmapViewOfFile(tunnel);
	CloseHandle(mapping);

	printf("LIPGen tool exiting\n");
	return 0;
}

HCUSTOMMODULE GetLibrary(LPCSTR Name, void *Userdata)
{
	constexpr static const char *moduleBlacklist[] =
	{
		"SSCE5432.DLL",
		"D3D9.DLL",
		"DSOUND.DLL",
		"STEAM_API.DLL",
		"X3DAUDIO1_7.DLL",
		"DBGHELP.DLL",
		"D3DX9_42.DLL",
		"XINPUT1_3.DLL",
	};

	// Check for blacklisted DLLs first
	for (int i = 0; i < ARRAYSIZE(moduleBlacklist); i++)
	{
		if (!_stricmp(Name, moduleBlacklist[i]))
			return BAD_DLL;
	}

	HCUSTOMMODULE importDll = (HCUSTOMMODULE)GetModuleHandleA(Name);

	if (!importDll)
		importDll = (HCUSTOMMODULE)LoadLibraryA(Name);

	return importDll;
}

FARPROC GetLibraryProcAddr(HCUSTOMMODULE Module, LPCSTR Name, void *Userdata)
{
	if (Module == BAD_DLL)
		return BAD_IMPORT;

	return MemoryDefaultGetProcAddress(Module, Name, Userdata);
}

LPVOID MemoryAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata)
{
	uintptr_t addr = (uintptr_t)address;

	if (addr >= 0x400000 && (addr + size) <= 0x1FC9000)
	{
		// Sanity check
		if (addr < (uintptr_t)&FORCE_LOAD_REQUIRED_DATA[0] ||
			(addr + size) > (uintptr_t)&FORCE_LOAD_REQUIRED_DATA[sizeof(FORCE_LOAD_REQUIRED_DATA)])
			__debugbreak();

		DWORD old;
		VirtualProtect(address, size, protect, &old);

		return address;
	}

	return VirtualAlloc(address, size, allocationType, protect);
}