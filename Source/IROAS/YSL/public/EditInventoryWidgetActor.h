// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditWidgetActor.h"
#include "IROASEnums.h"
#include "EditInventoryWidgetActor.generated.h"

UCLASS()
class IROAS_API AEditInventoryWidgetActor : public AEditWidgetActor
{
	GENERATED_BODY()


public:
	AEditInventoryWidgetActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	class UEditInventoryWidget* inventoryWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	TArray<class UEditStaticMeshComponent*> staticMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	TArray<TSubclassOf<class AEditableActor>> spawnable_BP;

	//EStaticMeshType staticMeshType;

	/*UFUNCTION()
	void ChangeVisibility(EStaticMeshType meshType);*/
};
