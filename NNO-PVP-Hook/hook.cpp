#include <windows.h>

DWORD base = (DWORD)GetModuleHandleA(NULL);

// ★★★ 你 scanner.exe 扫描出来的偏移：0x00093A70
DWORD addr_PVP = base + 0x00093A70;

// 这个函数会强制让游戏认为所有玩家“敌对” → 全地图 PVP
int __declspec(naked) Hook_PVP()
{
    __asm {
        mov eax, 1      // 返回 1 = 敌对
        ret
    }
}

void PatchPVP()
{
    DWORD old;
    VirtualProtect((LPVOID)addr_PVP, 5, PAGE_EXECUTE_READWRITE, &old);

    DWORD relative = (DWORD)Hook_PVP - (addr_PVP + 5);
    BYTE patch[5] = {
        0xE9,
        (BYTE)(relative & 0xFF),
        (BYTE)((relative >> 8) & 0xFF),
        (BYTE)((relative >> 16) & 0xFF),
        (BYTE)((relative >> 24) & 0xFF)
    };

    memcpy((LPVOID)addr_PVP, patch, 5);
    VirtualProtect((LPVOID)addr_PVP, 5, old, &old);
}

DWORD WINAPI HookMain(LPVOID)
{
    Sleep(1000); // 等 GameServer 初始化完
    PatchPVP();
    return 0;
}
