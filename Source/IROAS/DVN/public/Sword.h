
#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Sword.generated.h"

class UBoxComponent;
class UArrowComponent;
class AProjectile;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;
class ASlashProjectile;
class UPhysicsConstraintComponent;
class UStaticMeshComponent;

UCLASS()
class IROAS_API ASword : public AWeapon
{
	GENERATED_BODY()

public:
	ASword();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* handleRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bladeCollision = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UArrowComponent* slashDir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraComponent* slashEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraComponent* swordTrail;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UPhysicsConstraintComponent* constraints;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	//UNiagaraSystem* hitEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	UParticleSystem* hitEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* swingSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* slashModeSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* slashSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* clashSounds_Rock;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* clashSounds_Weapons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* sliceSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haptics")
	UHapticFeedbackEffect_Base* slashChargeHaptic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haptics")
	UHapticFeedbackEffect_Base* swordSliceHaptic;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float normalAttackDamage = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float slashDamage = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float slashProjectileSpeed = 2500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float absorbAmount = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float energyCost = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minSwingSpeed = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minSlashSpeed = 2000.f;
	float topSwingSpeed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float slashCoolTime = 1.f;
	bool bIsSlashMode = false;
	bool bIsCoolTime = false;
	FVector slashPoint = FVector::ZeroVector;
	FVector prevPoint = FVector::ZeroVector;
	FVector prevSlashLoc;
	FVector curSlashLoc;
	FRotator prevSlashRot;
	FRotator curSlashRot;

	FTimerHandle slashAttackTimer;
	FTimerHandle swingSoundTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASlashProjectile> bp_testprojectile;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// TQueue는 UPROPERTY 적용할수없음
	TQueue<ASlashProjectile*> slashPool;


public:
	UFUNCTION()
	void OnBladeCollided(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	/*
	UFUNCTION()
	void OnMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );
	*/
	
	void PlaySwingSound();

	void AbsorbEnergy();

	virtual void Attack() override;

	void SlashAttack(const FTransform& slashTrans);

	void EndSlash();

	void SetActive(bool activate) override;
	
};
