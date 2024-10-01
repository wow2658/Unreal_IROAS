#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "Animation/AnimMontage.h"
#include "MeleeEnemy.generated.h"

class USphereComponent;
class UBoxComponent;
class UCapsuleComponent;


UCLASS()
class IROAS_API AMeleeEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AMeleeEnemy();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 공격용 콜리전
	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	USphereComponent* leftHandCollision;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	USphereComponent* rightHandCollision;
	// Move Speed

	UFUNCTION()
	void SetMeleeMoveSpeed();

	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	float speedHigh = 500;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	float speedMiddle = 400;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	float speedLow = 350;



	UFUNCTION()
	void OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SetCollision(bool isActive);

	/********** BaseEnemy 함수 **********/
	//virtual void FindTarget() override;
	virtual void Move() override;
	virtual void ApplyDamage() override;
	virtual void OnTakeDamage() override;
	//virtual void Die() override;
	virtual void DieDown() override;
	virtual void PutObjectPool() override;

	//virtual void SetSpawner(class AEnemySpawner* owingSpawner) override;


	// ******** 근거리 전용 부위 콜리전

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	UBoxComponent* spine_01Comp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	UCapsuleComponent* upperarm_rComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	UCapsuleComponent* upperarm_lComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	UCapsuleComponent* thigh_rComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	UCapsuleComponent* thigh_lComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	UCapsuleComponent* foot_rComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	UCapsuleComponent* foot_lComp;

	// ******** 레그돌 관련
	UFUNCTION(BlueprintCallable)
	void OnRagdoll();

	UFUNCTION()
	void OffRagdoll();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "EnemySettings|Ragdoll")
	bool RagdollFlag = false;

	UFUNCTION()
	void MeleeSpawnSound();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Components")
	int32 myCycle = 0;
};
