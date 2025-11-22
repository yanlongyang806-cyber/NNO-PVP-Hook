#include "hook.h"
#include <windows.h>

// 你扫描出的地址（自动注入 GameServer.exe）
DWORD PVP_RELATION_OFFSET = 0x00093A70;

// 游戏原始函数的 typedef
typedef int(__cdecl* fnGetRelation)(DWORD* a1, DWORD* a2);

// 保存原函数指针
fnGetRelation oGetRelation = nullptr;

// 强制所有玩家互为敌人
int __cdecl hkGetRelation(DWORD* a1, DWORD* a2)
{
    return 2;  // 2 = hostile （敌对）
}

void InitPVPForceEnemyHook()
{
    DWORD gameBase = (DWORD)GetModuleHandleA(NULL);
    DWORD targetAddr = gameBase + PVP_RELATION_OFFSET;

    oGetRelation = (fnGetRelation)targetAddr;

    DWORD oldProtect;
    VirtualProtect((LPVOID)targetAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    *(BYTE*)targetAddr = 0xE9; // JMP
    *(DWORD*)(targetAddr + 1) = (DWORD)hkGetRelation - targetAddr - 5;

    VirtualProtect((LPVOID)targetAddr, 5, oldProtect, &oldProtect);

    MessageBoxA(0, "PVP 全图敌对 Hook 已注入", "NNO-PVP-Hook", 0);
}
