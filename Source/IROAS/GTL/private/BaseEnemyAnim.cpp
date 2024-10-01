#include "BaseEnemyAnim.h"
#include "BaseEnemy.h"
#include <GameFramework/FloatingPawnMovement.h>
#include "BattleGameModeBase.h"
#include "MeleeEnemy.h"


void UBaseEnemyAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	enemy = Cast<ABaseEnemy>(GetOwningActor());
	if (enemy != nullptr)
	{
		MeleeEnemy = Cast<AMeleeEnemy>(enemy);
		moveComp = enemy->pawnMovementComp;
		AActor* OwningActor = GetOwningActor();
		if (!OwningActor) return;

		// Get the game mode
		GameMode = Cast<ABattleGameModeBase>(OwningActor->GetWorld()->GetAuthGameMode());

	}

}

void UBaseEnemyAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	//if (enemy != nullptr && moveComp != nullptr)
	//	speed = moveComp->Velocity.Size2D();

	if (enemy != nullptr)
	{
		Iamvisible_Anim = enemy->IamInvisible;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("%s"), bAttackPlay ? TEXT("True") : TEXT("false")));
}

void UBaseEnemyAnim::AnimNotify_AttackStart()
{
	//bAttackPlay = true;
	if (MeleeEnemy)
	{
		MeleeEnemy->SetCollision(true);
	}
}

void UBaseEnemyAnim::AnimNotify_AttackEnd()
{
	//bAttackPlay = false;
	if (MeleeEnemy)
	{
		MeleeEnemy->SetCollision(false);
	}
}

void UBaseEnemyAnim::AnimNotify_MeleeDieEnd()
{
	
	
	if (GameMode)
	{
		//GameMode->CurrentEnemyCount -= 1;
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("MeleeDie GameMode->CurrentEnemyCount %d"), GameMode->CurrentEnemyCount));
	}
}

void UBaseEnemyAnim::AnimNotify_MeleeAttackEnd()
{
	AttackTypeInt = FMath::RandRange(0, 2);
}

void UBaseEnemyAnim::AnimNotify_Melee_DamageStart()
{
	
	runTypeInt = (runTypeInt + 1) % 5;

}

void UBaseEnemyAnim::AnimNotify_RangedDieEnd()
{
	if (GameMode)
	{
		//GameMode->CurrentEnemyCount -= 1;
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("RangedDie GameMode->CurrentEnemyCount %d"), GameMode->CurrentEnemyCount));
	}
}

void UBaseEnemyAnim::AnimNotify_MeleeRageEnd()
{
	if (MeleeEnemy)
	{
		MeleeEnemy->enemyState = EEnemyState::Move;
		animState = EEnemyState::Move;
	}
}
