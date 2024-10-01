
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BattlePlayerAnimInstance.generated.h"

UCLASS()
class IROAS_API UBattlePlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector headLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector leftHandLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector rightHandLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator headRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator leftHandRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator rightHandRot;
	
};
