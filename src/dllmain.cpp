// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"

static constexpr char paReplaceScene[]{"?replaceScene@CCDirector@cocos2d@@QAE_NPAVCCScene@2@@Z"};
static constexpr char paSetScaleX[]{ "?setScaleX@CCNode@cocos2d@@UAEXM@Z" };
static constexpr char paSetScaleY[]{ "?setScaleY@CCNode@cocos2d@@UAEXM@Z" };
static constexpr char paSharedDirector[]{ "?sharedDirector@CCDirector@cocos2d@@SAPAV12@XZ" };
static constexpr char paGetRunningScene[]{ "?getRunningScene@CCDirector@cocos2d@@QAEPAVCCScene@2@XZ" };

//should be __thiscall, but how do i do that? id probably have to import the cocos library

using tReplaceScene = bool(__thiscall*)(void* CCDirector, void* CCScene);
using tSetScale = void(__thiscall*)(void* CCNode, float fScale);
using tSharedDirector = void* (__cdecl*)();
using tGetRunningScene = void* (__thiscall*)(void* CCDirector);

tReplaceScene oReplaceScene;
tReplaceScene oRSbuff;
tSetScale setScaleX;
tSetScale setScaleY;
tSharedDirector sharedDirector;
tGetRunningScene getRunningScene;
uintptr_t* playLayerPtr{};
void* director{};
void* currScene{};
const float fScaleX{ 1.33f };
const float fScaleY{ 1.0f };


bool __fastcall hkReplaceScene(void* This, void* garbage, void* CCScene) {
    if (*playLayerPtr) {
        if (CCScene == (void*)mem::FindDMAAddy((uintptr_t)playLayerPtr, {0xB4, 0x0})) {
            setScaleX(CCScene, fScaleX);
            setScaleY(CCScene, fScaleY);
        }
    }
    return oReplaceScene(This, CCScene);
}

BOOL WINAPI HackThread(HMODULE hModule) {
    bool bToggled{ true };
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Hello.\n\nPress TAB to toggle on and off. Hack is enabled at startup.\nPress END to close the hack.\n\n";
    //returns 0 always, not sure what's up here

    HMODULE cocosModuleHandle{ GetModuleHandle(L"libcocos2d.dll") };
    uintptr_t moduleHandle{ (uintptr_t)GetModuleHandle(L"GeometryDash.exe") };

    oReplaceScene = (tReplaceScene)GetProcAddress(cocosModuleHandle, paReplaceScene);
    setScaleX = (tSetScale)GetProcAddress(cocosModuleHandle, paSetScaleX);
    setScaleY = (tSetScale)GetProcAddress(cocosModuleHandle, paSetScaleY);
    sharedDirector = (tSharedDirector)GetProcAddress(cocosModuleHandle, paSharedDirector);
    getRunningScene = (tGetRunningScene)GetProcAddress(cocosModuleHandle, paGetRunningScene);
    oRSbuff = oReplaceScene;
    playLayerPtr = (uintptr_t*)mem::FindDMAAddy((moduleHandle + 0x3222D0), {0x164}) ;
    uintptr_t RSrelativeAddr{};
    if (!oReplaceScene && !setScaleX && !sharedDirector && !getRunningScene) {
        std::cout << "Couldn't find functions.\n";
        goto end;
    }
    director = sharedDirector();
    oReplaceScene = (tReplaceScene)mem::TrampHook32((BYTE*)oReplaceScene, (BYTE*)hkReplaceScene, 5, RSrelativeAddr);
    while (!(GetAsyncKeyState(VK_END) & 1)) {
        if (GetAsyncKeyState(VK_TAB) & 1) {
            bToggled = !bToggled;
            if (bToggled) {
                std::cout << "Hack toggled on\n\n";
                if (*playLayerPtr) {
                    if (getRunningScene(director) == (void*)mem::FindDMAAddy((uintptr_t)playLayerPtr, { 0xB4, 0x0 })) {
                        setScaleX(getRunningScene(director), fScaleX);
                        setScaleY(getRunningScene(director), fScaleY);
                    }
                }
                mem::Patch((BYTE*)oRSbuff, (BYTE*)"\xE9", 1);
                mem::Patch((BYTE*)oRSbuff + 1, (BYTE*)&RSrelativeAddr, 4);

            }
            if (!bToggled) {
                std::cout << "Hack toggled off\n\n";
                if (*playLayerPtr) {
                    if (getRunningScene(director) == (void*)mem::FindDMAAddy((uintptr_t)playLayerPtr, { 0xB4, 0x0 })) {
                        setScaleX(getRunningScene(director), 1.0f);
                        setScaleY(getRunningScene(director), 1.0f);
                    }
                }
                mem::Patch((BYTE*)oRSbuff, (BYTE*)"\x55\x8B\xEC\x53\x56", 5);
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD1)) {
            std::cout << "Current Scene is 0x" << getRunningScene(director) << '\n';
        }
        Sleep(50);
    }
end:
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


