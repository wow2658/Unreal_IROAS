
#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

class USkeletalMeshComponent;
class USceneComponent;
class UBoxComponent;
class UTextRenderComponent;
class AEnergyBullet;
class AHand;
class UParticleSystem;
class USoundBase;
class UAnimationAsset;
class UAnimInstance;
class UAnimMontage;
class UNiagaraSystem;
class UHapticFeedbackEffect_Base;

UCLASS()
class IROAS_API AGun : public AWeapon
{
	GENERATED_BODY()

public:
	AGun();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* gunMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* firePos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* reloadPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* bulletGaugeBar1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* bulletGaugeBar2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextRenderComponent* bulletGaugeNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* bulletReloadArrowRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* bulletReloadArrowLeft;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnergyBullet> energyBullet_bp;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// TQueue적용, UPROPERTY사용불가
	TQueue<AEnergyBullet*> bulletPool;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GunSettings")
	float gunDamage = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA")
	float bulletSpeed = 6000.f;

	UPROPERTY(VisibleAnywhere)
    UParticleSystem* muzzleFX_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* fireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* emptySound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* spawnFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* releaseSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* releaseFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haptics")
	UHapticFeedbackEffect_Base* fireHaptic;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* reloadAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* recoilHandAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* recoilGunAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimInstance* gun_ABP;

	int32 bulletCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 maxBulletNum = 6;
	int32 bulletPoolSize = 30;
	float fireDelay = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInDelay = false;
	FTimerHandle fireDelayTimer;
	FTimerHandle reloadDelayTimer;
	// 장전 회전값 구하기 위한 초기 회전값
	FRotator initReloadRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA")
	float rightHandReload = 600;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA")
	float leftHandReload = -600;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minReloadSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QA")
	float energyCost = 30.f;
	bool bInReload = false;
	bool bReloadPos = false;
	bool bInFire = false;
	bool blink = true;
	bool isBlink = false;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Attack() override;
	void SetBulletCount(int32 bulletNum) { bulletCount = bulletNum; }
	void CheckSpeed(float Deltatime);
	UFUNCTION(BlueprintCallable)
	void Reload();
	void IncreaseBulletDamage(float bulletDamage);
	UFUNCTION()
	void SetWidget(int32 bulletNum);

	void SetActive(bool activate) override;
	void SetReloadWidget();
};
