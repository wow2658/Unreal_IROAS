
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerHandAnim.generated.h"

class AWeapon;
class AGun;
class ASword;

UCLASS()
class IROAS_API UPlayerHandAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	float PoseAlphaGrasp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	float PoseAlphaIndexCurl = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	float PoseAlphaPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	float PoseAlphaThumbUp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	float CurrentPoseAlphaThumbUp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	float CurrentPoseAlphaPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	bool bMirror = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	bool bHoldingGun = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Animation")
	bool bHoldingSword = false;

	AWeapon* holdingWeapon = nullptr;
	AGun* gun = nullptr;
	ASword* sword = nullptr;

	void RecoilAnim();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayRecoilAnim(FName sectionName);
};
