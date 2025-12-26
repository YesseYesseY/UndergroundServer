#include <Windows.h>
#include <iostream>

#include <Hook.hpp>
#include <Utils.hpp>

void RequestExitHook()
{
    return;
}

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    MH_Initialize();

    Hook::Function(InSDKUtils::GetImageBase() + 0x3641180, RequestExitHook);

    auto GameViewport = UEngine::GetEngine()->GameViewport;
    GameViewport->ViewportConsole = Utils::SpawnObject<UConsole>(GameViewport);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
        break;
    }

    return TRUE;
}
