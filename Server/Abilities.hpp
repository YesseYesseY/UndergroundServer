namespace Abilities
{
    void GiveAbilitySet(UAbilitySystemComponent* Component, UFortAbilitySet* AbilitySet)
    {
        for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
        {
            Component->K2_GiveAbility(AbilitySet->GameplayAbilities[i], 1, 1);
        }
        for (int i = 0; i < AbilitySet->GrantedGameplayEffects.Num(); i++)
        {
            auto Effect = AbilitySet->GrantedGameplayEffects[i];
            Component->BP_ApplyGameplayEffectToSelf(Effect.GameplayEffect, Effect.Level, Component->MakeEffectContext());
        }
    }

    void InternalServerTryActivateAbilityHook(UAbilitySystemComponent* Component, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData)
    {
        static FGameplayAbilitySpec* (*FindAbilitySpecFromHandle)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle) = decltype(FindAbilitySpecFromHandle)(InSDKUtils::GetImageBase() + 0x1EA3DB8);
    
        FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Component, Handle);
        if (!Spec)
        {
            Component->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
            return;
        }
    
        const UGameplayAbility* AbilityToActivate = Spec->Ability;
    
        if (!AbilityToActivate)
        {
            Component->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
            return;
        }
    
        if (AbilityToActivate->NetSecurityPolicy == EGameplayAbilityNetSecurityPolicy::ServerOnlyExecution ||
            AbilityToActivate->NetSecurityPolicy == EGameplayAbilityNetSecurityPolicy::ServerOnly)
        {
            Component->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
            return;
        }
    
        UGameplayAbility* InstancedAbility = nullptr;
        Spec->InputPressed = true;
    
        static bool (*InternalTryActivateAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, 
            UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
            = decltype(InternalTryActivateAbility)(InSDKUtils::GetImageBase() + 0x724A168);
    
        if (InternalTryActivateAbility(Component, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
        {
        }
        else
        {
            Component->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
            Spec->InputPressed = false;
            Utils::MarkArrayDirty(&Component->ActivatableAbilities);
        }
    }
}
