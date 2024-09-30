// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "IROASEnums.h"
#include "BossAnim.generated.h"

class ABoss;
class UBossFSM;


UCLASS()
class IROAS_API UBossAnim : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeInitializeAnimation() override;


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= FSM)
	EEnemyState BossAnimState = EEnemyState::Idle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= FSM)
	uint8 bPatternCnt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= FSM)
	uint8 brandomMovecnt;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= FSM)
	int32 bhitcnt;
	// Owner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Owner)
	class ABoss* Boss;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Owner)
	class ABossSlash* BossSlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FSM)
	bool SpawnReady = false;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = MySettings)
	bool inAction_Anim = false;

	UFUNCTION()
	void AnimNotify_Attack_Rock();
	
	UFUNCTION()
	void AnimNotify_End_Rock();
	
	UFUNCTION()
	void AnimNotify_Attack_Slash();
	
	UFUNCTION()
	void AnimNotify_End_Slash();

	UFUNCTION()
	void AnimNotify_Die();

	UPROPERTY()
	bool CanGoRock = false;




};
