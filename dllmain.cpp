#include <windows.h>
#include "hook.h"

DWORD WINAPI MainThread(LPVOID lpParam)
{
    // 初始化 Hook
    InitPVPForceEnemyHook();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        break;
    }
    return TRUE;
}
