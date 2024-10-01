// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EditWidgetActor.generated.h"

UCLASS()
class IROAS_API AEditWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEditWidgetActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class USceneComponent* rootComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class UWidgetComponent* widgetComp;
	

	uint8 numStaticMeshComp = 1;

	class AEditorPlayer* editor;
	//class AEditInventoryWidgetActor* inventoryWidgetActor;
};
