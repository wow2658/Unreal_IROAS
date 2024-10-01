
#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SlashProjectile.generated.h"

class ASword;
class UStaticMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class IROAS_API ASlashProjectile : public AProjectile
{
	GENERATED_BODY()
	
public:
	ASlashProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY()
	ASword* owningSword = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UStaticMeshComponent* collisionMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UNiagaraComponent* slashVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UNiagaraSystem* hitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	float slashDamage = 10.f;

	float slashLifeSpan = 5.f;

public:
	virtual void SetActive(bool isActive) override;

	UFUNCTION()
	void OnEnemyHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
