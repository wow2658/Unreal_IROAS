
#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "HealItem.generated.h"

UCLASS()
class IROAS_API AHealItem : public AItemBase
{
	GENERATED_BODY()

public:
	AHealItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float healAmount = 20.f;

public:
	virtual void OnItemUsed() override;
	
};
