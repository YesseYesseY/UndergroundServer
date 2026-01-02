namespace Building
{
    void ServerCreateBuildingActorHook(AFortPlayerControllerAthena* PlayerController, const FCreateBuildingActorData& CreateBuildingData)
    {
        static auto GameState = (AFortGameStateAthena*)Utils::GetGameMode()->GameState;
        auto BuildClass = GameState->AllPlayerBuildableClasses[CreateBuildingData.BuildingClassHandle];
        auto Build = Utils::SpawnActor<ABuildingSMActor>(BuildClass, CreateBuildingData.BuildLoc, nullptr, CreateBuildingData.BuildRot);
        Build->InitializeKismetSpawnedBuildingActor(Build, PlayerController, true, nullptr, true);

        // TODO Remove colliding ABuildActors
    }

    void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit)
    {
        static auto EditToolItem = Utils::GetSoftPtr(Utils::GetAssetManager()->GameDataCommon->EditToolItem);
        if (auto ItemEntry = Inventory::FindItemEntry(PlayerController, EditToolItem))
        {
            auto Pawn = (AFortPlayerPawnAthena*)PlayerController->Pawn;
            Pawn->EquipWeaponDefinition(EditToolItem, ItemEntry->ItemGuid, {}, false);

            auto PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;
            BuildingActorToEdit->EditingPlayer = PlayerState;
            BuildingActorToEdit->OnRep_EditingPlayer();
        }
        else
        {
            PlayerController->ClientFailedToBeginEditingBuildingActor(BuildingActorToEdit);
        }
    }

    void ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToStopEditing)
    {
        BuildingActorToStopEditing->EditingPlayer = nullptr;
        BuildingActorToStopEditing->OnRep_EditingPlayer();
    }

    void ServerEditBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit, TSubclassOf<class ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored)
    {
        ABuildingSMActor* (*ReplaceBuildingActor)(ABuildingSMActor*, EBuildingReplacementType, TSubclassOf<ABuildingSMActor>, int, int, bool, AFortPlayerControllerAthena*)
            = decltype(ReplaceBuildingActor)(InSDKUtils::GetImageBase() + 0x889BD24);

        ReplaceBuildingActor(BuildingActorToEdit, EBuildingReplacementType::BRT_Edited, NewBuildingClass, 0, RotationIterations, bMirrored, PlayerController);
    }

    void ServerRepairBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToRepair)
    {
        auto Cost = UKismetMathLibrary::FFloor(UKismetMathLibrary::Lerp(7, 0, BuildingActorToRepair->GetHealthPercent()));
        BuildingActorToRepair->RepairBuilding(PlayerController, Cost);
    }
}
