
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class IROAS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY()
	float attackDamage;
	float lifeSpan = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float speed = 0.0f;;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float currentTime = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsActive = false;


public:
	UFUNCTION(BlueprintCallable)
	virtual void OnComponentBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void SetActive(bool isActive) {}


};
