namespace ModStation
{
    void ServerPurchaseWeaponModForWeaponHook(UFortWeaponModStationComponent* Component, UFortWeaponModItemDefinition* WeaponMod, AFortWeapon* Weapon)
    {
        auto PlayerController = (AFortPlayerControllerAthena*)Component->GetOwner();
    
        if (!Component->CanPlayerAffordModForWeapon(WeaponMod, Weapon, PlayerController))
            return;
    
        Inventory::AddModToWeapon(Weapon, WeaponMod);

        if (auto ItemEntry = Inventory::FindItemEntry(PlayerController, Weapon->ItemEntryGuid))
        {
            static FGameplayTag* NameThing = (FGameplayTag*)(InSDKUtils::GetImageBase() + 0x117B6870);
            TWeakObjectPtr<UObject> Yes;
            Yes.ObjectIndex = PlayerController->PlayerState->Index;
            Yes.ObjectSerialNumber = Utils::GetSerialNumber(PlayerController->PlayerState);
            ItemEntry->StateValuesConstObject.Add({ *NameThing, Yes });
            Inventory::Update(PlayerController, ItemEntry);
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
