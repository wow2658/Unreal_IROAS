// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditWidgetActor.h"
#include "EditMenuWidgetActor.generated.h"

UCLASS()
class IROAS_API AEditMenuWidgetActor : public AEditWidgetActor
{
	GENERATED_BODY()
	
public:
	AEditMenuWidgetActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class UEditMenuWidget* menuWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	TArray<class UStaticMeshComponent*> staticMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	TArray<class UEditStaticMeshComponent*> editstaticMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class UStaticMeshComponent* pressedCheckMesh;

	class AEditInventoryWidgetActor* inventoryWidgetActor;
	class AEditWidgetActor* parentptr;
};
