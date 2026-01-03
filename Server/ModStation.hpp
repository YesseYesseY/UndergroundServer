namespace ModStation
{
    void ServerPurchaseWeaponModForWeaponHook(UFortWeaponModStationComponent* Component, UFortWeaponModItemDefinition* WeaponMod, AFortWeapon* Weapon)
    {
        auto PlayerController = (AFortPlayerControllerAthena*)Component->GetOwner();
    
        static bool (*CanAddWeaponMod)(AFortWeapon*, UFortWeaponModItemDefinition*) = decltype(CanAddWeaponMod)(InSDKUtils::GetImageBase() + 0x9AF824C);
        if (!CanAddWeaponMod(Weapon, WeaponMod) || !Component->CanPlayerAffordModForWeapon(WeaponMod, Weapon, PlayerController))
            return;
    
        Inventory::AddModToWeapon(Weapon, WeaponMod);

        if (auto ItemEntry = Inventory::FindItemEntry(PlayerController, Weapon->ItemEntryGuid))
        {
            Inventory::AddPlayerNameToItemEntry(ItemEntry, PlayerController);
        }
    
        static auto GoldItemDef = Utils::GetSoftPtr(Utils::GetAssetManager()->GameDataBR->DefaultGlobalCurrencyItemDefinition);
    
        // TODO Get real mod cost
        Inventory::RemoveItem(PlayerController, GoldItemDef, 75);
    }
    
    void (*ServerStopInteractWithWorkbenchActorOriginal)(UFortWeaponModStationComponent* Component, AFortWeaponModStationBase* NewInteractingWeaponModStation);
    void ServerStopInteractWithWorkbenchActorHook(UFortWeaponModStationComponent* Component, AFortWeaponModStationBase* NewInteractingWeaponModStation)
    {
        auto PlayerController = (AFortPlayerControllerAthena*)Component->GetOwner();
        auto PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;
    
        for (auto Ability : PlayerState->AbilitySystemComponent->ActivatableAbilities.Items)
        {
            static UClass* AbilityClass = UObject::FindClass("BlueprintGeneratedClass GA_WeaponModStation_Camera_v2.GA_WeaponModStation_Camera_v2_C");
            if (Ability.Ability->IsA(AbilityClass))
            {
                PlayerState->AbilitySystemComponent->ClearAbility(Ability.Handle);
                break;
            }
        }
    
        ServerStopInteractWithWorkbenchActorOriginal(Component, NewInteractingWeaponModStation);
    }
}
