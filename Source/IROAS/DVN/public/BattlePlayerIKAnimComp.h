
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattlePlayerIKAnimComp.generated.h"

class ABattlePlayer;
class UBattlePlayerAnimInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IROAS_API UBattlePlayerIKAnimComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBattlePlayerIKAnimComp();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY()
	ABattlePlayer* player;
	UPROPERTY()
	UBattlePlayerAnimInstance* playerAnim;
		
};
