
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
�ʱ�ȭ
	1. �� ���̱�
	2. ���� �ʱ�ȭ
		2.1. ������ ����
**************************************************/
public:
	UFUNCTION(BlueprintCallable)
	void CompensateEyeHeight();
	virtual void InitializeVRHand() {}
	void InitializeHMD();

	void ReCallibrateCamera(const FRotator rot);

/**************************************************
	Input ���ε� �Լ���
		- hand üũ�� �� ���� �����Ǵ� �Լ� ȣ��
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
