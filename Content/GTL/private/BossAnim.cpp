// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnim.h"
#include "Boss.h"
#include "BossSlash.h"

void UBossAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Boss = Cast<ABoss>(TryGetPawnOwner());

}

void UBossAnim::AnimNotify_Attack_Rock()
{
	CanGoRock = true;
}

void UBossAnim::AnimNotify_End_Rock()
{
	CanGoRock = false;
}

void UBossAnim::AnimNotify_Attack_Slash()
{
	Boss->PutObjectPool();
}

void UBossAnim::AnimNotify_End_Slash()
{
	if (Boss->BossFSM->mBossState != EEnemyState::Dead)
	{
		BossAnimState = EEnemyState::Idle;
		Boss->BossFSM->mBossState = BossAnimState;
		inAction_Anim = false;
	}
	
}

void UBossAnim::AnimNotify_Die()
{
	BossAnimState = EEnemyState::Dead;
	Boss->BossFSM->mBossState = EEnemyState::Dead;
}
