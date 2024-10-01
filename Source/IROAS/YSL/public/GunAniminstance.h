// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GunAniminstance.generated.h"

class AGun;
class UAnimMontage;

UCLASS()
class IROAS_API UGunAniminstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	bool bInRecoil = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	bool bInReload = false;

	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	UAnimMontage* recoilMontage;
	UPROPERTY(EditDefaultsOnly, Category = "AnimMontage")
	UAnimMontage* reloadMontage;

	AGun* gun;
	void RecoilAnim();
	void ReloadAnim();

	UFUNCTION()
	void AnimNotify_RecoilEnd();
	UFUNCTION()
	void AnimNotify_ReloadEnd();
};
