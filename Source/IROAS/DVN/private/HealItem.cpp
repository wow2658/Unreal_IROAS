

#include "HealItem.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "BattlePlayer.h"

AHealItem::AHealItem()
{
	PrimaryActorTick.bCanEverTick = true;

	if (bp_niagaraSystem != nullptr) {
		niagaraComp->SetAsset(bp_niagaraSystem);
	}
}

void AHealItem::BeginPlay()
{
	Super::BeginPlay();

}

void AHealItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealItem::OnItemUsed()
{
	if (targetPlayer != nullptr) {
		targetPlayer->OnHeal(healAmount);
	}
}