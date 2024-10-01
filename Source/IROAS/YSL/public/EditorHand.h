// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hand.h"
#include "EditorHand.generated.h"


UCLASS()
class IROAS_API AEditorHand : public AHand
{
	GENERATED_BODY()
	
public:
	AEditorHand();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	class UStaticMeshComponent* controllerMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	class UStaticMeshComponent* niddlePointer;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	class UStaticMesh* rightMesh;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	class UStaticMesh* leftMesh;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	class UChildActorComponent* menuWidgetComp;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	class UChildActorComponent* inventoryWidgetComp;
	/*UPROPERTY(EditDefaultsOnly, Category = Settings)
	class UNiagaraComponent* pointerFX;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	class UNiagaraComponent* pointerImpactPointFX;*/
	TArray<FVector> pointerLinePositions;
	class AEditorPlayer* editor;
	class AMovableFrame* movableFrame;
	class AEditMenuWidgetActor* menuWidgetActor;
	class AEditInventoryWidgetActor* inventoryWidgetActor;
	class AEditableActor* interactableActor;
	
	bool bisPlacing = false;
	bool bCanSelect = false;
	bool bSpawnTrace = false;
	bool bSelectTrace = true;

	float debugLineAlpha = 0;

	virtual void InitializeVRHand() override;
	void InitializeWidget();
	void SpawnLineTrace();
	void SelectLineTrace();
	void DrawPointerLine();
};
