#include <Windows.h>
#include <iostream>

#include <Utils.hpp>
#include <Hook.hpp>

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

    Utils::ExecuteConsoleCommand(L"net.AllowEncryption 0");
    *(bool*)(InSDKUtils::GetImageBase() + 0x117E1128) = true;

    Utils::ExecuteConsoleCommand(L"open 127.0.0.1");
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
