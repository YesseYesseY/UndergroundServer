#include <Minhook.h>

namespace Hook
{
    template <typename T = void*>
    void Function(uintptr_t Addr, void* Hook, T* Original = nullptr)
    {
        MH_CreateHook((LPVOID)Addr, Hook, (LPVOID*)Original);
        MH_EnableHook((LPVOID)Addr);
    }

    template <typename T, typename T2 = void*>
    void VTable(int32 Index, void* Hook, T2* Original = nullptr)
    {
        auto vTable = (void**)T::GetDefaultObj()->VTable;
        auto Addr = (LPVOID)(int64(vTable) + (Index * 8));
        if (Original)
            *Original = (T2)vTable[Index];
        DWORD old;
        VirtualProtect(Addr, 8, PAGE_EXECUTE_READWRITE, &old);
        vTable[Index] = Hook;
        VirtualProtect(Addr, 8, old, &old);
    }
}
