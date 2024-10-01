// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EditorPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaticVisibleOff);

UCLASS()
class IROAS_API AEditorPlayer : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEditorPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	class UTextRenderComponent* textRender;
	/*UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	class UBasicMovementComponent* moveComp;*/

	float currentZoomRate = 1.0f;
	float prevZoomRate = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Settings")
	float dist = 0;
	bool checkStartDist = false;
	int8 currentCameraIndex = 0;
	int8 prevCameraIndex = 3;
	FVector spawnLocation = FVector::ZeroVector;
	FRotator spawnRoatation = FRotator::ZeroRotator;
	FRotator relocateRotation = FRotator::ZeroRotator;
	
	class AEditableActor* spawnableActor = nullptr;
	class AEditableSpawner* editableSpawner;
	class AEditableProp* editableProp;


	class UMaterialInstanceDynamic* matInstance;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<class AEditableActor*> interactableActors;
	class AEditableActor* interactableActor;
	TSubclassOf<class AEditableActor> spawnableActor_bp = nullptr;

	class AEditorHand* leftHand;
	class AEditorHand* rightHand;
	FTransform lHandPrevTrans;
	FTransform rHandPrevTrans;
	class AEditGameStateBase* gameState;
	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<class AEditorZoomSpline*> zoomSplines;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	class USoundBase* cantSpawnSound;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	class USoundBase* canSpawnSound;


	UPROPERTY(BlueprintAssignable, Category = MySetting)
	FOnStaticVisibleOff OnStaticVisibleOff;

	virtual void InitializeVRHand() override;
	void ZoomIn();
	void SwitchCamera(bool sth);
	void GetZoomSplines();
	void SetEditableSpawner(class AEditableSpawner* spawner);
	void SetEditableProp(class AEditableProp* prop);
	void SaveSpawnerInfo();
	void SavePropInfo();

public:
	// Left
	virtual void HandleLeftGripPressed() override;
	virtual void HandleLeftGripReleased() override;
	//virtual void HandleLeftTriggerPressed() override;
	//virtual void HandleLeftTriggerReleased() override;
	//virtual void HandleLeftThumbstickAxis(const FInputActionValue& Value) override;
	virtual void HandleLeftThumbstickLeft() override;
	virtual void HandleLeftThumbstickRight() override;
	virtual void HandleLeftMenu() override;

	// Right
	virtual void HandleRightGripPressed() override;
	virtual void HandleRightGripReleased() override;
	virtual void HandleRightTriggerPressed() override;
	virtual void HandleRightTriggerReleased() override;
	virtual void HandleRightThumbstickAxis(const FInputActionValue& Value) override;
	//virtual void HandleRightThumbstickLeft() override;
	//virtual void HandleRightThumbstickRight() override;
	void HandleRightAButtonPressed();
	void HandleRightBButtonPressed();

	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	bool bLeftGripped = false;
	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	bool bRightGripped = false;

	bool bRelocate = false;
};
