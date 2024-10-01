// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleResultActor.generated.h"

class USceneComponent;
class UWidgetComponent;
class UChildActorComponent;
class AShootableUIActor;
class URankingBoardWidget;

UCLASS()
class IROAS_API ABattleResultActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABattleResultActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	USceneComponent* rootComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings|Components")
	UWidgetComponent* widgetComp;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UChildActorComponent* childComp1;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UChildActorComponent* childComp2;
	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	UChildActorComponent* childComp3;

	URankingBoardWidget* resultWidget;

	TArray<AShootableUIActor*> orbActors;

	bool rankingScale = false;

	void RankingActor();
	void FailActor();

private:
	
};
