namespace Inventory
{
    void GiveItem(AFortPlayerControllerAthena* PlayerController, UFortWorldItemDefinition* ItemDef, int32 Count = -1, UFortWeaponModSetData* ModSetData = nullptr)
    {
        if (!ItemDef)
            return;

        if (Count == -1)
        {
            auto PlayerState = (AFortPlayerStateAthena*)PlayerController;
            Count = ItemDef->GetMaxStackSize(PlayerState->AbilitySystemComponent);
        }

        auto Item = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count, 1);

        // if (ModSetData)
        // {
        //     TArray<struct FFortModSetModEntry> Mods;
        //     if (ItemDef->Rarity == EFortRarity::Common) Mods = ModSetData->CommonRarityMods;
        //     else if (ItemDef->Rarity == EFortRarity::Uncommon) Mods = ModSetData->UncommonRarityMods;
        //     else if (ItemDef->Rarity == EFortRarity::Rare) Mods = ModSetData->RareRarityMods;
        //     else if (ItemDef->Rarity == EFortRarity::Epic) Mods = ModSetData->EpicRarityMods;
        //     else if (ItemDef->Rarity == EFortRarity::Legendary) Mods = ModSetData->LengdaryRarityMods; // Lengdary

        //     for (auto Mod : Mods)
        //     {
        //         Item->ItemEntry.WeaponModSlots.Add({ Mod.ModDefinition, true });
        //     }

        //     // TWeakObjectPtr<UObject> Yes;
        //     // Yes.ObjectIndex = ModSetData->Index;
        //     // static FGameplayTag* NameThing = (FGameplayTag*)(InSDKUtils::GetImageBase() + 0x117B6870);
        //     // Item->ItemEntry.StateValuesConstObject.Add({ *NameThing, Yes });

        //     Item->ItemEntry.LoadedAmmo = 3;
        // }

        PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
        PlayerController->WorldInventory->Inventory.ItemInstances.Add(Item);
    }

    void RemoveItem(AFortPlayerControllerAthena* PlayerController, int32 Index, int32 Count = -1)
    {
        auto& Entry = PlayerController->WorldInventory->Inventory.ReplicatedEntries[Index];

        if (Count == -1)
            Count = Entry.Count;

        if (Entry.Count - Count <= 0)
        {
            PlayerController->WorldInventory->Inventory.ReplicatedEntries.Remove(Index);
            PlayerController->WorldInventory->Inventory.ItemInstances.Remove(Index);
            Utils::MarkArrayDirty(&PlayerController->WorldInventory->Inventory);
        }
        else
        {
            Entry.Count -= Count;
            Utils::MarkItemDirty(&PlayerController->WorldInventory->Inventory, &Entry);
        }
    }

    void RemoveItem(AFortPlayerControllerAthena* PlayerController, UFortWorldItemDefinition* ItemDef, int32 Count = -1)
    {
        for (int i = 0; i < PlayerController->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            auto Entry = PlayerController->WorldInventory->Inventory.ReplicatedEntries[i];
            if (Entry.ItemDefinition == ItemDef)
            {
                RemoveItem(PlayerController, i, Count);
                break;
            }
        }
    }

    void Update(AFortPlayerControllerAthena* PlayerController)
    {
        PlayerController->WorldInventory->HandleInventoryLocalUpdate();
        Utils::MarkArrayDirty(&PlayerController->WorldInventory->Inventory);
    }
    
    void ServerExecuteInventoryItemHook(AFortPlayerControllerAthena* PlayerController, const FGuid& ItemGuid)
    {
        auto Pawn = (AFortPlayerPawnAthena*)PlayerController->Pawn;

        for (int i = 0; i < PlayerController->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            auto Entry = PlayerController->WorldInventory->Inventory.ReplicatedEntries[i];
            if (UKismetGuidLibrary::EqualEqual_GuidGuid(Entry.ItemGuid, ItemGuid))
            {
                auto Weapon = Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry.ItemDefinition, ItemGuid, {}, false);
                break;
            }
        }
    }

    void ServerAttemptInventoryDrop(AFortPlayerControllerAthena* PlayerController, const FGuid& ItemGuid, int32 Count, bool bTrash)
    {
        for (int i = 0; i < PlayerController->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            auto Entry = PlayerController->WorldInventory->Inventory.ReplicatedEntries[i];
            if (UKismetGuidLibrary::EqualEqual_GuidGuid(Entry.ItemGuid, ItemGuid))
            {
                auto Pawn = (AFortPlayerPawnAthena*)PlayerController->Pawn;
                auto PickupLocation = Pawn->K2_GetActorLocation();
                auto Pickup = Utils::SpawnActor<AFortPickupAthena>(PickupLocation);
                Pickup->PrimaryPickupItemEntry = PlayerController->WorldInventory->Inventory.ItemInstances[i]->CreateItemEntryForDrop();
                Pickup->PrimaryPickupItemEntry.Count = Count;
                Pickup->TossPickup(PickupLocation, Pawn, Count, true, true, EFortPickupSourceTypeFlag::Player /*| EFortPickupSourceTypeFlag::Tossed*/, EFortPickupSpawnSource::TossedByPlayer);

                RemoveItem(PlayerController, i, Count);
                break;
            }
        }
    }

    void ServerHandlePickup(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, const FFortPickupRequestInfo& Params_0)
    {
        // TODO
        Pickup->K2_DestroyActor();
    }
}
