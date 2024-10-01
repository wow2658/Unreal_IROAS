// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TimelineComponent.h>
#include "IROASEnums.h"
#include "BattlePlayerIndicator.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UAudioComponent;
class UMaterialInstanceDynamic;
class UCurveFloat;
class AMovableFrame;
class USoundBase;

UCLASS()
class IROAS_API ABattlePlayerIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	ABattlePlayerIndicator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* rootComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* indicator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* indicatorMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioComponent* audioComp;

	struct FTimeline indicatorTimeline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor indicationColor = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* indicationCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float indicationTime = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInstanceDynamic*> matArray;
		
	EIndicatorType currentType = EIndicatorType::Default;

	UFUNCTION()
	void IndicateStart(EIndicatorType indicationType);
	UFUNCTION()
	void FinishIndication();
	UFUNCTION()
	void OnStartIndication(float output);
	UFUNCTION()
	void OnFinishIndication();
	UFUNCTION()
	void HealthUnder30();
	UFUNCTION()
	void HealthUnder10();
	UFUNCTION()
	void SetDefaultIndicator();
	UFUNCTION()
	void DieStopAudio();

	void SetMaterialArray();

};
