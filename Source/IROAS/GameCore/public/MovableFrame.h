
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include <Components/TimelineComponent.h>
#include "MovableFrame.generated.h"

class USceneComponent;
class UArrowComponent;
class UNiagaraComponent;
class ABaseCharacter;
class UBoxComponent;
class ABattlePlayerIndicator;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPositionUpdateSignature, const TArray<bool>&, occupation);

UCLASS()
class IROAS_API AMovableFrame : public APlayerStart
{
	GENERATED_BODY()
	
public:
	AMovableFrame(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick( float DeltaSeconds ) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UArrowComponent* snapPoint_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UArrowComponent* snapPoint_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UArrowComponent* snapPoint_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UArrowComponent* snapPoint_4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UArrowComponent* snapPoint_5;

	TArray<UArrowComponent*> snapPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UNiagaraComponent* snapRing_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UNiagaraComponent* snapRing_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UNiagaraComponent* snapRing_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UNiagaraComponent* snapRing_4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UNiagaraComponent* snapRing_5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UBoxComponent* boxComp;

	UPROPERTY()
	TArray<bool> occupation = {false, false,  false,  false, false};

	UPROPERTY()
	ABaseCharacter* player_1 = nullptr;
	int8 playerLoc_1;

	UPROPERTY()
	ABaseCharacter* player_2 = nullptr;
	int8 playerLoc_2;

	UPROPERTY()
	FPositionUpdateSignature onPositionChanged;

	UPROPERTY()
	ABattlePlayerIndicator* playerIndicator;

	FTimeline moveTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCurveFloat* moveCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float moveTime = 1.f;
	FVector currPosition;
	FVector nextPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* moveSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* immovableSound;

public:
	UFUNCTION()
	void InitializePlayerPostitions();
	UFUNCTION()
	void ResetPosition();
	UFUNCTION()
	void MovePlayerLeft(ABaseCharacter* player);
	UFUNCTION()
	void MovePlayerRight(ABaseCharacter* player);

	int8 FindAvailableLocationLeft(int8 currLocation);
	int8 FindAvailableLocationRight(int8 currLocation);


	bool TutorialCheckRightMove();
	bool TutorialCheckMiddleMove();

	UFUNCTION()
	void MoveProcess(float alpha);
	UFUNCTION()
	void OnMoveFinished();
};
