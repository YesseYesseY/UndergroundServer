namespace Net
{
    int64 GetNetModeHook(int64 a1)
    {
        return 1;
    }

    void SendClientMoveAdjustments(UNetDriver* NetDriver)
    {
        for (UNetConnection* Connection : NetDriver->ClientConnections)
        {
            if (Connection == nullptr || Connection->ViewTarget == nullptr)
            {
                continue;
            }
    
            static void (*SendClientAdjustment)(APlayerController*) = decltype(SendClientAdjustment)(InSDKUtils::GetImageBase() + 0x663CE28);
    
            if (APlayerController* PC = Connection->PlayerController)
            {
                SendClientAdjustment(PC);
            }
    
            for (UNetConnection* ChildConnection : Connection->Children)
            {
                if (ChildConnection == nullptr)
                {
                    continue;
                }
    
                if (APlayerController* PC = ChildConnection->PlayerController)
                {
                    SendClientAdjustment(PC);
                }
            }
        }
    }

    enum class EReplicationSystemSendPass : unsigned
    {
        Invalid,
        PostTickDispatch,
        TickFlush,
    };
    
    struct FSendUpdateParams
    {
        EReplicationSystemSendPass SendPadd;
        float DeltaSeconds;
    };

    void (*TickFlushOriginal)(UNetDriver* NetDriver, float DeltaSeconds);
    void TickFlushHook(UNetDriver* NetDriver, float DeltaSeconds)
    {
        auto ReplicationSystem = *(UReplicationSystem**)(int64(NetDriver) + 0x748);
    
        if (NetDriver->ClientConnections.Num() > 0 && ReplicationSystem)
        {
            static void (*UpdateIrisReplicationViews)(UNetDriver*) = decltype(UpdateIrisReplicationViews)(InSDKUtils::GetImageBase() + 0x6577FB4);
            static void (*PreSendUpdate)(UReplicationSystem*, const FSendUpdateParams&) = decltype(PreSendUpdate)(InSDKUtils::GetImageBase() + 0x58675D8);
    
            UpdateIrisReplicationViews(NetDriver);
            SendClientMoveAdjustments(NetDriver);
            PreSendUpdate(ReplicationSystem, { EReplicationSystemSendPass::TickFlush, DeltaSeconds });
        }
    
        TickFlushOriginal(NetDriver, DeltaSeconds);
    }
    
    bool KickPlayerHook(int64 a1, int64 a2, int64 a3)
    {
        return false;
    }
}
