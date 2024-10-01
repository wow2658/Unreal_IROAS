// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedEnemyAnim.h"
#include "RangedGroundEnemy.h"
#include "RangedEnemyProjectile.h"
#include "BattlePlayer.h"
#include "Camera/CameraComponent.h"
#include <GameFramework/FloatingPawnMovement.h>
#include "Kismet/KismetMathLibrary.h"

void URangedEnemyAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	me = Cast<ARangedGroundEnemy>(GetOwningActor());
	if (me != nullptr)
		moveComp = me->pawnMovementComp;
}

void URangedEnemyAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (me != nullptr && moveComp != nullptr)
		speed = moveComp->Velocity.Size2D();
}

void URangedEnemyAnim::AnimNotify_magicBall_l()
{
	FVector leftMagicLoc = me->meshComp->GetSocketLocation(FName("magicBall_l"));
	if (me->magicBallPool.Num() <= 1) return;
	ARangedEnemyProjectile* spawnedMagicBall_l = me->magicBallPool[0];
	spawnedMagicBall_l->SetActive(true);
	me->magicBallPool.RemoveAt(0);
	if (spawnedMagicBall_l != nullptr) {
		spawnedMagicBall_l->SetActorLocation(leftMagicLoc);

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(leftMagicLoc,me->target->camera->GetComponentLocation());
		spawnedMagicBall_l->SetActorRotation(LookAtRotation);

	}
}

void URangedEnemyAnim::AnimNotify_magicBall_r()
{
	FVector rightMagicLoc = me->meshComp->GetSocketLocation(FName("magicBall_r"));

	if (me->magicBallPool.Num() <= 1) return;

	ARangedEnemyProjectile* spawnedMagicBall_r = me->magicBallPool[1];
	spawnedMagicBall_r->SetActive(true);
	me->magicBallPool.RemoveAt(1);
	if (spawnedMagicBall_r != nullptr) {
		spawnedMagicBall_r->SetActorLocation(rightMagicLoc);

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(rightMagicLoc, me->target->camera->GetComponentLocation());
		spawnedMagicBall_r->SetActorRotation(LookAtRotation);
	}
}

void URangedEnemyAnim::AnimNotify_RangedAttackEnd()
{
	me->curTime = 0;
	//bAttackPlay = false;
	AttackTypeInt_Ranged = FMath::RandRange(0,1);
	IdleTypeInt_Ranged = FMath::RandRange(0,3);
	bAttackPlay = false;
}

void URangedEnemyAnim::AnimNotify_Ranged_DamageStart()
{
	DamageRandomInt_Ranged = FMath::RandRange(0, 3);
	runTypeInt = (runTypeInt + 1) % 5;

}