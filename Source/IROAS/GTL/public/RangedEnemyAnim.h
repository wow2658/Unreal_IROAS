// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyAnim.h"
#include "RangedEnemyAnim.generated.h"


UCLASS()
class IROAS_API URangedEnemyAnim : public UBaseEnemyAnim
{
	GENERATED_BODY()


public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_magicBall_r();

	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_magicBall_l();
	
	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_RangedAttackEnd();

	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_Ranged_DamageStart();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DamageRandomInt_Ranged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackTypeInt_Ranged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IdleTypeInt_Ranged;

	class ARangedGroundEnemy* me;
};
