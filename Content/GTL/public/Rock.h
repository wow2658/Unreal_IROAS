// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Rock.generated.h"

/**
 * 
 */
UCLASS()
class IROAS_API ARock : public AProjectile
{
	GENERATED_BODY()

public:
	ARock();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY()
	float RockDelayTime = 2;

	float Rockhp = 30;



public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	class UStaticMeshComponent* RockMesh;


	//virtual void ApplyDamage(float DamageAmount) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner)
	class ABoss* Boss;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner)
	class UBossAnim* BossAnim;
};
