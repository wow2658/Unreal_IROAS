

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IROASEnums.h"
#include "Hand.generated.h"

class USceneComponent;
class UMotionControllerComponent;
class UWidgetInteractionComponent;


UCLASS()
class IROAS_API AHand : public AActor
{
	GENERATED_BODY()
	
public:	
	AHand();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	USceneComponent* defaultSceneRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMotionControllerComponent* motionController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetInteractionComponent* widgetinteractor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* attachedActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRight = false;


public:
	void SetVRHandIsRight(bool isRight);
	const bool GetIsRight() { return bIsRight; }
	

	const FVector GetControllerWorldLocation();
	UMotionControllerComponent* GetMotionController() { return motionController; };

	virtual void InitializeVRHand(){}
};
