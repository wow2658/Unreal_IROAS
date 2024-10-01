
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WidgetManagerBase.generated.h"

class ABaseCharacter;
class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IROAS_API UWidgetManagerBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWidgetManagerBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	ABaseCharacter* baseCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> defaultWidgetClass;

		
};
