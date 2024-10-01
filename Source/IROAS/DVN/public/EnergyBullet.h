
#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "EnergyBullet.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AGun;
class UParticleSystemComponent;
class UParticleSystem;
class UNiagaraSystem;

UCLASS()
class IROAS_API AEnergyBullet : public AProjectile
{
	GENERATED_BODY()

public:
	AEnergyBullet();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	float bulletDamage = 5.0f;
	float bulletLifeSpan = 3.0f;
	float bulletSpeed = 2000.0f;
	
	UPROPERTY()
	AGun* firingGun = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	USphereComponent* sphereComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	UStaticMeshComponent* meshComp;
	UPROPERTY(VisibleAnywhere)
    UParticleSystemComponent* bulletTrail;

	UPROPERTY(EditAnywhere)
	UParticleSystem* enemyHitFX1;
	UPROPERTY(EditAnywhere)
	UParticleSystem* enemyHitFX2;
	UPROPERTY(EditAnywhere)
	USoundBase* enemyHitSound;
	
	UPROPERTY(EditAnywhere)
	USoundBase* woodHitSound;
	UPROPERTY(EditAnywhere)
	UParticleSystem* woodHitFX;

	UPROPERTY(EditAnywhere)
	USoundBase* groundHitSound;
	UPROPERTY(EditAnywhere)
	UParticleSystem* groundHitFX;
	
	UPROPERTY(EditAnywhere)
	USoundBase* rockHitSound;
	UPROPERTY(EditAnywhere)
	UParticleSystem* rockHitFX;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDebugging = false;

public:
	virtual void SetActive(bool isActive) override;
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
