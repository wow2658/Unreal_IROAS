
#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "RangedGroundEnemy.generated.h"


UCLASS()
class IROAS_API ARangedGroundEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	ARangedGroundEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/********** BaseEnemy �Լ� **********/
	virtual void FindTarget() override;
	virtual void Move() override;
	virtual void ApplyDamage() override;
	virtual void OnTakeDamage() override;
	virtual void DieDown() override;

	virtual void PutObjectPool() override;

	//virtual void OnDamage_Interface(float DamageAmount) override;

	// ������Ʈ Ǯ źâ
	TArray<class ARangedEnemyProjectile*> magicBallPool;
	UPROPERTY(EditdefaultsOnly, Category = "EnemySettings|Projectile")
	TSubclassOf<class ARangedEnemyProjectile> magicBallFactory;
	uint8 magicBallPoolSize = 10;
	class ARangedEnemyProjectile* magicBall;


	UFUNCTION()
	void SetRangedMoveSpeed();

	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	float speedHigh = 300;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	float speedMiddle = 200;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	float speedLow = 150;


	UFUNCTION()
	void OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:
	class URangedEnemyAnim* rangedEnemyAnim;

public:
	// ******** ���Ÿ� ���� ���� �ݸ���

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	class UBoxComponent* spine_01Comp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	class UCapsuleComponent* upperarm_rComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	class UCapsuleComponent* upperarm_lComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	class UCapsuleComponent* thigh_rComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	class UCapsuleComponent* thigh_lComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	class UCapsuleComponent* foot_rComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
	class UCapsuleComponent* foot_lComp;

	// ******** ���׵� ����
	UFUNCTION()
	void OnRagdoll();

	UFUNCTION()
	void OffRagdoll(); 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "EnemySettings|Ragdoll")
	bool RagdollFlag = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 myCycle = 0;
};
