#include <Windows.h>
#include <iostream>

#include <Hook.hpp>
#include <Utils.hpp>

void ReturnHook()
{
    return;
}

int64 GetNetModeHook(int64 a1)
{
    return 1;
}

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    MH_Initialize();

    Utils::ExecuteConsoleCommand(L"log LogFortUIDirector None");

    Hook::Function(InSDKUtils::GetImageBase() + 0x3641180, ReturnHook); // RequestExit
    Hook::Function(InSDKUtils::GetImageBase() + 0x38930E0, ReturnHook); // GameSession crash
    Hook::Function(InSDKUtils::GetImageBase() + 0x118C5B0, GetNetModeHook);

    *(bool*)(InSDKUtils::GetImageBase() + 0x1164007B) = false; // GIsClient
    *(bool*)(InSDKUtils::GetImageBase() + 0x1164000D) = true; // GIsServer
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);

    Utils::ExecuteConsoleCommand(L"open Helios_Terrain");

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
