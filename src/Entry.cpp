#include "pch.h"
#include "memory/Tools.h"
#include "memory/Hooks.h"

DWORD WINAPI Entry(LPVOID hModule) {
    if (gd::init()) {
        Hook(
            gd::base + 0x1E4620,
            hooks::PauseLayer_customSetup,
            &gates::PauseLayer_customSetup,
            6
        ).enable();

        if (auto cocos = GetModuleHandleA("libcocos2d.dll")) {
            Hook(
                GetProcAddress(cocos, "?replaceScene@CCDirector@cocos2d@@QAE_NPAVCCScene@2@@Z"),
                hooks::CCDirector_replaceScene,
                &gates::CCDirector_replaceScene,
                5
            ).enable();
        }
        else goto exit;
    }
    else {
        exit:
        MessageBoxA(nullptr, "Could not find module base addresses. Exiting.", "", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 0);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        HANDLE _ = CreateThread(0, 0, Entry, hModule, 0, nullptr);
        if (_) CloseHandle(_);
    }
    return TRUE;
}