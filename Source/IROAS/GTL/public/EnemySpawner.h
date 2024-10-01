
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IROASEnums.h"
#include "OnDamage_Interface.h"
#include <Components/TimelineComponent.h>
#include <Containers/Queue.h>
#include "EnemySpawner.generated.h"

class UBoxComponent;
class USceneComponent;
class USphereComponent;
class ABattleGameModeBase;
class ABattleGameStateBase;
class UStaticMesh;
class UParticleSystemComponent;
class UParticleSystem;
class UNiagaraSystem;
class UNiagaraComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class AMeleeEnemy;
class ARangedGroundEnemy;
class UStaticMeshComponent;
class AChaosCachePlayer;
class USoundBase;



UCLASS()
class IROAS_API AEnemySpawner : public AActor, public IOnDamage_Interface
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* rootComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* underGroundPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* actualPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* monolithCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* monolithMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USphereComponent* shieldcollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* shieldMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* shieldFX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* torchComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* riseSmoke;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* riseDebris;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//UParticleSystem* destructionEffect;


	bool bIsRising = false;
	float debrisCurrtime = 0.f;
	float debrisInterval = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* riseSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* destroySound;

public:	// �� ���� ����
	// ���� ü��(�ǰ� Ƚ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | ShieldAttributes")
	int32 shieldCountDown = 0;
	// ���� �ִ�ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | ShieldAttributes")
	float spawnerHP = 0;
	float spawnerCurrHP = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESpawnerState spawnerState = ESpawnerState::Inactive;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* meleeStatueMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* rangedStatueMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* meleeTorch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* rangedTorch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* meleeBurst;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* rangedBurst;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* impactFX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* statueHitImpact;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* impactSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInterface* sourceMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AChaosCachePlayer* destructionPlayerRef;


	// �ش� ���� Ȱ��ȭ �� ���̺�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | ShieldAttributes")
	uint8 activationWave = 0;

	FTimeline riseTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCurveFloat* riseCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float riseTime = 5.f;
	
	FVector riseStartLoc;

	FTimeline impactTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCurveFloat* impactCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float impactTime = 0.5f;
	UMaterialInstanceDynamic* mi_Dynamic;

	FTimeline shieldOnTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCurveFloat* shieldOnCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float shieldOnTime = 1.f;

	FTimeline shieldOffTimeline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCurveFloat* shieldOffCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float shieldOffTime = 1.f;

public: // �� ���� �Լ�
	UFUNCTION()
	virtual void OnDamage_Interface(float DamageAmount) override;
	UFUNCTION()
	void OnWaveStarted();
	UFUNCTION()
	void RiseProcess(float alpha);
	UFUNCTION()
	void OnRiseFinished();
	UFUNCTION()
	void DamageShield();
	UFUNCTION()
	void OnShieldHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnStatueHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ImpactProcess(float alpha);
	UFUNCTION()
	void DamageMonolith(float damage);
	UFUNCTION()
	void ShieldOnProcess(float alpha);
	UFUNCTION()
	void ShieldOffProcess(float alpha);
	UFUNCTION()
	void OnShieldDestroyed();
	UFUNCTION()
	void OnSpawnerDestroyed();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayChaosDestruction();

public:	// ������ ���ʹ� ����
	UPROPERTY()
	ABattleGameModeBase* gmb = nullptr;
	UPROPERTY()
	ABattleGameStateBase* gsb = nullptr;
	float currentTime = 0;

	// ������ �� �� ���� ������ ��
	UPROPERTY(VisibleAnywhere, Category = "Setting | Developer")
	uint8 spawnMaxNum = 6;

	// �ѹ��� ���� ������ ��
	UPROPERTY(EditAnywhere, Category = "Setting | WithShield")
	uint8 spawnNum = 2;
	// �ѹ��� ���� ������ ��(���������)
	UPROPERTY(EditAnywhere, Category = "Setting | NoShield")
	uint8 noShieldSpawnNum = 2;
	// ���� �ν����ڸ��� ù���̺� �߰�������
	UPROPERTY(EditAnywhere, Category = "Setting | NoShield")
	uint8 spawnBurstCnt = 2;
	bool bIsBurstActivated = false;
	// ���� ���� ������ ��
	uint8 currentSpawnNum = 0;

	// �������͹�
	UPROPERTY(EditAnywhere, Category = "Setting | WithShield")
	float spawnInterval = 2.0f;
	// �������͹�(���������)
	UPROPERTY(EditAnywhere, Category = "Setting | NoShield")
	float noShieldSpawnInterval = 2.0f;
	// ���罺�����͹�
	float currentSpawnInterval = 0.f;
	

	// ���ʹ� Ư��
	UPROPERTY(EditAnywhere, Category = "Setting | EnemyAttributes")
	EEnemyType enemyType = EEnemyType::None;
	UPROPERTY(EditAnywhere, Category = "Setting | EnemyAttributes")
	float enemyHP = 100;
	UPROPERTY(EditAnywhere, Category = "Setting | EnemyAttributes")
	float attackPower = 10;

	// DEPRECATED
	UPROPERTY(VisibleAnywhere, Category = "Setting | Deprecated")
	float KnockbackDuration = 0.25; //�˹�ð�
	UPROPERTY(VisibleAnywhere, Category = "Setting | Deprecated")
	float KnockbackDistance = 250; //�˹����
	UPROPERTY(EditAnywhere, Category = "Setting | Deprecated")
	float spawnZHeight = 82; //����Z�����



	uint8 maxMelee1PoolSize = 10;
	uint8 maxMelee2PoolSize = 10;
	uint8 maxMelee3PoolSize = 10;
	uint8 maxRangedGround1PoolSize = 7;
	uint8 maxRangedGround2PoolSize = 7;
	uint8 maxRangedAirPoolSize = 5;
	uint8 maxElitePoolSize = 3;

	uint8 spawnedMelee1Count = 0;
	uint8 spawnedRangedGround1Count = 0;


	// Melee1 ������Ʈ Ǯ
	TQueue<AMeleeEnemy*> Melee1Pool;
	UPROPERTY(EditDefaultsOnly, Category = SpawnSetting)
	TSubclassOf<AMeleeEnemy> Melee1Factory;
	AMeleeEnemy* Melee1;
	void SpawnMelee1();

	// RangedGround1 ������Ʈ Ǯ
	TQueue<ARangedGroundEnemy*> RangedGround1Pool;
	UPROPERTY(EditDefaultsOnly, Category = SpawnSetting)
	TSubclassOf<ARangedGroundEnemy> RangedGround1Factory;
	ARangedGroundEnemy* RangedGround1;
	void SpawnRangedGround1();

	// ���λ���
	UFUNCTION()
	void DestroyAll();
};
