#include <SDK/FortniteGame_classes.hpp>
using namespace SDK;

namespace Utils
{
    template <typename T>
    T* SpawnObject(UObject* Outer)
    {
        return (T*)UGameplayStatics::SpawnObject(T::StaticClass(), Outer);
    }

    template <typename T = AActor>
    T* SpawnActor(UClass* ActorClass, FVector Pos = {}, AActor* Owner = nullptr, FRotator Rot = {}, FVector Scale = { 1, 1, 1 })
    {
        auto translivesmatter = UKismetMathLibrary::MakeTransform(Pos, Rot, Scale);

        auto ret = UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), ActorClass, translivesmatter, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner, ESpawnActorScaleMethod::MultiplyWithRoot);

        if (ret)
            ret = UGameplayStatics::FinishSpawningActor(ret, translivesmatter, ESpawnActorScaleMethod::MultiplyWithRoot);

        return (T*)ret;
    }

    template <typename T>
    T* SpawnActor(FVector Pos = {}, AActor* Owner = nullptr, FRotator Rot = {}, FVector Scale = { 1, 1, 1 })
    {
        return SpawnActor<T>(T::StaticClass(), Pos, Owner, Rot, Scale);
    }

    void ExecuteConsoleCommand(const wchar_t* Cmd)
    {
        UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), Cmd, nullptr);
    }

    void MarkArrayDirty(void* Arr)
    {
        static void (*NativeFunc)(void*) = decltype(NativeFunc)(InSDKUtils::GetImageBase() + 0x168F3EC);
        NativeFunc(Arr);
    }

    void MarkItemDirty(void* Arr, void* Item)
    {
        static void (*NativeFunc)(void*, void*) = decltype(NativeFunc)(InSDKUtils::GetImageBase() + 0x168F3C8);
        NativeFunc(Arr, Item);
    }

    UFortAssetManager* GetAssetManager()
    {
        return (UFortAssetManager*)UEngine::GetEngine()->AssetManager;
    }

    template <typename T>
    TArray<T*> GetAllActorsOfClass()
    {
        TArray<AActor*> ret;
        UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), T::StaticClass(), &ret);
        return *(TArray<T*>*)(&ret);
    }

    AFortGameModeBR* GetGameMode()
    {
        return (AFortGameModeBR*)UWorld::GetWorld()->AuthorityGameMode;
    }
}
