// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TimelineComponent.h>
#include "ShootableUIActor.generated.h"

class USceneComponent;
class UTextRenderComponent;
class UBoxComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UWidgetComponent;
class UAudioComponent;
class USoundBase;
class UCurveVector;
class UTimelineComponent;
class USoundBase;
class ABattlePlayer;
class APlayerController;
class AGun;
class ASword;
class ABattleResultActor;
class AAmbientSound;

UCLASS()
class IROAS_API AShootableUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AShootableUIActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	USceneComponent* rootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MySettings|Components")
	UTextRenderComponent* textRenderComp;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UBoxComponent* boxComp;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UNiagaraComponent* niagaraComp;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UWidgetComponent* widgetComp;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UAudioComponent* audioComp;


	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* appearSound;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	UNiagaraSystem* selectFX;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* selectSound;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* bulletClashSound;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* narrationSound;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* lobbySound;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	USoundBase* introSound;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	TSubclassOf<AActor> bp_LogoMesh;
	AActor* logoMesh;

	UFUNCTION()
	void OnOrbCrashed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Basic Orb
	void OnBasicOrbShot();
	UFUNCTION(BlueprintImplementableEvent)
	void OnFinishedOrbShot();

	// Lobby Start Orb
	void OnLobbyStartOrbShot();
	
	// StartTrigger Orb
	void OnStartTriggerSlashed();
	void OnStartTriggerShot(class AEnergyBullet* bullet);

	// Reward Orb
	UPROPERTY(EditAnywhere, Category = "MySettings")
	float energyRewardValue = 10.0f;
	UPROPERTY(EditAnywhere, Category = "MySettings")
	float healthRewardValue = 10.0f;
	UPROPERTY(EditAnywhere, Category = "MySettings")
	float attackRewardValue = 10.0f;

	void SetRewardText();
	void OnOrbActivate();
	void OnGettingReward();
	void GetHandedWeaponReward();
	void GetRewardActor(ABattleResultActor* ownerActor) { rewardActor = ownerActor; }
	
	// Floating Curve
	float curtime = 0.f;
	float floatingStartTime = 1.0f;
	bool bInFloating = false;
	FTimeline floatingTimeLine;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MySettings")
	UCurveVector* floatingCurve1;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	float floatingTimeLineLength;

	FTimeline textAppearTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MySettings")
	UCurveFloat* textAppearCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MySettings")
	float textAppearTime = 1.f;

	void CurveSetting();
	UFUNCTION()
	void FloatingCurveStart(FVector value);
	UFUNCTION()
	void FloatingCurveEnd();

	UFUNCTION()
	void OnTextAppear(UActorComponent* Component, bool bReset);
	UFUNCTION()
	void TextAppearProcess(float alpha);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MySettings")
	FName levelName = "Beta_StoryLevel";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MySettings")
	APlayerController* pc;
private:
	ABattlePlayer* player;
	ABattleResultActor* rewardActor;
};
