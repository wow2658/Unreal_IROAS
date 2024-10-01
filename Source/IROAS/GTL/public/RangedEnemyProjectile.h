
#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RangedEnemyProjectile.generated.h"

UCLASS()
class IROAS_API ARangedEnemyProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	ARangedEnemyProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	float magicDamage = 1.0f; // Spawner 에서 결정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	float magicLifeSpan = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	float magicSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	class USphereComponent* magicCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	class UStaticMeshComponent* magicMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	class UNiagaraComponent* magicFX;
	 
public:
	virtual void SetActive(bool isActive) override;
	void GetOwningEnemy(class ARangedGroundEnemy* enemy);

	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


private:
	class ARangedGroundEnemy* me;

};