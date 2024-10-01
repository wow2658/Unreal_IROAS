
#pragma once

#include "CoreMinimal.h"
#include "PlayerComponentBase.h"
#include "InputActionValue.h"
#include "BasicMovementComponent.generated.h"

UCLASS()
class IROAS_API UBasicMovementComponent : public UPlayerComponentBase
{
	GENERATED_BODY()

public:
	UBasicMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Move(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
	
};
