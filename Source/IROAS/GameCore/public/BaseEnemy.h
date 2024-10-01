#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IROASEnums.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OnDamage_Interface.h"
#include "BaseEnemy.generated.h"

class ABattleGameStateBase;

UCLASS()
class IROAS_API ABaseEnemy : public APawn, public IOnDamage_Interface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings")
	bool bIsAlreadyDead = false;

	/********** ������Ʈ **********/

	UPROPERTY(VisibleDefaultsOnly, Category = "EnemySettings|Components")
	class UCapsuleComponent* capsuleComp;
	UPROPERTY(VisibleDefaultsOnly, Category = "EnemySettings|Components")
	class USkeletalMeshComponent* meshComp;
	UPROPERTY(VisibleDefaultsOnly, Category = "EnemySettings|Components")
	class UFloatingPawnMovement* pawnMovementComp;


	//*********** �پ��� �ǰݰ����� �ݸ��� ������Ʈ�� **************
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings|Components")
    class USphereComponent* HeadComp;

	

	bool InOverlapDelay = false;

	//�׽�Ʈ�� �ؽ�Ʈ����
	UPROPERTY(VisibleDefaultsOnly, Category = "EnemySettings|Components")
	class UTextRenderComponent* curState;

	/********** Enemy �Ӽ� **********/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="debug")
	float hp;
	float attackDelay; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="debug")
	float attackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="debug")
	float attackPower;
	float IdleDelay;
	float damageDelay;
	float DownDelay;
	EEnemyType enemyType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="debug")
	EEnemyState enemyState = EEnemyState::Idle;

	/********** Enemy ���� �Լ� **********/
	virtual void FindTarget();
	virtual void Move();
	virtual void ApplyDamage();
	virtual void OnTakeDamage();
	virtual void DieDown();

	virtual void RageState();
	bool RageTypeIntFlag = false;


	// �ǰݽ� ȣ��� �ݹ�(�̺�Ʈ) �Լ�
	virtual void OnDamageProcess(float deal);

	virtual void OnDamage_Interface(float DamageAmount) override; //{}

	// object Ǯ�� �ִ� �Լ�
	virtual void SetActive(bool isActive);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings")
	bool IamInvisible = true;

	virtual void PutObjectPool() {};
	virtual void SetSpawner(class AEnemySpawner* owingSpawner);

	bool GetRandomPosInNavMesh(FVector center, float radius, FVector& dest);
	FString EnumToString(EEnemyState state);

//private:

	class ABattlePlayer* target;
	class AAIController* aiController;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	class UBaseEnemyAnim* enemyAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	class URangedEnemyAnim* RangedGenemyAnim;

	class AEnemySpawner* spawner;
	FVector direction;
	FVector randomDest;
	float dist = 0;
	float curTime = 0;

//�˹���
	UPROPERTY(EditDefaultsOnly, Category = "EnemySettings|Components")
	bool bIsKnockbackActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Components")
	float KnockbackDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Components")
	float KnockbackDistance = 250.0f;

	UPROPERTY()
	float Alpha;

	UPROPERTY()
	FVector NewPosition;

	UPROPERTY()
	FVector KnockbackDirection;

	UPROPERTY()
	FVector StartPosition;

	UPROPERTY()
	FVector EndPosition;

	UFUNCTION()
	void StartKnockback();

//�˹����� ����ȣȯ ���׵�+���޽����
	FVector CalculateVelocity(AActor* OtherActor, const FVector& CurrentLocation);

	bool overlapFlag = false;

	//World Y
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "EnemySettings|Ragdoll")
	float RagdollBackPower = 600000.f;
	//World Z
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "EnemySettings|Ragdoll")
	float RagdollUpPower = 500000.f;

// �ƿ����α��

	 // �⺻ ������ �����ϱ� ���� ����
    UPROPERTY(VisibleAnywhere)
    UMaterialInterface* OriginalMaterial;

    // �������� ������ �����ϱ� ���� Dynamic Material Instance
    UPROPERTY(VisibleAnywhere)
    UMaterialInstanceDynamic* HitOverlayMaterialInstance;

	UPROPERTY(VisibleAnywhere)
    UMaterialInstanceDynamic* DieOverlayMaterialInstance;

	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void OnHitOverlayMaterial();

	UFUNCTION(BlueprintCallable, Category = BossFuntions)
	void OnDieOverlayMaterial();

	void ResetMaterial();

	FTimerHandle TimerHandle_ResetMaterial_FromHit;
	FTimerHandle TimerHandle_ResetMaterial_FromRage;
	FTimerHandle TimerHandle_StopAnim;

public:
	// ��Ÿ�� ���� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Animation")
    UAnimMontage* HitFromTestRockMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Animation")
    UAnimMontage* HitFromBulletMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Animation")
    UAnimMontage* HitFromSwordMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Animation")
    UAnimMontage* HitFromSlashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Animation")
    UAnimMontage* DieFromTestRockMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Animation")
    UAnimMontage* DieFromBulletMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Animation")
	UAnimMontage* DieFromSwordMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySettings|Animation")
	UAnimMontage* DieFromSlashMontage;

public:
	UPROPERTY()
	ABattleGameStateBase* gsb = nullptr;
};