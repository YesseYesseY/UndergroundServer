namespace GameLogic
{
    static inline UFortGameStateComponent_BattleRoyaleGamePhaseLogic* Logic = nullptr;

    double GetTimeSeconds()
    {
        return UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
    }
    
    void Tick(UNetDriver* NetDriver)
    {
        if (!Logic)
            return;

        auto Time = GetTimeSeconds();
        if (Logic->GamePhase == EAthenaGamePhase::Warmup)
        {
            if (Time > Logic->WarmupCountdownEndTime)
            {
                // TODO Aircraft
                for (auto Player : NetDriver->ClientConnections)
                {
                    Player->PlayerController->Pawn->K2_TeleportTo({ 0, 0, 3000 }, {});
                }

                Logic->GamePhase = EAthenaGamePhase::None;
                Logic->OnRep_GamePhase(EAthenaGamePhase::Warmup);
            }
        }
    }

    void InitWarmupTimer()
    {
        Logic = UFortGameStateComponent_BattleRoyaleGamePhaseLogic::Get(UWorld::GetWorld());

        auto Time = GetTimeSeconds();
        Logic->WarmupCountdownStartTime = Time;
        Logic->WarmupCountdownEndTime = Time + 60;
        Logic->WarmupCountdownDuration = 60;
        Logic->WarmupEarlyCountdownDuration = 10; // ?
    }
}
