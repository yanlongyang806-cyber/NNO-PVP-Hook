#include <windows.h>
#include <cstdio>

// ===== 强制全野外 PVP 的 Hook =====
// 适配 Neverwinter 真端 GameServer.exe (自动从 scanner.exe 找到 PVP 偏移)

typedef int(__cdecl* PVP_RELATION_FUNC)(DWORD a1, DWORD a2);
PVP_RELATION_FUNC OriginalPvpRelation = nullptr;

// ★★★ 全部玩家都强制为敌人 ★★★
int __cdecl HookPvpRelation(DWORD a1, DWORD a2)
{
    return 2;  // 0=友善 1=队伍 2=敌对
}

// DLL 入口：自动注入 Hook
DWORD WINAPI InitHook(LPVOID)
{
    FILE* fp = fopen("pvp_offset.txt", "r");
    if (!fp)
    {
        MessageBoxA(0, "缺少 pvp_offset.txt（请先运行 scanner.exe）", "NNO-PVP-Hook", 0);
        return 0;
    }

    unsigned int offset = 0;
    fscanf(fp, "%x", &offset);
    fclose(fp);

    DWORD base = (DWORD)GetModuleHandleA("GameServer.exe");
    OriginalPvpRelation = (PVP_RELATION_FUNC)(base + offset);

    DWORD oldProtect;
    VirtualProtect(OriginalPvpRelation, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    // 写入跳转到 Hook
    *(BYTE*)OriginalPvpRelation = 0xE9;
    *(DWORD*)((DWORD)OriginalPvpRelation + 1) = (DWORD)HookPvpRelation - (DWORD)OriginalPvpRelation - 5;

    VirtualProtect(OriginalPvpRelation, 5, oldProtect, &oldProtect);

    MessageBoxA(0, "全野外 PVP Hook 已启用！", "NNO-PVP-Hook", 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, InitHook, NULL, 0, NULL);
    }
    return TRUE;
}
