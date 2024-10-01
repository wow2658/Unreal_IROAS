// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TimelineComponent.h>
#include "LobbyTutorialOrb.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UAudioComponent;
class UNiagaraSystem;
class USoundBase;
class UCurveVector;
class UTimelineComponent;
class ABattlePlayer;
class ALobbyTutorialActor;
class UBattlePlayerWidget;

UCLASS()
class IROAS_API ALobbyTutorialOrb : public AActor
{
	GENERATED_BODY()
	
public:	
	ALobbyTutorialOrb();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	USceneComponent* rootComp;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UBoxComponent* boxComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MySettings|Components")
	UNiagaraComponent* niagaraComp;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UAudioComponent* audioComp;

	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* appearSound;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	UNiagaraSystem* destroyFX;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* destroySound;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* bulletClashSound;

	void OnOrbBegin();
	void SetOrbScale(float scale);
	void OnOrbDestroyed();

	int8 destroyCount = 0;
	float newPercent = 0.1f;

	UFUNCTION()
	void OnOrbOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Floating Curve
	float curtime = 0.f;
	float floatingStartTime = 1.0f;
	bool bInFloating = false;
	struct FTimeline floatingTimeLine;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MySettings")
	UCurveVector* floatingCurve1;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	float floatingTimeLineLength;

	void CurveSetting();
	UFUNCTION()
	void FloatingCurveStart(FVector value);
	UFUNCTION()
	void FloatingCurveEnd();

	ALobbyTutorialActor* widgetActor;
	ABattlePlayer* player;

private:
};
