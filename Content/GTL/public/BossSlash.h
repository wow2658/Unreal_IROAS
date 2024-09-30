// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "BossSlash.generated.h"

/**
 * 
 */
UCLASS()
class IROAS_API ABossSlash : public AProjectile
{
	GENERATED_BODY()

public:
	ABossSlash();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	class UCapsuleComponent* SlashCollision;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MySettings)
	class UStaticMeshComponent* SlashMesh;*/


	float slashDamage = 10.0f;
	float slashLifeSpan = 3.0f;
	float slashSpeed = 1000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner)
	class ABoss* Boss;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner)
	class UBossAnim* BossAnim;

	virtual void SetActive(bool isActive) override;
	


};
