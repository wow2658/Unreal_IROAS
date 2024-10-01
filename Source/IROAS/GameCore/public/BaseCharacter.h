
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HeadMountedDisplayTypes.h"
#include "IROASEnums.h"
#include "BaseCharacter.generated.h"

class USceneComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class UInputMappingContext;
class AHand;
class APlayerHand;
class APlayerController;
struct FInputActionValue;

UCLASS()
class IROAS_API ABaseCharacter : public APawn
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	 USceneComponent* defaultSceneRoot = nullptr; 
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	USceneComponent* vrRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	UCameraComponent* camera = nullptr;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	//TSubclassOf<AVRTechHand> _vrHandClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings|Components")
	UFloatingPawnMovement* floatingPawnMovment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AHand> vrHandClass;

	//AHand* leftHand = nullptr;
	//AHand* rightHand = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	TEnumAsByte<EHMDTrackingOrigin::Type> trackingOrigin = EHMDTrackingOrigin::Floor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	float rotationSnapAngle = .0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	float desiredEyeHeight = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Inputs")
	UInputMappingContext* inputMapping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerSettings|Inputs")
	TArray<class UInputAction*> inputActions;

	APlayerController* pc;
	

/**************************************************
초기화
	1. 손 붙이기
	2. 헤드셋 초기화
		2.1. 눈높이 보정
**************************************************/
public:
	UFUNCTION(BlueprintCallable)
	void CompensateEyeHeight();
	virtual void InitializeVRHand() {}
	void InitializeHMD();

	void ReCallibrateCamera(const FRotator rot);

/**************************************************
	Input 바인딩 함수들
		- hand 체크한 뒤 각각 대응되는 함수 호출
**************************************************/
public:
	// Left
	virtual void HandleLeftGripPressed() {}
	virtual void HandleLeftGripReleased() {}
	virtual void HandleLeftTriggerPressed() {}
	virtual void HandleLeftTriggerReleased() {}
	virtual void HandleLeftThumbstickAxis(const struct FInputActionValue& Value) {}
	virtual void HandleLeftThumbstickLeft() {}
	virtual void HandleLeftThumbstickRight() {}
	virtual void HandleLeftMenu() {}

	// Right
	virtual void HandleRightGripPressed() {}
	virtual void HandleRightGripReleased() {}
	virtual void HandleRightTriggerPressed() {}
	virtual void HandleRightTriggerReleased() {}
	virtual void HandleRightThumbstickAxis(const struct FInputActionValue& Value) {}
	virtual void HandleRightThumbstickLeft() {}
	virtual void HandleRightThumbstickRight() {}
	
};
