// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "IROASEnums.h"
#include "EditStaticMeshComponent.generated.h"


UCLASS()
class IROAS_API UEditStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	class AEditInventoryWidgetActor* InventoryWidgetActor;
	class AEditMenuWidgetActor* MenuWidgetActor;

	UPROPERTY(EditDefaultsOnly, Category = MySettings)
	class UEditButtonWidget* owingButton;
	UPROPERTY(EditDefaultsOnly, Category = MySettings)
	EStaticMeshType staticMeshType;

	class AEditorPlayer* editor;

	UFUNCTION()
	void ChangeVisibility(EStaticMeshType meshType);
	UFUNCTION()
	void CategoryChangeVisibility(bool canVisible);
	UFUNCTION()
	void VisibilityOff();
	UFUNCTION()
	void BindDelegate();
};