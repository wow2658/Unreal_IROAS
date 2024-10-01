
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IROASEnums.h"
#include "Weapon.generated.h"

class USceneComponent;
class UGrabComp;
class UStaticMeshComponent;
class ABattlePlayer;
class AHand;
class USoundBase;

UCLASS()
class IROAS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

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
	UGrabComp* grabComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float attackEnergy = 10.0f;

	ABattlePlayer* owningPlayer = nullptr;

	UPROPERTY()
	EWeaponType weaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* spawnSound;

	bool bisRightHand = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive = false;

private:


public:
	virtual void Attack() {}
	virtual void SetActive(bool activate) {}
};
