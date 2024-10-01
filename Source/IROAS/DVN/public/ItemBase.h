
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../GameCore/public/IROASEnums.h"
#include "ItemBase.generated.h"

class ABattlePlayer;
class USceneComponent;
class UStaticMeshComponent;
class UGrabComp;
class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class IROAS_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* defaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* meshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* hoverComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* detectionComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGrabComp* grabComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* niagaraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* bp_niagaraSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABattlePlayer* targetPlayer = nullptr;

	EItemType itemType = EItemType::None;

public:
	UFUNCTION()
	virtual void OnHovered();
	UFUNCTION()
	virtual void OnUnhovered();
	UFUNCTION()
	virtual void OnItemUsed() {}
};
