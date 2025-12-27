#include <Windows.h>
#include <iostream>

#include <Utils.hpp>
#include <Hook.hpp>

void RequestExitHook()
{
    return;
}

bool (*UWorldExecOriginal)(UWorld* World, int64 a2, const wchar_t* Cmd, int64 a4);
bool UWorldExecHook(UWorld* World, int64 a2, const wchar_t* Cmd, int64 a4)
{
    if (wcscmp(Cmd, L"givemecheats") == 0)
    {
        auto PlayerController = World->OwningGameInstance->LocalPlayers[0]->PlayerController;
        PlayerController->CheatManager = Utils::SpawnObject<UCheatManager>(PlayerController);
        return true;
    }

    return UWorldExecOriginal(World, a2, Cmd, a4);
}

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    MH_Initialize();

    Hook::Function(InSDKUtils::GetImageBase() + 0x3641180, RequestExitHook);
    Hook::Function(InSDKUtils::GetImageBase() + 0x2465198, UWorldExecHook, &UWorldExecOriginal);

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
