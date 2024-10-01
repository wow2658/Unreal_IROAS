
#pragma once

#include "CoreMinimal.h"
#include "PlayerComponentBase.h"
#include "SnapMovementComponent.generated.h"


class AMovableFrame;

UCLASS()
class IROAS_API USnapMovementComponent : public UPlayerComponentBase
{
	GENERATED_BODY()

public:
	USnapMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY()
	AMovableFrame* moveableFrame_1 = nullptr;
	UPROPERTY()
	AMovableFrame* moveableFrame_2 = nullptr;
	UPROPERTY()
	AMovableFrame* moveableFrame_3 = nullptr;
	UPROPERTY(VisibleAnywhere)
	AMovableFrame* currentFrame = nullptr;
	

public:
	UFUNCTION()
	void SwitchMovableFrame();
	UFUNCTION()
	void MoveLeft();
	UFUNCTION()
	void MoveRight();
};
